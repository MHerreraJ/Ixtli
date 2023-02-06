#ifndef IXTLI_MATH_H
#define IXTLI_MATH_H

namespace Ixtli{
    inline unsigned int isqrt(unsigned int x){
        if(x <= 1) return x;
        unsigned int x0 = x >> 1;
        unsigned int x1 = (x0 + x/x0) >> 1;
        while(x1 < x0){
            x0 = x1;
            x1 = (x0 + x/x0) >> 1;
        }
        return x0;
    }
    
    template<typename Number>
    inline Number abs(const Number& x){
        return x < 0 ? -x : x;
    }

    template<typename Number>
    inline Number sign(const Number& x){
        if(x == 0) return 0;
        return x < 0 ? -1 : 1;
    }

    template<typename Number>
    inline Number min(const Number& x, const Number& y){
        return x < y ? x : y;
    }

    template<typename Number>
    inline Number max(const Number& x, const Number& y){
        return x > y ? x : y;
    }
}

#endif //IXTLI_MATH_H