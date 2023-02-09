#ifndef IXTLI_WIDGET_EDIT_TEXT_H
#define IXTLI_WIDGET_EDIT_TEXT_H

#include <iostream>
#include <Ixtli/Core/Types.h>
#include <Ixtli/Events/KeyboardEvent.h>
#include <Ixtli/Graphics/Paint.h>
#include <Ixtli/Graphics/Canvas.h>
#include <Ixtli/Widget/TextView.h>

namespace Ixtli{

class EditText : public TextView {
    private:
        size_t cursor;
        size_t x1TextViewport;
        size_t x2TextViewport;
        Mutex scrollMutex;

        bool visibleCursor;
        bool runThreads;
        bool runningCursorThread;

        int screenChars;
        int screenOffset;

        void cursorThread();
        void updateTextViewport();
    
    protected:
        void onAttachedToWindow() override;

    public:
        EditText();
        ~EditText();

        inline bool acceptsKeyboardInput() const override
            { return true; }
    
        void onDraw(Canvas& c) override;

        void onKeyEvent(int key, KeyAction action) override;

        void onMouseEvent(MouseButton btn, MouseAction action, int x, int y) override;

        void onSizeChanged(int w, int h, int oldw, int oldh) override;

        void onFocusChanged(bool gainFocus) override;

};

};


#endif //IXTLI_WIDGET_EDIT_TEXT_H