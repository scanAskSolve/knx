#include "Dog.h"

using namespace std;
Dog::Dog(const char* name) {
    this->name = NULL;  // Ensure name pointer is initialized to NULL
    setName(this, name);
}

void Dog::bark() {
    std::cout << getName(this) << " says: Woof!" << std::endl;
}