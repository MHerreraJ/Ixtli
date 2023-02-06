#ifndef IXTLI_GRAPHICS_COLOR_DRAWABLE_H
#define IXTLI_GRAPHICS_COLOR_DRAWABLE_H

#include <Ixtli/Graphics/Color.h>
#include <Ixtli/Graphics/Canvas.h>
#include <Ixtli/Graphics/Drawable.h>

namespace Ixtli{

class ColorDrawable : public Drawable {
private:
    Paint paint;
public:
    ColorDrawable() : paint() {}
    ColorDrawable(const Color& c) : paint(Paint().setColor(c)) { }

    inline void setColor(Color c) 
        { paint.setColor(c); }

    void draw(Canvas& canvas) override
        {  canvas.drawRect(getBounds(), paint); }
    

};

}

#endif //IXTLI_GRAPHICS_COLOR_DRAWABLE_H