#include "Animal.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void setName(Animal* a, const char* newName) {
    if (a->name) {
        free(a->name);
    }
    a->name = _strdup(newName);
}

const char* getName(const Animal* a) {
    return a->name;
}