#ifndef IXTLI_WIDGET_TEXT_VIEW_H
#define IXTLI_WIDGET_TEXT_VIEW_H

#include <iostream>
#include <Ixtli/Graphics/Color.h>
#include <Ixtli/Graphics/Paint.h>
#include <Ixtli/Graphics/Canvas.h>
#include <Ixtli/View/View.h>

namespace Ixtli{

class TextView : public View {
    protected:
        std::string text;
        Paint paint;

        OnTextChangedHandler onTextChanged;
        void onParentSetTransparencyEventRequest(float transparency) override;

    public:
        TextView() : View(), text(), paint(), onTextChanged() {}
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
        
        inline void setOnTextChangedListener(IOnTextChanged listener)
            { onTextChanged = listener; }
        
        inline void setOnTextChangedListener(OnTextChangedListener* listener)
            { onTextChanged = listener; }
        
};


};


#endif //IXTLI_WIDGET_TEXT_VIEW_H