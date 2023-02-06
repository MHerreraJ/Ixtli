#ifndef IXTLI_GRAPHICS_PATH_EFFECT_H
#define IXTLI_GRAPHICS_PATH_EFFECT_H

#include <iostream>

namespace Ixtli {

struct PathEffect{
    int factor;
    unsigned short pattern;
    PathEffect() : factor(3) , pattern(0xFFFF) {};

    inline PathEffect& setFactor(int f)
        { factor = f; return *this; }

    inline PathEffect& setPattern(unsigned short p)
        { pattern = p; return *this; }
    
};

inline std::ostream& operator << (std::ostream& os, const PathEffect& p){
    os << "(pattern: " << p.pattern << "factor: " << p.factor << ")";
    return os;
}

};
#endif  //IXTLI_GRAPHICS_PATH_EFFECT_H