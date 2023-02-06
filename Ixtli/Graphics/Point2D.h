#ifndef IXTLI_GRAPHICS_POINT_H
#define IXTLI_GRAPHICS_POINT_H

#include <iostream>

namespace Ixtli {

template <typename N>
struct Point2D{
    N x, y;

    Point2D() : x(0), y(0) {}
    Point2D(N x, N y) : x(x), y(y) {}

    // Unary operator negate
    inline Point2D<N> operator-() const 
        { return Point2D<N>(-x, -y); }


    // Binary operators addition, substraction
    inline Point2D<N> operator+(const Point2D<N>& p) const 
        { return Point2D<N>(x + p.x, y + p.y); }

    inline Point2D<N> operator-(const Point2D<N>& p) const 
        { return Point2D<N>(x - p.x, y - p.y); }

    
    // Binary operators self additon, self substaction
    inline Point2D<N>& operator+=(const Point2D<N>& p)
        { x += p.x; y += p.y; return *this; }

    inline Point2D<N>& operator-=(const Point2D<N>& p)
        { x -= p.x; y -= p.y; return *this; }


    // Compare operators
    inline bool operator == (const Point2D<N> & p) const
        { return (x == p.x) && (y == p.y); }
    
    inline bool operator != (const Point2D<N> & p) const
        { return (x != p.x) || (y != p.y); }
    

    inline N distanceSquaredTo(const Point2D<N>& p) const
        { return (p.x - x)*(p.x - x) + (p.y - y)*(p.y - y); }

    
    template <typename T> 
    inline operator Point2D<T>() const
        { return Point2D<T>(static_cast<T>(x), static_cast<T>(y)); }
};

using Point = Point2D<int>;
using PointF = Point2D<float>;


template <typename N>
inline std::ostream& operator << (std::ostream& os, const Point2D<N>& p){
    os << "(x:" << p.x << ", y:" << p.y << ")";
    return os;
}

};

#endif //IXTLI_GRAPHICS_POINT_H