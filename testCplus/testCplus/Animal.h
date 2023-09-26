#pragma once
#include <stdio.h>

typedef struct Animal {
    
} Animal;

// Function prototypes
//void getName(Animal* a);
//void setName(Animal* a, const char* newName);
#ifdef __cplusplus
extern "C" {
#endif

    void setName(Animal* a, const char* newName);
    const char* getName(const Animal* a);

#ifdef __cplusplus
}
#endif