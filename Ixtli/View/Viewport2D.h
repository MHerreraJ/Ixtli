#ifndef IXTLI_GRAPHICS_VIEWPORT_H
#define IXTLI_GRAPHICS_VIEWPORT_H

#include <iostream>
#include <Ixtli/Graphics/Point2D.h>

namespace Ixtli{

template <typename N>
struct Viewport2D{
    N left, bottom;
    N right, top;

    Viewport2D() : left(0), bottom(0), right(0), top(0) {}
    Viewport2D(N left, N bottom, N width, N height) : left(left), bottom(bottom), right(left + width), top(bottom + height) {}
    Viewport2D(const Point2D<N>& leftBottom, N width, N height) : left(leftBottom.x), bottom(leftBottom.y), right(leftBottom.x + width), top(leftBottom.y + height) {}

    inline N height() const
        { return  top - bottom; }
    
    inline N width() const
        { return right - left; }
    
    template <typename T = N>
    inline bool contains(const Point2D<T>& p)
        { return p.x >= left && p.x <= right && p.y >= bottom && p.y <= top;}
};

using Viewport = Viewport2D<int>;

template <typename N>
inline std::ostream& operator << (std::ostream& os, const Viewport2D<N>& v){
    std::cout << "(x0: " << v.left << ", y0: " << v.bottom << ", w: " << v.width() << ", h: " << v.height() << ")";
    return os;
}

};

#endif //IXTLI_GRAPHICS_VIEWPORT_H