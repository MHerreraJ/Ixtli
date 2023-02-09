#include <thread>
#include <chrono>
#include <Ixtli/Graphics/Rectangle.h>
#include <Ixtli/Widget/EditText.h>

using namespace Ixtli;

EditText::EditText() : TextView(), cursor(0), x1TextViewport(0), x2TextViewport(0), scrollMutex(),
    visibleCursor(false), runThreads(true), runningCursorThread(false),
    screenChars(0), screenOffset(0){
    
}

EditText::~EditText(){
    runThreads = false;
    while(runningCursorThread);
}

void EditText::onAttachedToWindow(){
    if(!runningCursorThread){
        auto t = std::thread(&EditText::cursorThread, this);
        t.detach();
    }
}

void EditText::onDraw(Canvas& c){
    Rect bounds{};
    paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);
    //c.drawText(text, 0.5*bounds.height(), 0.5f*(getHeight() - bounds.height()), paint);
    c.drawText(text, x1TextViewport, x2TextViewport, 0.5*bounds.height(), 0.5f*(getHeight() - bounds.height()), paint);
    if(visibleCursor && hasFocus()){
        Rect cursorBounds{};
        paint.getTextBounds(text, x1TextViewport, cursor, cursorBounds);
        c.drawLine(cursorBounds.x2 + 10, 4, cursorBounds.x2 + 10, getHeight()-4, Paint().setColor(Color(Color::RED).setAlpha(170)).setStrokeWidth(3));
    }
}

void EditText::updateTextViewport(){
    Rect bounds{};
    if(x2TextViewport > text.length()){
        x2TextViewport = text.length();
    }

    if(cursor + 1 > x2TextViewport){
        x2TextViewport = cursor;
        if(x2TextViewport < text.length()){
            x2TextViewport++;
            if(x1TextViewport < text.length()){
                x1TextViewport ++;
            }
        }
        paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);
        while(bounds.x2 + 15 > getWidth()){
            x1TextViewport++;
            paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);
        }
    }else if(cursor < x1TextViewport + 1){
        x1TextViewport = cursor;
        if(x1TextViewport > 0 ){
            x1TextViewport --;
            if(x2TextViewport > 0){
                x2TextViewport --;
            }
        }
        paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);
        while(bounds.x2 + 15 > getWidth()){
            x2TextViewport--;
            paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);
        }
    }

    if(x1TextViewport == x2TextViewport){
        if(x1TextViewport > 0){
            x1TextViewport --;
        }
    }

    paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);
    while (bounds.x2 + 30 <= getWidth() && x2TextViewport < text.length()){
        x2TextViewport++;
        paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);
    }
}


void EditText::onKeyEvent(int key, KeyAction action){
    if(action != KeyAction::KEY_DOWN) return;

    bool triggerEvent = false;
    std::string before = text;
    switch(static_cast<Key>(key)){
        case Key::LEFT:{
            if(cursor > 0){
                cursor --;
            }
        }break;
        case Key::RIGHT:{
            if(cursor < text.length()){
                cursor ++;                
            }
        }break;
        case Key::HOME:{
            cursor = 0;
        }break;
        case Key::END:{
            cursor = text.length();
        }break;
        case Key::BACKSPACE:{
            if(cursor > 0){
                text = text.replace(cursor-1, 1, "");                
                cursor--;
                triggerEvent = true;
                
                if(x1TextViewport < cursor + 1 && x1TextViewport > 0){
                    x1TextViewport--;
                }
            }
        }break;
        case Key::DEL:{
            if(cursor < text.length()){
                text = text.replace(cursor, 1, "");
                triggerEvent = true;
            }
        }break;
        default:{
            if(isSpecialKey(key) || key == Key::NUL || key == Key::ENTER || key == Key::CARRY_RETURN){
                return;
            }
            if(text.length() == cursor){
                text += key;
                cursor ++;
            }else{
                text = text.insert(cursor, 1, key);
                cursor ++;
            }
            triggerEvent = true;
        }break;
    }
    if(triggerEvent){
        onTextChanged(this, before, text);
    }
    updateTextViewport();
    visibleCursor = true;
    invalidate();
}

void EditText::onMouseEvent(MouseButton btn, MouseAction action, int x, int y){
    if(btn == MouseButton::MIDDLE){
        if(scrollMutex.try_lock()){
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
            if(action == MouseAction::SCROLL_UP){
                onKeyEvent(static_cast<int>(Key::LEFT), KeyAction::KEY_DOWN);
            }else if(action == MouseAction::SCROLL_DOWN){
                onKeyEvent(static_cast<int>(Key::RIGHT), KeyAction::KEY_DOWN);
            }
            scrollMutex.unlock();
        }
        return;
    }
    if(btn != MouseButton::LEFT || action != MouseAction::PRESSED_DOWN) return;

    size_t pos = x1TextViewport;
    Rect bounds;
    paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);

    if(x >= bounds.x2){
        //pos = text.length();
        pos = x2TextViewport;
    }else{
        paint.getTextBounds(text, x1TextViewport, pos, bounds);
        while(x > bounds.x2){
            pos ++;
            paint.getTextBounds(text, x1TextViewport, pos, bounds);   

            if(pos >= text.length()) break;
        }
        if(pos > x1TextViewport){
            pos--;
        }
    }
    if(pos != cursor){
        cursor  = pos;
        visibleCursor = true;
        invalidate();
    }
}

void EditText::onSizeChanged(int w, int h, int oldw, int oldh){
    Rect bounds{};

    if(w > oldw){
        paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);
        while(bounds.x2 + 15 < getWidth()){
            if(x1TextViewport == 0) break;
            x1TextViewport--;
            paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);
        }
        while(bounds.x2 + 15 < getWidth()){
            if(x2TextViewport == text.length()) break;
            x2TextViewport++;
            paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);
        }
    }else {
        paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);
        while(bounds.x2 + 15 > getWidth()){
            if(x2TextViewport == cursor) break;
            x2TextViewport--;
            paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);
        }

        while(bounds.x2 + 15 > getWidth()){
            if(x1TextViewport >= cursor) break;
            x1TextViewport++;
            paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);
        }
    }
}

void EditText::onFocusChanged(bool gainFocus){
    visibleCursor = gainFocus;
    invalidate();
}


void EditText::cursorThread(){
    runningCursorThread = true;
    while(runThreads){
        if(visibleCursor){
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
            visibleCursor = false;
        }else{
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
            visibleCursor = true;
        }
        if(hasFocus())
            invalidate();
    }
    runningCursorThread = false;
}