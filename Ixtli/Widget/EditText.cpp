#include <thread>
#include <chrono>
#include <Ixtli/Graphics/Rectangle.h>
#include <Ixtli/Widget/EditText.h>

using namespace Ixtli;

EditText::EditText() : TextView(), cursor(0), x1TextViewport(0), x2TextViewport(0), scrollMutex(),
    lastMouseScroll(getTime()), lastCursorBlink(getTime()),
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

    float textOffsetX = 0.5f * bounds.height();
    float textOffsetY = 0.5f * (getHeight() - bounds.height());

    c.drawText(text, x1TextViewport, x2TextViewport, textOffsetX, textOffsetY, paint);
    if(visibleCursor && hasFocus()){
        Rect cursorBounds{};
        paint.getTextBounds(text, x1TextViewport, cursor, cursorBounds);

        float cursorX = textOffsetX + cursorBounds.x2 + 1.0f;
        float cursorTop = textOffsetY + 1.0f;
        float cursorBottom = getHeight() - textOffsetY/2;
        c.drawLine(cursorX, cursorTop, cursorX, cursorBottom, Paint().setColor(Color(Color::RED)).setStrokeWidth(1.5f));
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
            // if(x1TextViewport < text.length()){
            //     x1TextViewport ++;
            // }
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
    if(!hasFocus()){
        std::cout << "No tiene foco" << std::endl;
        return;
    }
    if(keyEventListener && keyEventListener(this, key, action)){
        return;
    }

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
    (void)y;
    if(btn == MouseButton::MIDDLE){
        // if(scrollMutex.try_lock()){
            time_s now = getTime();
            auto dt = timeDiff_ms<long long int>(lastMouseScroll, now);
            if(dt < 3){
                // scrollMutex.unlock();
                return;
            }
            lastMouseScroll = now;
            if(action == MouseAction::SCROLL_UP){
                onKeyEvent(static_cast<int>(Key::LEFT), KeyAction::KEY_DOWN);
            }else if(action == MouseAction::SCROLL_DOWN){
                onKeyEvent(static_cast<int>(Key::RIGHT), KeyAction::KEY_DOWN);
            }
            // scrollMutex.unlock();
        // }
        return;
    }
    if(btn != MouseButton::LEFT || action != MouseAction::PRESSED_DOWN) return;

    size_t pos = x1TextViewport;
    Rect bounds;
    paint.getTextBounds(text, x1TextViewport, x2TextViewport, bounds);

    float textOffsetX = 0.5f * bounds.height();
    float localX = x - textOffsetX;

    if(localX <= 0){
        pos = x1TextViewport;
    }else if(localX >= bounds.x2){
        pos = x2TextViewport;
    }else{
        paint.getTextBounds(text, x1TextViewport, pos, bounds);
        while(localX > bounds.x2){
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
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        if(!hasFocus()) continue;

        auto dt = timeDiff_ms<long long int>(lastCursorBlink, getTime());
        if((visibleCursor && dt < 600) || (!visibleCursor && dt < 400)){
            continue;
        }

        visibleCursor = !visibleCursor;
        invalidate();
        lastCursorBlink = getTime();
    }
    runningCursorThread = false;
}

void EditText::registerOnKeyEventListener(Ixtli::OnKeyEventHandler listener)
{
    keyEventListener = listener;
}

void EditText::setText(const char* txt){
    cursor = 0;
    x1TextViewport = 0;
    x2TextViewport = 0;
    TextView::setText(txt);
}
