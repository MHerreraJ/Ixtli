#ifndef IXTLI_GRAPHICS_DRAWABLE_H
#define IXTLI_GRAPHICS_DRAWABLE_H

#include <Ixtli/Graphics/Rectangle.h>
#include <Ixtli/Graphics/Canvas.h>

namespace Ixtli{

class Drawable{
    private:
        float transparency;
        Rect bounds;
    public:
        Drawable() : transparency(1), bounds() {}
        virtual ~Drawable() {};

        inline void setBounds(const Rect& b)
            { bounds = b; }
        
        virtual inline void setTransparency(float t)
            { transparency = t; }
        
        inline int getTransparency() const
            { return transparency; }
        
        inline const Rect& getBounds() const
            { return bounds; }

        virtual void draw(Canvas& canvas) {};
};

};

#endif