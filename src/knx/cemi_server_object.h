#pragma once

#include "config.h"
#ifdef USE_CEMI_SERVER

#include "knx_types.h"

// #include "interface_object.h"
// class CemiServerObject: public InterfaceObject
class CemiServerObject
{
public:
  CemiServerObject();

  void setMediumTypeAsSupported(DptMedium dptMedium);
  void clearSupportedMediaTypes();
};

#endif
