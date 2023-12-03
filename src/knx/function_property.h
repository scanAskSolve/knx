#pragma once

#include "property.h"

class InterfaceObject;

enum CallBack
{
    PID_ROUTETABLE = 0,
    PID_RF_ENABLE = 1,
    PID_SECURITY = 2
};

class FunctionProperty : public Property
{

public:
    FunctionProperty(RouterObject *io, PropertyID id,
                     CallBack callBack)
        //  void (*commandCallback)(T*, uint8_t*, uint8_t, uint8_t*, uint8_t&),
        //  void (*stateCallback)(T*, uint8_t*, uint8_t, uint8_t*, uint8_t&))
        : Property(id, false, PDT_FUNCTION, 1, ReadLv0 | WriteLv0), _interfaceObjectRouter(io) //, _commandCallback(commandCallback), _stateCallback(stateCallback)
    /* max_elements is set to 1, read and write level any value so we use Lv0, see 3.3.7 Application Layer p.68 */
    {
        _callback = callBack;
        // if(_callback == CallBack::PID_ROUTETABLE){
        //     io->functionRouteTableControl(false, data, length, resultData, resultLength);
        // }else if(_callback == CallBack::PID_ROUTETABLE){
        //     io->functionRfEnableSbc(true, data, length, resultData, resultLength);
        // }

        if (_callback == CallBack::PID_ROUTETABLE)
        {
            _commandCallback = [](RouterObject *obj, uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength)
            {
                obj->functionRouteTableControl(false, data, length, resultData, resultLength);
            };
            _stateCallback = [](RouterObject *obj, uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength)
            {
                obj->functionRouteTableControl(false, data, length, resultData, resultLength);
            };
        }
        else if (_callback == CallBack::PID_RF_ENABLE)
        {
            _commandCallback = [](RouterObject *obj, uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength)
            {
                obj->functionRfEnableSbc(true, data, length, resultData, resultLength);
            };
            _stateCallback = [](RouterObject *obj, uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength)
            {
                obj->functionRfEnableSbc(true, data, length, resultData, resultLength);
            };
        }
    }
    // FunctionProperty(SecurityInterfaceObject* io, PropertyID id,
    //                  CallBack callBack)
    //                 //  void (*commandCallback)(T*, uint8_t*, uint8_t, uint8_t*, uint8_t&),
    //                 //  void (*stateCallback)(T*, uint8_t*, uint8_t, uint8_t*, uint8_t&))
    //     : Property(id, false, PDT_FUNCTION, 1, ReadLv0|WriteLv0), _interfaceObjectSecurity(io), _commandCallback(commandCallback), _stateCallback(stateCallback)
    //     /* max_elements is set to 1, read and write level any value so we use Lv0, see 3.3.7 Application Layer p.68 */
    // {
    //     _callback = callBack;
    //     callBackPIDSecurityFailuresLog(obj, data, length, resultData, resultLength)
    // }

    // void callBackPIDSecurityFailuresLog(SecurityInterfaceObject* obj, uint8_t* data, uint8_t length, uint8_t* resultData, uint8_t& resultLength) {
    //             if (length != 3)
    //             {
    //                 resultData[0] = ReturnCodes::DataVoid;
    //                 resultLength = 1;
    //                 return;
    //             }
    //             uint8_t id = data[1];
    //             uint8_t info = data[2];
    //             if (id == 0 && info == 0)
    //             {
    //                 obj->clearFailureLog();
    //                 resultData[0] = ReturnCodes::Success;
    //                 resultData[1] = id;
    //                 resultLength = 2;
    //                 return;
    //             }
    //             resultData[0] = ReturnCodes::GenericError;
    //             resultLength = 1;
    //         }

    uint8_t read(uint16_t start, uint8_t count, uint8_t *data)
    {
        return 0;
    }

    uint8_t write(uint16_t start, uint8_t count, const uint8_t *data)
    {
        return 0;
    }

    void command(uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength) override
    {
        if (length == 0 || _commandCallback == nullptr)
        {
            resultLength = 0;
            return;
        }
        // if(_callback==CallBack::PID_SECURITY){
        //     _commandCallback(_interfaceObjectSecurity, data, length, resultData, resultLength);
        // }
        // else{
        _commandCallback(_interfaceObjectRouter, data, length, resultData, resultLength);
        // }
    }

    void state(uint8_t *data, uint8_t length, uint8_t *resultData, uint8_t &resultLength) override
    {
        if (length == 0 || _stateCallback == nullptr)
        {
            resultLength = 0;
            return;
        }
        // if(_callback==CallBack::PID_SECURITY){
        //     _commandCallback(_interfaceObjectSecurity, data, length, resultData, resultLength);
        // }
        // else{
        _commandCallback(_interfaceObjectRouter, data, length, resultData, resultLength);
        // }
    }

private:
    CallBack _callback;
    RouterObject *_interfaceObjectRouter = nullptr;
    void (*_commandCallback)(RouterObject *, uint8_t *, uint8_t, uint8_t *, uint8_t &) = nullptr;
    void (*_stateCallback)(RouterObject *, uint8_t *, uint8_t, uint8_t *, uint8_t &) = nullptr;

    // SecurityInterfaceObject* _interfaceObjectSecurity = nullptr;
    // void (*_commandCallback)(SecurityInterfaceObject*, uint8_t*, uint8_t, uint8_t*, uint8_t&) = nullptr;
    // void (*_stateCallback)(SecurityInterfaceObject*, uint8_t*, uint8_t, uint8_t*, uint8_t&) = nullptr;
};
