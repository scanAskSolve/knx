#include "arduino_platform.h"
#include "Print_Function.h"
#include "dpt.h"

//#include "bits.h"

Dpt::Dpt()
{}

Dpt::Dpt(short _mainGroup, short _subGroup, short _index /* = 0 */)
    //: mainGroup(_mainGroup), subGroup(_subGroup), index(_index)
{

    mainGroup = _mainGroup;
    subGroup = _subGroup;
    index = _index;

    if (subGroup == 0)
        print("WARNING: You used and invalid Dpt *.0\r\n");
}

bool Dpt::operator==(const Dpt& other) const
{
    return other.mainGroup == mainGroup && other.subGroup == subGroup && other.index == index;
}

bool Dpt::operator!=(const Dpt& other) const
{
    return !(other == *this);
}
