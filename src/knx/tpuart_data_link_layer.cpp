#include "config.h"
#ifdef USE_TP

#include "tpuart_data_link_layer.h"
#include "bits.h"

#include "device_object.h"
#include "address_table_object.h"
#include "cemi_frame.h"

#include <stdio.h>
#include <string.h>

// Activate trace output
// #define DBG_TRACE

// NCN5130
#define NCN5130

// services Host -> Controller :
// internal commands, device specific
#define U_RESET_REQ 0x01
#define U_STATE_REQ 0x02
#define U_SET_BUSY_REQ 0x03
#define U_QUIT_BUSY_REQ 0x04
#define U_BUSMON_REQ 0x05
#define U_SET_ADDRESS_REQ 0xF1 // different on TP-UART
#define U_SET_REPETITION_REQ 0xF2
#define U_L_DATA_OFFSET_REQ 0x08 //-0x0C
#define U_SYSTEM_STATE 0x0D
#define U_STOP_MODE_REQ 0x0E
#define U_EXIT_STOP_MODE_REQ 0x0F
#define U_ACK_REQ 0x10 //-0x17
#define U_ACK_REQ_NACK 0x04
#define U_ACK_REQ_BUSY 0x02
#define U_ACK_REQ_ADRESSED 0x01
#define U_CONFIGURE_REQ 0x18
#define U_INT_REG_WR_REQ 0x28
#define U_INT_REG_RD_REQ 0x38
#define U_POLLING_STATE_REQ 0xE0

// knx transmit data commands
#define U_L_DATA_START_CONT_REQ 0x80 //-0xBF
#define U_L_DATA_END_REQ 0x40        //-0x7F

// serices to host controller

// DLL services (device is transparent)
#define L_DATA_STANDARD_IND 0x90
#define L_DATA_EXTENDED_IND 0x10
#define L_DATA_MASK 0xD3
#define L_POLL_DATA_IND 0xF0

// acknowledge services (device is transparent in bus monitor mode)
#define L_ACKN_IND 0x00
#define L_ACKN_MASK 0x33
#define L_DATA_CON 0x0B
#define L_DATA_CON_MASK 0x7F
#define SUCCESS 0x80

// control services, device specific
#define U_RESET_IND 0x03
#define U_STATE_IND 0x07
#define SLAVE_COLLISION 0x80
#define RECEIVE_ERROR 0x40
#define TRANSMIT_ERROR 0x20
#define PROTOCOL_ERROR 0x10
#define TEMPERATURE_WARNING 0x08
#define U_FRAME_STATE_IND 0x13
#define U_FRAME_STATE_MASK 0x17
#define PARITY_BIT_ERROR 0x80
#define CHECKSUM_LENGTH_ERROR 0x40
#define TIMING_ERROR 0x20
#define U_CONFIGURE_IND 0x01
#define U_CONFIGURE_MASK 0x83
#define AUTO_ACKNOWLEDGE 0x20
#define AUTO_POLLING 0x10
#define CRC_CCITT 0x80
#define FRAME_END_WITH_MARKER 0x40
#define U_FRAME_END_IND 0xCB
#define U_STOP_MODE_IND 0x2B
#define U_SYSTEM_STAT_IND 0x4B

// tx states
enum
{
    TX_IDLE,
    TX_FRAME,
    TX_WAIT_CONN
};

// rx states
enum
{
    RX_WAIT_START,
    RX_L_ADDR,
    RX_L_DATA,
    RX_WAIT_EOP
};

#define EOP_TIMEOUT 2  // milli seconds; end of layer-2 packet gap
#ifndef EOPR_TIMEOUT   // allow to set EOPR_TIMEOUT externally
#define EOPR_TIMEOUT 8 // ms; relaxed EOP timeout; usally to trigger after NAK
#endif
#define CONFIRM_TIMEOUT 500 // milli seconds
#define RESET_TIMEOUT 100   // milli seconds
#define TX_TIMEPAUSE 0      // 0 means 1 milli seconds

#ifndef OVERRUN_COUNT
#define OVERRUN_COUNT 7 // bytes; max. allowed bytes in receive buffer (on start) to see it as overrun
#endif

// If this threshold is reached loop() goes into
// "hog mode" where it stays in loop() while L2 address reception
#define HOGMODE_THRESHOLD 3 // milli seconds

void TpUartDataLinkLayer::enterRxWaitEOP()
{
    // Flush input
    while (uartAvailable())
    {
        readUart();
    }
    _lastByteRxTime = HAL_GetTick();
    _rxState = RX_WAIT_EOP;
}

void TpUartDataLinkLayer::loop()
{
    if (!_enabled)
    {
        if (_waitConfirmStartTime == 0)
        {
            if (HAL_GetTick() - _lastResetChipTime > 1000)
            {
                // reset chip every 1 seconds
                _lastResetChipTime = HAL_GetTick();
                _enabled = resetChip();
            }
        }
        else
        {
            _enabled = resetChipTick();
        }
    }

    if (!_enabled)
        return;

    // Loop once and repeat as long we have rx data available
    do
    {
        // Signals to communicate from rx part with the tx part
        uint8_t dataConnMsg = 0; // The DATA_CONN message just seen or 0

#ifdef KNX_WAIT_FOR_ADDR
        // After seeing a L2 packet start, stay in loop until address bytes are
        // received and the AK/NAK packet is sent
        bool stayInRx = true;
#elif defined(KNX_AUTO_ADAPT)
        // After seeing a L2 packet start, stay in loop until address bytes are
        // received and the AK/NAK packet is sent, when last loop call delayed
        // by more than HOGMODE_THRESHOLD
        bool stayInRx = HAL_GetTick() - _lastLoopTime > HOGMODE_THRESHOLD;
        _lastLoopTime = HAL_GetTick();
#else
        // After seeing a L2 packet start, leave loop and hope the loop
        // is called early enough to do further processings
        bool stayInRx = false;
#endif

        // Loop once and repeat as long we are in the receive phase for the L2 address
        do
        {
            uint8_t *buffer = _receiveBuffer + 2;
            uint8_t rxByte;
            switch (_rxState)
            {
            case RX_WAIT_START:
                if (uartAvailable())
                {
                    if (uartAvailable() > OVERRUN_COUNT)
                    {
                        print("input buffer overrun: ");
                        print(uartAvailable());
                        print("\r\n");
                        enterRxWaitEOP();
                        break;
                    }
                    rxByte = readUart();
#ifdef DBG_TRACE
                    print(rxByte, HEX);
#endif
                    _lastByteRxTime = HAL_GetTick();

                    // Check for layer-2 packets
                    _RxByteCnt = 0;
                    _xorSum = 0;
                    if ((rxByte & L_DATA_MASK) == L_DATA_STANDARD_IND)
                    {
                        buffer[_RxByteCnt++] = rxByte;
                        _xorSum ^= rxByte;
                        _RxByteCnt++; // convert to L_DATA_EXTENDED
                        _convert = true;
                        _rxState = RX_L_ADDR;
#ifdef DBG_TRACE
                        print("RLS\r\n");
#endif
                        break;
                    }
                    else if ((rxByte & L_DATA_MASK) == L_DATA_EXTENDED_IND)
                    {
                        buffer[_RxByteCnt++] = rxByte;
                        _xorSum ^= rxByte;
                        _convert = false;
                        _rxState = RX_L_ADDR;
#ifdef DBG_TRACE
                        print("RLX\r\n");
#endif
                        break;
                    }

                    // Handle all single byte packets here
                    else if ((rxByte & L_DATA_CON_MASK) == L_DATA_CON)
                    {
                        dataConnMsg = rxByte;
                    }
                    else if (rxByte == L_POLL_DATA_IND)
                    {
                        // not sure if this can happen
                        print("got L_POLL_DATA_IND\r\n");
                    }
                    else if ((rxByte & L_ACKN_MASK) == L_ACKN_IND)
                    {
                        // this can only happen in bus monitor mode
                        print("got L_ACKN_IND\r\n");
                    }
                    else if (rxByte == U_RESET_IND)
                    {
                        print("got U_RESET_IND\r\n");
                    }
                    else if ((rxByte & U_STATE_IND) == U_STATE_IND)
                    {
                        print("got U_STATE_IND:");
                        if (rxByte & 0x80)
                            print(" SC");
                        if (rxByte & 0x40)
                            print(" RE");
                        if (rxByte & 0x20)
                            print(" TE");
                        if (rxByte & 0x10)
                            print(" PE");
                        if (rxByte & 0x08)
                            print(" TW");
                        print("\r\n");
                    }
                    else if ((rxByte & U_FRAME_STATE_MASK) == U_FRAME_STATE_IND)
                    {
                        print("got U_FRAME_STATE_IND: 0x");
                        print(rxByte, HEX);
                        print("\r\n");
                    }
                    else if ((rxByte & U_CONFIGURE_MASK) == U_CONFIGURE_IND)
                    {
                        print("got U_CONFIGURE_IND: 0x");
                        print(rxByte, HEX);
                        print("\r\n");
                    }
                    else if (rxByte == U_FRAME_END_IND)
                    {
                        print("got U_FRAME_END_IND\r\n");
                    }
                    else if (rxByte == U_STOP_MODE_IND)
                    {
                        print("got U_STOP_MODE_IND\r\n");
                    }
                    else if (rxByte == U_SYSTEM_STAT_IND)
                    {
                        print("got U_SYSTEM_STAT_IND: 0x");
                        while (true)
                        {
                            int tmp = readUart();
                            if (tmp < 0)
                                continue;

                            print(tmp, HEX);
                            break;
                        }
                        print("\r\n");
                    }
                    else
                    {
                        print("got UNEXPECTED: 0x");
                        print(rxByte, HEX);
                        print("\r\n");
                    }
                }
                break;
            case RX_L_ADDR:
                if (HAL_GetTick() - _lastByteRxTime > EOPR_TIMEOUT)
                {
                    _rxState = RX_WAIT_START;
                    print("EOPR @ RX_L_ADDR\r\n");
                    break;
                }
                if (!uartAvailable())
                    break;
                _lastByteRxTime = HAL_GetTick();
                rxByte = readUart();
#ifdef DBG_TRACE
                print(rxByte, HEX);
#endif
                buffer[_RxByteCnt++] = rxByte;
                _xorSum ^= rxByte;

                if (_RxByteCnt == 7)
                {
                    // Destination Address + payload available
                    // check if echo; ignore repeat bit of control byte
                    _isEcho = (_sendBuffer != nullptr && (!((buffer[0] ^ _sendBuffer[0]) & ~0x20) && !memcmp(buffer + _convert + 1, _sendBuffer + 1, 5)));

                    // convert into Extended.ind
                    if (_convert)
                    {
                        buffer[1] = buffer[6] & 0xF0;
                        buffer[6] &= 0x0F;
                    }

                    if (!_isEcho)
                    {
                        uint8_t c = U_ACK_REQ;

                        // The bau knows everything and could either check the address table object (normal device)
                        // or any filter tables (coupler) to see if we are addressed.

                        // check if individual or group address
                        bool isGroupAddress = (buffer[1] & 0x80) != 0;
                        uint16_t addr = getWord(buffer + 4);

                        if (_cb.isAckRequired(addr, isGroupAddress))
                        {
                            c |= U_ACK_REQ_ADRESSED;
                        }

                        // Hint: We can send directly here, this doesn't disturb other transmissions
                        // We don't have to update _lastByteTxTime because after U_ACK_REQ the timing is not so tight
                        writeUart(c);
                    }
                    _rxState = RX_L_DATA;
                }
                break;
            case RX_L_DATA:
                if (!uartAvailable())
                    break;
                _lastByteRxTime = HAL_GetTick();
                rxByte = readUart();
#ifdef DBG_TRACE
                print(rxByte, HEX);
#endif
                if (_RxByteCnt == MAX_KNX_TELEGRAM_SIZE - 2)
                {
                    print("invalid telegram size\r\n");
                    enterRxWaitEOP();
                }
                else
                {
                    buffer[_RxByteCnt++] = rxByte;
                }

                if (_RxByteCnt == buffer[6] + 7 + 2)
                {
                    // complete Frame received, payloadLength+1 for TCPI +1 for CRC
                    // check if crc is correct
                    if (rxByte == (uint8_t)(~_xorSum))
                    {
                        if (!_isEcho)
                        {
                            _receiveBuffer[0] = 0x29;
                            _receiveBuffer[1] = 0;
#ifdef DBG_TRACE
                            unsigned long runTime = HAL_GetTick();
#endif
                            frameBytesReceived(_receiveBuffer, _RxByteCnt + 2);
#ifdef DBG_TRACE
                            runTime = HAL_GetTick() - runTime;
                            if (runTime > (OVERRUN_COUNT * 14) / 10)
                            {
                                // complain when the runtime was long than the OVERRUN_COUNT allows
                                print("processing received frame took: ");
                                print(runTime);
                                print(" ms\r\n");
                            }
#endif
                        }
                        _rxState = RX_WAIT_START;
#ifdef DBG_TRACE
                        print("RX_WAIT_START\r\n");
#endif
                    }
                    else
                    {
                        print("frame with invalid crc ignored\r\n");
                        enterRxWaitEOP();
                    }
                }
                else
                {
                    _xorSum ^= rxByte;
                }
                break;
            case RX_WAIT_EOP:
                if (HAL_GetTick() - _lastByteRxTime > EOP_TIMEOUT)
                {
                    // found a gap
                    _rxState = RX_WAIT_START;
#ifdef DBG_TRACE
                    print("RX_WAIT_START\r\n");
#endif
                    break;
                }
                if (uartAvailable())
                {
                    readUart();
                    _lastByteRxTime = HAL_GetTick();
                }
                break;
            default:
                print("invalid _rxState\r\n");
                enterRxWaitEOP();
                break;
            }
        } while (_rxState == RX_L_ADDR && (stayInRx || uartAvailable()));

        // Check for spurios DATA_CONN message
        if (dataConnMsg && _txState != TX_WAIT_CONN)
        {
            print("unexpected L_DATA_CON\r\n");
        }

        switch (_txState)
        {
        case TX_IDLE:
            if (!isTxQueueEmpty())
            {
                loadNextTxFrame();
                _txState = TX_FRAME;
#ifdef DBG_TRACE
                print("TX_FRAME\r\n");
#endif
            }
            break;
        case TX_FRAME:
            if (HAL_GetTick() - _lastByteTxTime > TX_TIMEPAUSE)
            {
                if (sendSingleFrameByte() == false)
                {
                    _waitConfirmStartTime = HAL_GetTick();
                    _txState = TX_WAIT_CONN;
#ifdef DBG_TRACE
                    print("TX_WAIT_CONN\r\n");
#endif
                }
                else
                {
                    _lastByteTxTime = HAL_GetTick();
                }
            }
            break;
        case TX_WAIT_CONN:
            if (dataConnMsg)
            {
                dataConBytesReceived(_receiveBuffer, _RxByteCnt + 2, (dataConnMsg & SUCCESS));
                delete[] _sendBuffer;
                _sendBuffer = 0;
                _sendBufferLength = 0;
                _txState = TX_IDLE;
            }
            else if (HAL_GetTick() - _waitConfirmStartTime > CONFIRM_TIMEOUT)
            {
                print("L_DATA_CON not received within expected time\r\n");
                uint8_t cemiBuffer[MAX_KNX_TELEGRAM_SIZE];
                cemiBuffer[0] = 0x29;
                cemiBuffer[1] = 0;
                memcpy((cemiBuffer + 2), _sendBuffer, _sendBufferLength);
                dataConBytesReceived(cemiBuffer, _sendBufferLength + 2, false);
                delete[] _sendBuffer;
                _sendBuffer = 0;
                _sendBufferLength = 0;
                _txState = TX_IDLE;
#ifdef DBG_TRACE
                print("TX_IDLE\r\n");
#endif
            }
            break;
        }
    } while (uartAvailable());
}

bool TpUartDataLinkLayer::sendFrame(CemiFrame &frame)
{
    if (!_enabled)
    {
        dataConReceived(frame, false);
        return false;
    }

    addFrameTxQueue(frame);
    return true;
}

bool TpUartDataLinkLayer::resetChip()
{
    if (_waitConfirmStartTime > 0)
        return false;
    uint8_t cmd = U_RESET_REQ;
    writeUart(cmd);

    int resp = readUart();
    if (resp == U_RESET_IND)
        return true;

    _waitConfirmStartTime = HAL_GetTick();
    return false;
}

bool TpUartDataLinkLayer::resetChipTick()
{
    int resp = readUart();
    if (resp == U_RESET_IND)
    {
        _waitConfirmStartTime = 0;
        return true;
    }
    else if (HAL_GetTick() - _waitConfirmStartTime > RESET_TIMEOUT)
        _waitConfirmStartTime = 0;

    return false;
}

void TpUartDataLinkLayer::stopChip()
{
#ifdef NCN5130
    uint8_t cmd = U_STOP_MODE_REQ;
    writeUart(cmd);
    while (true)
    {
        int resp = readUart();
        if (resp == U_STOP_MODE_IND)
            break;
    }
#endif
}

TpUartDataLinkLayer::TpUartDataLinkLayer(DeviceObject &devObj,
                                         NetworkLayerEntity &netLayerEntity,
                                         ITpUartCallBacks &cb)
    : _deviceObject(devObj), _networkLayerEntity(netLayerEntity), _cb(cb)
{
}

void TpUartDataLinkLayer::frameBytesReceived(uint8_t *buffer, uint16_t length)
{
    // printHex("=>", buffer, length);
    CemiFrame frame(buffer, length);

    frameReceived(frame);
}

void TpUartDataLinkLayer::dataConBytesReceived(uint8_t *buffer, uint16_t length, bool success)
{
    // printHex("=>", buffer, length);
    CemiFrame frame(buffer, length);
    dataConReceived(frame, success);
}

void TpUartDataLinkLayer::enabled(bool value)
{
    if (value && !_enabled)
    {
        setupUart();

        uint8_t cmd = U_RESET_REQ;
        writeUart(cmd);
        _waitConfirmStartTime = HAL_GetTick();
        bool flag = false;

        while (true)
        {
            int resp = readUart();
            if (resp == U_RESET_IND)
            {
                flag = true;
                break;
            }
            else if (HAL_GetTick() - _waitConfirmStartTime > RESET_TIMEOUT)
            {
                flag = false;
                break;
            }
        }

        if (flag)
        {
            _enabled = true;
            print("ownaddr ");
            print(_deviceObject.individualAddress(), HEX);
            print("\r\n");
        }
        else
        {
            _enabled = false;
            print("ERROR, TPUART not responding\r\n");
        }
        return;
    }
    if (!value && _enabled)
    {
        _enabled = false;
        stopChip();
        print("closeUart\r\n");
        closeUart();
        return;
    }
}

bool TpUartDataLinkLayer::enabled() const
{
    return _enabled;
}

DptMedium TpUartDataLinkLayer::mediumType() const
{
    return DptMedium::KNX_TP1;
}

bool TpUartDataLinkLayer::sendSingleFrameByte()
{
    uint8_t cmd[2];

    uint8_t idx = _TxByteCnt >> 6;

    if (_sendBuffer == NULL)
        return false;

    if (_TxByteCnt < _sendBufferLength)
    {
        if (idx != _oldIdx)
        {
            _oldIdx = idx;
            cmd[0] = U_L_DATA_OFFSET_REQ | idx;
            writeUart(cmd, 1);
        }

        if (_TxByteCnt != _sendBufferLength - 1)
            cmd[0] = U_L_DATA_START_CONT_REQ | (_TxByteCnt & 0x3F);
        else
            cmd[0] = U_L_DATA_END_REQ | (_TxByteCnt & 0x3F);

        cmd[1] = _sendBuffer[_TxByteCnt];
#ifdef DBG_TRACE
        print(cmd[1], HEX);
#endif

        writeUart(cmd, 2);
        _TxByteCnt++;
    }

    // Check for last byte send
    if (_TxByteCnt >= _sendBufferLength)
    {
        _TxByteCnt = 0;
        return false;
    }
    return true;
}

void TpUartDataLinkLayer::addFrameTxQueue(CemiFrame &frame)
{
    _tx_queue_frame_t *tx_frame = new _tx_queue_frame_t;
    tx_frame->length = frame.telegramLengthtTP();
    tx_frame->data = new uint8_t[tx_frame->length];
    tx_frame->next = NULL;
    frame.fillTelegramTP(tx_frame->data);

    if (_tx_queue.back == NULL)
    {
        _tx_queue.front = _tx_queue.back = tx_frame;
    }
    else
    {
        _tx_queue.back->next = tx_frame;
        _tx_queue.back = tx_frame;
    }
}

bool TpUartDataLinkLayer::isTxQueueEmpty()
{
    if (_tx_queue.front == NULL)
    {
        return true;
    }
    return false;
}

void TpUartDataLinkLayer::loadNextTxFrame()
{
    if (_tx_queue.front == NULL)
    {
        return;
    }
    _tx_queue_frame_t *tx_frame = _tx_queue.front;
    _sendBuffer = tx_frame->data;
    _sendBufferLength = tx_frame->length;
    _tx_queue.front = tx_frame->next;

    if (_tx_queue.front == NULL)
    {
        _tx_queue.back = NULL;
    }
    delete tx_frame;
}
void TpUartDataLinkLayer::dataRequest(AckType ack, AddressType addrType, uint16_t destinationAddr, uint16_t sourceAddr, FrameFormat format, Priority priority, NPDU &npdu)
{
    // Normal data requests and broadcasts will always be transmitted as (domain) broadcast with domain address for open media (e.g. RF medium)
    // The domain address "simulates" a closed medium (such as TP) on an open medium (such as RF or PL)
    // See 3.2.5 p.22
    sendTelegram(npdu, ack, destinationAddr, addrType, sourceAddr, format, priority, Broadcast);
}

void TpUartDataLinkLayer::systemBroadcastRequest(AckType ack, FrameFormat format, Priority priority, NPDU &npdu, uint16_t sourceAddr)
{
    // System Broadcast requests will always be transmitted as broadcast with KNX serial number for open media (e.g. RF medium)
    // See 3.2.5 p.22
    sendTelegram(npdu, ack, 0, GroupAddress, sourceAddr, format, priority, SysBroadcast);
}
bool TpUartDataLinkLayer::sendTelegram(NPDU &npdu, AckType ack, uint16_t destinationAddr, AddressType addrType, uint16_t sourceAddr, FrameFormat format, Priority priority, SystemBroadcast systemBroadcast)
{
    CemiFrame &frame = npdu.frame();
    frame.messageCode(L_data_ind);
    frame.destinationAddress(destinationAddr);
    frame.sourceAddress(sourceAddr);
    frame.addressType(addrType);
    frame.priority(priority);
    frame.repetition(RepetitionAllowed);
    frame.systemBroadcast(systemBroadcast);

    if (npdu.octetCount() <= 15)
        frame.frameType(StandardFrame);
    else
        frame.frameType(format);

    if (!frame.valid())
    {
        print("invalid frame\r\n");
        return false;
    }

    //    if (frame.npdu().octetCount() > 0)
    //    {
    //        _print("<- DLL ");
    //        frame.apdu().printPDU();
    //    }

    // The data link layer might be an open media link layer
    // and will setup rfSerialOrDoA, rfInfo and rfLfn that we also
    // have to send through the cEMI server tunnel
    // Thus, reuse the modified cEMI frame as "frame" is only passed by reference here!
    bool success = sendFrame(frame);

#ifdef USE_CEMI_SERVER
    CemiFrame tmpFrame(frame.data(), frame.totalLenght());
    // We can just copy the pointer for rfSerialOrDoA as sendFrame() sets
    // a pointer to const uint8_t data in either device object (serial) or
    // RF medium object (domain address)
    tmpFrame.rfSerialOrDoA(frame.rfSerialOrDoA());
    tmpFrame.rfInfo(frame.rfInfo());
    tmpFrame.rfLfn(frame.rfLfn());
    tmpFrame.confirm(ConfirmNoError);
    _cemiServer->dataIndicationToTunnel(tmpFrame);
#endif

    return success;
}

void TpUartDataLinkLayer::dataConReceived(CemiFrame &frame, bool success)
{
    MessageCode backupMsgCode = frame.messageCode();
    frame.messageCode(L_data_con);
    frame.confirm(success ? ConfirmNoError : ConfirmError);
    AckType ack = frame.ack();
    AddressType addrType = frame.addressType();
    uint16_t destination = frame.destinationAddress();
    uint16_t source = frame.sourceAddress();
    FrameFormat type = frame.frameType();
    Priority priority = frame.priority();
    NPDU &npdu = frame.npdu();
    SystemBroadcast systemBroadcast = frame.systemBroadcast();

#ifdef USE_CEMI_SERVER
    // if the confirmation was caused by a tunnel request then
    // do not send it to the local stack
    if (frame.sourceAddress() == _cemiServer->clientAddress())
    {
        // Stop processing here and do NOT send it the local network layer
        return;
    }
#endif

    if (addrType == GroupAddress && destination == 0)
        if (systemBroadcast == SysBroadcast)
            _networkLayerEntity.systemBroadcastConfirm(ack, type, priority, source, npdu, success);
        else
            _networkLayerEntity.broadcastConfirm(ack, type, priority, source, npdu, success);
    else
        _networkLayerEntity.dataConfirm(ack, addrType, destination, type, priority, source, npdu, success);

    frame.messageCode(backupMsgCode);
}
void TpUartDataLinkLayer::frameReceived(CemiFrame &frame)
{
    AckType ack = frame.ack();
    AddressType addrType = frame.addressType();
    uint16_t destination = frame.destinationAddress();
    uint16_t source = frame.sourceAddress();
    FrameFormat type = frame.frameType();
    Priority priority = frame.priority();
    NPDU &npdu = frame.npdu();
    uint16_t ownAddr = _deviceObject.individualAddress();
    SystemBroadcast systemBroadcast = frame.systemBroadcast();

#ifdef USE_CEMI_SERVER
    // Do not send our own message back to the tunnel
    if (frame.sourceAddress() != _cemiServer->clientAddress())
    {
        _cemiServer->dataIndicationToTunnel(frame);
    }
#endif

    if (source == ownAddr)
        _deviceObject.individualAddressDuplication(true);

    if (addrType == GroupAddress && destination == 0)
    {
        if (systemBroadcast == SysBroadcast)
            _networkLayerEntity.systemBroadcastIndication(ack, type, npdu, priority, source);
        else
            _networkLayerEntity.broadcastIndication(ack, type, npdu, priority, source);
    }
    else
    {
        _networkLayerEntity.dataIndication(ack, addrType, destination, type, npdu, priority, source);
    }
}
#endif
