#pragma once
#include <iostream>
#include"Animal.h"
class Dog : public Animal{

public:
	Dog(const char* name);
	void bark();

};
