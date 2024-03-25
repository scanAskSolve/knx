#pragma once

#define paramDelay(time) (uint32_t)( \
            (time & 0xC000) == 0xC000 ? (time & 0x3FFF) * 100 : \
            (time & 0xC000) == 0x0000 ? (time & 0x3FFF) * 1000 : \
            (time & 0xC000) == 0x4000 ? (time & 0x3FFF) * 60000 : \
            (time & 0xC000) == 0x8000 ? ((time & 0x3FFF) > 1000 ? 3600000 : \
                                            (time & 0x3FFF) * 3600000 ) : 0 )
#define PT_CH1_Enable_bit_Disable 0
#define PT_CH1_Enable_bit_Enable 1
#define PT_CH2_Enable_bit_Disable 0
#define PT_CH2_Enable_bit_Enable 1
#define PT_CH3_Enable_bit_Disable 0
#define PT_CH3_Enable_bit_Enable 1
#define PT_CH4_Enable_bit_Disable 0
#define PT_CH4_Enable_bit_Enable 1
#define PT_Manual_Auto_bit_Disable Auto send 0
#define PT_Manual_Auto_bit_Enable Auto send 1
#define PT_Auto_Mode_delay_1s 0
#define PT_Auto_Mode_delay_3s 1
#define PT_Auto_Mode_delay_5s 2
#define PT_Auto_Mode_delay_10s 3
//--------------------Allgemein---------------------------
#define MAIN_ApplicationNumber 0x00FA
#define MAIN_ApplicationVersion 0x01
#define MAIN_OrderNumber "ADC Sensor"
#define MAIN_ParameterSize 17
#define MAIN_MaxKoNumber 20


#define APP_CH1_Enable		0x0000
// Offset: 0, Size: 1 Bit, Text: CH1 Enable Select
#define ParamAPP_CH1_Enable knx.paramBit(0, 0)
#define APP_CH2_Enable		0x0000
// Offset: 0, BitOffset: 1, Size: 1 Bit, Text: CH2 Enable Select
#define ParamAPP_CH2_Enable knx.paramBit(0, 1)
#define APP_CH3_Enable		0x0000
// Offset: 0, BitOffset: 2, Size: 1 Bit, Text: CH3 Enable Select
#define ParamAPP_CH3_Enable knx.paramBit(0, 2)
#define APP_CH4_Enable		0x0000
// Offset: 0, BitOffset: 3, Size: 1 Bit, Text: CH4 Enable Select
#define ParamAPP_CH4_Enable knx.paramBit(0, 3)
#define APP_Transmit_Mode		0x0000
// Offset: 0, BitOffset: 4, Size: 1 Bit, Text: Transmit Mode
#define ParamAPP_Transmit_Mode knx.paramBit(0, 4)
#define APP_Auto_Mode_delay		0x0000
#define APP_Auto_Mode_delay_Shift	1
#define APP_Auto_Mode_delay_Mask	0x0003
// Offset: 0, BitOffset: 5, Size: 2 Bit, Text: Set Auto Mode Delay Time
#define ParamAPP_Auto_Mode_delay ((uint)((knx.paramByte(0) >> APP_Auto_Mode_delay_Shift) & APP_Auto_Mode_delay_Mask))
#define APP_CH1_Upper_Limit		0x0001
// Offset: 1, Size: 8 Bit (1 Byte), Text: Set CH1 Upper Limit(%)
#define ParamAPP_CH1_Upper_Limit ((uint)((knx.paramByte(1))))
#define APP_CH1_Upper_Warning_Limit		0x0002
// Offset: 2, Size: 8 Bit (1 Byte), Text: Set CH1 Upper Warning Limit(%)
#define ParamAPP_CH1_Upper_Warning_Limit ((uint)((knx.paramByte(2))))
#define APP_CH1_Lower_Warning_Limit		0x0003
// Offset: 3, Size: 8 Bit (1 Byte), Text: Set CH1 Lower Warning Limit(%)
#define ParamAPP_CH1_Lower_Warning_Limit ((uint)((knx.paramByte(3))))
#define APP_CH1_Lower_Limit		0x0004
// Offset: 4, Size: 8 Bit (1 Byte), Text: Set CH1 Lower Limit(%)
#define ParamAPP_CH1_Lower_Limit ((uint)((knx.paramByte(4))))
#define APP_CH2_Upper_Limit		0x0005
// Offset: 5, Size: 8 Bit (1 Byte), Text: Set CH2 Upper Limit(%)
#define ParamAPP_CH2_Upper_Limit ((uint)((knx.paramByte(5))))
#define APP_CH2_Upper_Warning_Limit		0x0006
// Offset: 6, Size: 8 Bit (1 Byte), Text: Set CH2 Upper Warning Limit(%)
#define ParamAPP_CH2_Upper_Warning_Limit ((uint)((knx.paramByte(6))))
#define APP_CH2_Lower_Warning_Limit		0x0007
// Offset: 7, Size: 8 Bit (1 Byte), Text: Set CH2 Lower Warning Limit(%)
#define ParamAPP_CH2_Lower_Warning_Limit ((uint)((knx.paramByte(7))))
#define APP_CH2_Lower_Limit		0x0008
// Offset: 8, Size: 8 Bit (1 Byte), Text: Set CH2 Lower Limit(%)
#define ParamAPP_CH2_Lower_Limit ((uint)((knx.paramByte(8))))
#define APP_CH3_Upper_Limit		0x0009
// Offset: 9, Size: 8 Bit (1 Byte), Text: Set CH3 Upper Limit(%)
#define ParamAPP_CH3_Upper_Limit ((uint)((knx.paramByte(9))))
#define APP_CH3_Upper_Warning_Limit		0x000A
// Offset: 10, Size: 8 Bit (1 Byte), Text: Set CH3 Upper Warning Limit(%)
#define ParamAPP_CH3_Upper_Warning_Limit ((uint)((knx.paramByte(10))))
#define APP_CH3_Lower_Warning_Limit		0x000B
// Offset: 11, Size: 8 Bit (1 Byte), Text: Set CH3 Lower Warning Limit(%)
#define ParamAPP_CH3_Lower_Warning_Limit ((uint)((knx.paramByte(11))))
#define APP_CH3_Lower_Limit		0x000C
// Offset: 12, Size: 8 Bit (1 Byte), Text: Set CH3 Lower Limit(%)
#define ParamAPP_CH3_Lower_Limit ((uint)((knx.paramByte(12))))
#define APP_CH4_Upper_Limit		0x000D
// Offset: 13, Size: 8 Bit (1 Byte), Text: Set CH4 Upper Limit(%)
#define ParamAPP_CH4_Upper_Limit ((uint)((knx.paramByte(13))))
#define APP_CH4_Upper_Warning_Limit		0x000E
// Offset: 14, Size: 8 Bit (1 Byte), Text: Set CH4 Upper Warning Limit(%)
#define ParamAPP_CH4_Upper_Warning_Limit ((uint)((knx.paramByte(14))))
#define APP_CH4_Lower_Warning_Limit		0x000F
// Offset: 15, Size: 8 Bit (1 Byte), Text: Set CH4 Lower Warning Limit(%)
#define ParamAPP_CH4_Lower_Warning_Limit ((uint)((knx.paramByte(15))))
#define APP_CH4_Lower_Limit		0x0010
// Offset: 16, Size: 8 Bit (1 Byte), Text: Set CH4 Lower Limit(%)
#define ParamAPP_CH4_Lower_Limit ((uint)((knx.paramByte(16))))
//!< Number: 1, Text: Ch1_value, Function: Get CH1 Value
#define APP_KoCh1_value 1
#define KoAPP_Ch1_value knx.getGroupObject(1)
//!< Number: 2, Text: Ch2_value, Function: Get CH2 Value
#define APP_KoCh2_value 2
#define KoAPP_Ch2_value knx.getGroupObject(2)
//!< Number: 3, Text: Ch3_value, Function: Get CH3 Value
#define APP_KoCh3_value 3
#define KoAPP_Ch3_value knx.getGroupObject(3)
//!< Number: 4, Text: Ch4_value, Function: Get CH4 Value
#define APP_KoCh4_value 4
#define KoAPP_Ch4_value knx.getGroupObject(4)
//!< Number: 5, Text: CH1 Upper Limit Flag, Function: CH1 Upper Limit Flag
#define APP_KoCH1_Upper_Limit_Flag 5
#define KoAPP_CH1_Upper_Limit_Flag knx.getGroupObject(5)
//!< Number: 6, Text: CH1 Upper Warning Limit Flag, Function: CH1 Upper Warning Limit Flag
#define APP_KoCH1_Upper_Warning_Limit_Flag 6
#define KoAPP_CH1_Upper_Warning_Limit_Flag knx.getGroupObject(6)
//!< Number: 7, Text: CH1 Lower Warning Limit Flag, Function: CH1 Lower Warning Limit Flag
#define APP_KoCH1_Lower_Warning_Limit_Flag 7
#define KoAPP_CH1_Lower_Warning_Limit_Flag knx.getGroupObject(7)
//!< Number: 8, Text: CH1 Lower Limit Flag, Function: CH1 Lower Limit Flag
#define APP_KoCH1_Lower_Limit_Flag 8
#define KoAPP_CH1_Lower_Limit_Flag knx.getGroupObject(8)
//!< Number: 9, Text: CH2 Upper Limit Flag, Function: CH2 Upper Limit Flag
#define APP_KoCH2_Upper_Limit_Flag 9
#define KoAPP_CH2_Upper_Limit_Flag knx.getGroupObject(9)
//!< Number: 10, Text: CH2 Upper Warning Limit Flag, Function: CH2 Upper Warning Limit Flag
#define APP_KoCH2_Upper_Warning_Limit_Flag 10
#define KoAPP_CH2_Upper_Warning_Limit_Flag knx.getGroupObject(10)
//!< Number: 11, Text: CH2 Lower Warning Limit Flag, Function: CH2 Lower Warning Limit Flag
#define APP_KoCH2_Lower_Warning_Limit_Flag 11
#define KoAPP_CH2_Lower_Warning_Limit_Flag knx.getGroupObject(11)
//!< Number: 12, Text: CH2 Lower Limit Flag, Function: CH2 Lower Limit Flag
#define APP_KoCH2_Lower_Limit_Flag 12
#define KoAPP_CH2_Lower_Limit_Flag knx.getGroupObject(12)
//!< Number: 13, Text: CH3 Upper Limit Flag, Function: CH3 Upper Limit Flag
#define APP_KoCH3_Upper_Limit_Flag 13
#define KoAPP_CH3_Upper_Limit_Flag knx.getGroupObject(13)
//!< Number: 14, Text: CH3 Upper Warning Limit Flag, Function: CH3 Upper Warning Limit Flag
#define APP_KoCH3_Upper_Warning_Limit_Flag 14
#define KoAPP_CH3_Upper_Warning_Limit_Flag knx.getGroupObject(14)
//!< Number: 15, Text: CH3 Lower Warning Limit Flag, Function: CH3 Lower Warning Limit Flag
#define APP_KoCH3_Lower_Warning_Limit_Flag 15
#define KoAPP_CH3_Lower_Warning_Limit_Flag knx.getGroupObject(15)
//!< Number: 16, Text: CH3 Lower Limit Flag, Function: CH3 Lower Limit Flag
#define APP_KoCH3_Lower_Limit_Flag 16
#define KoAPP_CH3_Lower_Limit_Flag knx.getGroupObject(16)
//!< Number: 17, Text: CH4 Upper Limit Flag, Function: CH4 Upper Limit Flag
#define APP_KoCH4_Upper_Limit_Flag 17
#define KoAPP_CH4_Upper_Limit_Flag knx.getGroupObject(17)
//!< Number: 18, Text: CH4 Upper Warning Limit Flag, Function: CH4 Upper Warning Limit Flag
#define APP_KoCH4_Upper_Warning_Limit_Flag 18
#define KoAPP_CH4_Upper_Warning_Limit_Flag knx.getGroupObject(18)
//!< Number: 19, Text: CH4 Lower Warning Limit Flag, Function: CH4 Lower Warning Limit Flag
#define APP_KoCH4_Lower_Warning_Limit_Flag 19
#define KoAPP_CH4_Lower_Warning_Limit_Flag knx.getGroupObject(19)
//!< Number: 20, Text: CH4 Lower Limit Flag, Function: CH4 Lower Limit Flag
#define APP_KoCH4_Lower_Limit_Flag 20
#define KoAPP_CH4_Lower_Limit_Flag knx.getGroupObject(20)

