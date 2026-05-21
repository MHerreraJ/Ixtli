#ifndef IXTLI_GRAPHICS_POINT_H
#define IXTLI_GRAPHICS_POINT_H

#include <iostream>

namespace Ixtli {

template <typename N>
struct Point2D{
    N x, y;

    Point2D() : x(0), y(0) {}
    Point2D(N x, N y) : x(x), y(y) {}

    inline N dot(const Point2D<N>& p) const
        { return x*p.x + y*p.y; }

    inline Point2D<N> normalize() const{
        N l = x*x + y*y;
        if(l == 0) return Point2D<N>(0, 0);
        l = 1.0f/sqrt(l);
        return Point2D<N>(x*l, y*l);
    }

    // Unary operator negate
    inline Point2D<N> operator-() const 
        { return Point2D<N>(-x, -y); }


    // Binary operators addition, substraction
    inline Point2D<N> operator+(const Point2D<N>& p) const 
        { return Point2D<N>(x + p.x, y + p.y); }

    inline Point2D<N> operator-(const Point2D<N>& p) const 
        { return Point2D<N>(x - p.x, y - p.y); }

    template<typename Number>
    inline Point2D<N> operator*(Number num) const 
        { return Point2D<N>(x*num, y*num); }

    
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
    
    double length() const
        { return sqrt(x*x + y*y); }
};

template <typename T, typename N>
inline Point2D<T> operator*(N num, const Point2D<T>& p){
    return p*num;
}



using Point = Point2D<int>;
using PointF = Point2D<float>;


template <typename N>
inline std::ostream& operator << (std::ostream& os, const Point2D<N>& p){
    os << "(x:" << p.x << ", y:" << p.y << ")";
    return os;
}

};

#endif //IXTLI_GRAPHICS_POINT_H