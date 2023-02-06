#ifndef IXTLI_GRAPHICS_PAINT_H
#define IXTLI_GRAPHICS_PAINT_H

#include <iostream>
#include <Ixtli/Graphics/Rectangle.h>
#include <Ixtli/Graphics/Color.h>
#include <Ixtli/Graphics/PathEffect.h>

namespace Ixtli{

struct Paint{
    Color color;
    float strokeWidth;
    float textSize;
    PathEffect pathEffect;

    Paint():color(Color::BLACK), strokeWidth(1), textSize(10), pathEffect() {}

    inline Paint& setColor(const Color& c)
        { color = c; return *this; }
    
    inline Paint& setStrokeWidth(float w)
        { strokeWidth = w; return *this; }
    
    inline Paint& setTextSize(float s)
        { textSize = s; return *this; }

    inline Paint& setPathEffect(const PathEffect& p)
        { pathEffect = p; return *this; }

    
    inline void getTextBounds(const std::string& text, int start, int end, Rect& bounds) const{
        if((end-start) < 0 || start < 0 || end < 0 || text.length() < static_cast<size_t>(end)) return;
        bounds.x1 = 0;
        bounds.y1 = 0;
        bounds.x2 = (int)(textSize*(end - start));
        bounds.y2 = (int)(textSize * 119.05f / 104.76f);
    }
    

};

inline std::ostream& operator << (std::ostream& os, const Paint& p){
    os << "(color: " << p.color << ", strokeWidth: " << p.strokeWidth 
        << ", textSize: " << p.textSize << ", pathEffect: " << p.pathEffect << ")";
    return os;
}

}

#endif //IXTLI_GRAPHICS_PAINT_H