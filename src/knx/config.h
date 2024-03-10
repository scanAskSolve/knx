#pragma once

#ifndef NO_KNX_CONFIG

// Normal devices
// TP1: 0x07B0
// RF: 0x27B0
// IP: 0x57B0
#define MASK_VERSION 0x07B0
// #define MASK_VERSION 0x27B0
// #define MASK_VERSION 0x57B0

// Couplers
// IP/TP1: 0x091A
// TP1/RF: 0x2920
// #define MASK_VERSION 0x091A
// #define MASK_VERSION 0x2920

// Data Linklayer Driver Options
#if MASK_VERSION == 0x07B0
#define USE_TP
#endif

#if MASK_VERSION == 0x27B0
#define USE_RF
#endif

#if MASK_VERSION == 0x57B0
#define USE_IP
#endif

#if MASK_VERSION == 0x091A
#define USE_TP
#define USE_IP
#endif

#if MASK_VERSION == 0x2920
#define USE_TP
#define USE_RF
#endif

// option to have GroupObjects (KO in German) use 8 bytes mangement information RAM instead of 19 bytes
// see knx-demo-small-go for example
// this option might be also set via compiler flag -DSMALL_GROUPOBJECT if required
// #define SMALL_GROUPOBJECT

// Some defines to reduce footprint
// Do not perform conversion from KNXValue(const char*) to other types, it mainly avoids the expensive strtod
// #define KNX_NO_STRTOx_CONVERSION
// Do not print messages
// #define KNX_NO_PRINT


#endif

#if !defined(MASK_VERSION)
#error MASK_VERSION must be defined! See config.h for possible values!
#endif
