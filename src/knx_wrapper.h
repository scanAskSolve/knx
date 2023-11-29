#ifndef KNX_WRAPPER_H
#define KNX_WRAPPER_H
#include <cstdint>
#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration of the opaque pointer to KnxFacade
typedef struct KnxFacadeTop KnxFacadeTop;

// Function declarations
void knx_create();
void knx_destroy();
void knx_getGroupObject(int goNr);
uint8_t knx_paramByte(uint32_t addr);
void knx_loop();
int knx_configured();
void knx_start();
void knx_readMemory();

#ifdef __cplusplus
}
#endif

#endif // KNX_WRAPPER_H