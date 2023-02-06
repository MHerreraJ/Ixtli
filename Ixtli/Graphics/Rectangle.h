#ifndef IXTLI_GRAPHICS_RECTANGLE_H
#define IXTLI_GRAPHICS_RECTANGLE_H

#include <iostream>
#include <Ixtli/Math.h>
#include <Ixtli/Graphics/Point2D.h>

namespace Ixtli{

template <typename N>
struct Rectangle{
    N x1, y1;
    N x2, y2;

    Rectangle() : x1(0), y1(0), x2(0), y2(0) {}
    Rectangle(N x1, N y1, N x2, N y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}

    Rectangle(const Point2D<N>& p1, N width, N height) : x1(p1.x), y1(p1.y), x2(p1.x + width), y2(p1.y + height){}
    Rectangle(const Point2D<N>& p1, const Point2D<N>& p2) : x1(p1.x), y1(p1.y), x2(p2.x), y2(p2.y) {}

    inline N width() const
        { return abs(x2 - x1); }
    
    inline N height() const
        { return abs(y2 - y1); }
    
    template <typename T>
    inline operator Rectangle<T>() const 
        { return Rectangle<T>(static_cast<T>(x1), static_cast<T>(y1), static_cast<T>(x2), static_cast<T>(y2)); }
};

using Rect = Rectangle<int>;
using RectF = Rectangle<float>;

template <typename N>
inline std::ostream& operator << (std::ostream& os, const Rectangle<N>& r){
    os << "(x1: " << r.x1 << ", y1: " << r.y1 << ", x2: " << r.x2 << ", y2: " << r.y2 << ")" ;
    return os;
}

};


#endif //IXTLI_GRAPHICS_RECTANGLE_H