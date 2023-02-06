#ifndef IXTLI_GRAPHICS_DRAWABLE_H
#define IXTLI_GRAPHICS_DRAWABLE_H

#include <Ixtli/Graphics/Rectangle.h>
#include <Ixtli/Graphics/Canvas.h>

namespace Ixtli{

class Drawable{
    private:
        Rect bounds;
    public:
        Drawable() : bounds() {}
        virtual ~Drawable() {};

        inline void setBounds(const Rect& b)
            { bounds = b; }
        
        inline const Rect& getBounds() const
            { return bounds; }

        virtual void draw(Canvas& canvas) {};


};

};

#endif