#pragma once

#include <stdint.h>
#include "knx_types.h"
#include "Print_Function.h"

struct CemiFrameC;

typedef struct
{
    uint8_t *_data;
    uint8_t length;
    CemiFrameC &_frame;
} APDUC;

APDUC APDU_create(uint8_t *data, CemiFrameC &frame);

void printPDU();

uint8_t length() const;

CemiFrameC &frame();

uint8_t *data();

void type(ApduType atype);

ApduType type();