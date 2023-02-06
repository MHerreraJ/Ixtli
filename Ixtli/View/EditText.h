#ifndef IXTLI_VIEW_EDIT_TEXT_H
#define IXTLI_VIEW_EDIT_TEXT_H

#include <iostream>
#include <Ixtli/Events/KeyboardEvent.h>
#include <Ixtli/Graphics/Paint.h>
#include <Ixtli/Graphics/Canvas.h>
#include <Ixtli/View/View.h>

namespace Ixtli{

class EditText : public View {
    private:
        std::string text;
        Paint paint;

        OnTextChangedHandler textChanged;
    
    protected:
        void onViewCreated(bool& textFocusable) override;

    public:
        EditText() : View(), text(), paint(), textChanged() {}
        virtual ~EditText() {}

    
        void setText(const char* txt);
        void setText(const std::string& txt);

        inline const std::string& getText() const
            { return text; }
        
        void onDraw(Canvas& c) override;

        inline void setTextColor(const Color& c)
            { paint.setColor(c); invalidate(); }

        inline void setTextSize(float size)
            { paint.setTextSize(size);  invalidate(); }
        
        inline void setTextThickness(float stroke)
            { paint.setStrokeWidth(stroke); invalidate(); }
        
        void onKeyEvent(int key, KeyAction action) override;
};

};


#endif //IXTLI_VIEW_EDIT_TEXT_H