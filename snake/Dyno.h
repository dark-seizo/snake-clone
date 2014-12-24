
#ifndef _DYNO_H_
#define _DYNO_H_

#include <vector>
#include <string>
#include <math.h>

namespace dyno
{
    ///returns decimal precision, floats and doubles
    ///decimals not working atm
    template<typename T>
    T decimalPrecision(unsigned int decimals, T &x, bool roundedDown = false)
    {
        T val = floorf(x * 100 + (roundedDown == true ? 0.0 : 0.5))/ 100;
        return val;
    }

    ///splits a string by spaces (delimiter)
    std::vector<std::string> splitString(std::string &str);
}

#endif // _DYNO_H_
