#ifndef IXTLI_VIEW_TEXT_VIEW_H
#define IXTLI_VIEW_TEXT_VIEW_H

#include <iostream>
#include <Ixtli/Graphics/Color.h>
#include <Ixtli/Graphics/Paint.h>
#include <Ixtli/Graphics/Canvas.h>
#include <Ixtli/View/View.h>

namespace Ixtli{

class TextView : public View {
    private:
        std::string text;
        Paint paint;
    public:
        TextView() : View(), text(), paint() {}
        virtual ~TextView() {}

        void setText(const char* txt);
        void setText(const std::string& txt);

        inline const std::string& getText() const 
            { return text; }

        void onDraw(Canvas& c) override;

        inline void setTextColor(const Color& c)
            { paint.setColor(c); invalidate(); }
        
        inline void setTextSize(float size)
            { paint.setTextSize(size); invalidate(); }
        
        inline void setTextThickness(float stroke)
            { paint.setStrokeWidth(stroke); invalidate(); }
        


};


};


#endif //IXTLI_VIEW_TEXT_VIEW_H