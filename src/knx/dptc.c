#include "dptc.h"
Dptc Dptc_create(unsigned short mainGroup,
                 unsigned short subGroup,
                 unsigned short index)
{
    Dptc dptc;
    dptc.mainGroup = mainGroup;
    dptc.subGroup = subGroup;
    dptc.index = index;
    return dptc;
}
Dptc Dptc_default()
{
    Dptc dptc;
    dptc.mainGroup = 0;
    dptc.subGroup = 0;
    dptc.index = 0;
    return dptc;
}
Dptc Dptc_copy(Dptc dptc)
{
    Dptc result_dptc;
    result_dptc.mainGroup = dptc.mainGroup;
    result_dptc.subGroup = dptc.subGroup;
    result_dptc.index = dptc.index;
    return result_dptc;
}

int Dptc_equals(Dptc dptc1, Dptc dptc2)
{
    return (dptc1.index == dptc2.index) && (dptc1.subGroup == dptc2.subGroup) && (dptc1.mainGroup == dptc2.mainGroup);
}