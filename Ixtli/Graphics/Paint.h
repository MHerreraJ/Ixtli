#ifndef IXTLI_GRAPHICS_PAINT_H
#define IXTLI_GRAPHICS_PAINT_H

#include <iostream>
#include <Ixtli/Graphics/Rectangle.h>
#include <Ixtli/Graphics/Color.h>
#include <Ixtli/Graphics/PathEffect.h>

namespace Ixtli{

class Paint{
private:
    Color realColor;
    Color color;
public:
    float strokeWidth;
    float textSize;
    float transparency;
    PathEffect pathEffect;

    Paint(): realColor(Color::TRANSPARENT), color(Color::TRANSPARENT), strokeWidth(1), textSize(10), transparency(1.0), pathEffect() {}

    inline Paint& setColor(const Color& c) { 
        realColor = c; 
        color = c; 
        auto alpha = min(255U, (unsigned int)(255.0f*(min(1.0f, max(0.0f, c.a_f() * transparency)))));
        color.setAlpha(alpha); 
        return *this; 
    }
    
    inline Paint& setTransparency(float t) { 
        transparency = max(0.0f, min(t, 1.0f)); 
        auto alpha = min(255U, (unsigned int)(255.0f*(min(1.0f, max(0.0f, realColor.a_f() * transparency)))));
        color.setAlpha(alpha); 
        return *this; 
    }
    
    inline Paint& setStrokeWidth(float w)
        { strokeWidth = w; return *this; }
    
    inline Paint& setTextSize(float s)
        { textSize = s; return *this; }

    inline Paint& setPathEffect(const PathEffect& p)
        { pathEffect = p; return *this; }
    
    inline const Color& getRealColor() const
        { return realColor; }
    
    inline const Color& getColor() const{
        return color;
    }

    
    inline void getTextBounds(const std::string& text, int start, int end, Rect& bounds) const{
        if((end-start) < 0 || start < 0 || end < 0 || text.length() < static_cast<size_t>(end)) return;
        bounds.x1 = 0;
        bounds.y1 = 0;
        bounds.x2 = (int)(textSize*(end - start));
        bounds.y2 = (int)(textSize * 119.05f / 104.76f);
    }
    

};

inline std::ostream& operator << (std::ostream& os, const Paint& p){
    os << "(color: " << p.getRealColor() << ", strokeWidth: " << p.strokeWidth 
        << ", textSize: " << p.textSize << ", pathEffect: " << p.pathEffect << ")";
    return os;
}

}

#endif //IXTLI_GRAPHICS_PAINT_H