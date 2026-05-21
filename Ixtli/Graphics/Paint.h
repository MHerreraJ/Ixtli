#ifndef IXTLI_GRAPHICS_PAINT_H
#define IXTLI_GRAPHICS_PAINT_H

#include <iostream>
#include <memory>
#include <GL/freeglut.h>
#include <Ixtli/Graphics/Rectangle.h>
#include <Ixtli/Graphics/Color.h>
#include <Ixtli/Graphics/PathEffect.h>
#include <Ixtli/Graphics/Font.h>

namespace Ixtli{

class Paint{
public:
    enum class Style{
        FILL,
        STROKE,
        FILL_AND_STROKE
    };
private:
    Color realColor;
    Color color;
    std::shared_ptr<Font> font;
public:
    float strokeWidth;
    float textSize;
    float transparency;
    PathEffect pathEffect;
    Style style;

    Paint(): realColor(Color::TRANSPARENT), color(Color::TRANSPARENT),font(nullptr), strokeWidth(1), textSize(10), transparency(1.0), pathEffect(), style(Style::FILL_AND_STROKE) {}

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

    inline Paint& setStyle(Style s) { 
        style = s; 
        return *this; 
    }

    
    inline Paint& setStrokeWidth(float w)
        { strokeWidth = w; return *this; }
    
    inline Paint& setTextSize(float s)
        { textSize = s; return *this; }

    inline Paint& setPathEffect(const PathEffect& p)
        { pathEffect = p; return *this; }

    inline Paint& setFont(const std::shared_ptr<Font>& f)
        { font = f; return *this; }

    inline bool loadFont(const std::string& path){
        auto loadedFont = std::make_shared<Font>();
        if(!loadedFont->loadFromFile(path)){
            return false;
        }
        font = loadedFont;
        return true;
    }

    inline const std::shared_ptr<Font>& getFont() const
        { return font; }

    inline bool hasLoadedFont() const
        { return font != nullptr && font->isLoaded(); }
    
    inline const Color& getRealColor() const
        { return realColor; }
    
    inline const Color& getColor() const{
        return color;
    }

    inline Style getStyle() const
        { return style; }

    inline void* getTextFont() const{
        if(textSize <= 10){
            return GLUT_BITMAP_HELVETICA_10;
        }
        if(textSize <= 12){
            return GLUT_BITMAP_HELVETICA_12;
        }
        return GLUT_BITMAP_HELVETICA_18;
    }

    inline int getTextHeight() const{
        if(textSize <= 10){
            return 10;
        }
        if(textSize <= 12){
            return 12;
        }
        return 18;
    }

    inline void getTextBounds(const std::string& text, Rect& bounds) const{
        if(hasLoadedFont() && font->getTextBounds(text, max(1U, static_cast<unsigned int>(textSize)), bounds)){
            return;
        }

        bounds.x1 = 0;
        bounds.y1 = 0;
        bounds.x2 = glutBitmapLength(getTextFont(), reinterpret_cast<const unsigned char*>(text.c_str()));
        bounds.y2 = getTextHeight();
    }

    inline void getTextBounds(const std::string& text, int start, int end, Rect& bounds) const{
        if((end-start) < 0 || start < 0 || end < 0 || text.length() < static_cast<size_t>(end)) return;
        std::string clipped = text.substr(start, end-start);

        if(hasLoadedFont() && font->getTextBounds(clipped, max(1U, static_cast<unsigned int>(textSize)), bounds)){
            return;
        }

        bounds.x1 = 0;
        bounds.y1 = 0;
        bounds.x2 = glutBitmapLength(getTextFont(), reinterpret_cast<const unsigned char*>(clipped.c_str()));
        bounds.y2 = getTextHeight();
    }
    

};

inline std::ostream& operator << (std::ostream& os, const Paint& p){
    os << "(color: " << p.getRealColor() << ", strokeWidth: " << p.strokeWidth 
        << ", textSize: " << p.textSize << ", pathEffect: " << p.pathEffect << ")";
    return os;
}

}

#endif //IXTLI_GRAPHICS_PAINT_H