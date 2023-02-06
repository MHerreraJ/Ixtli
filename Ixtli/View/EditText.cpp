#include <Ixtli/Graphics/Rectangle.h>
#include <Ixtli/View/EditText.h>

using namespace Ixtli;


void EditText::onViewCreated(bool& textFocusable){
    textFocusable = true;
}

void EditText::setText(const char* txt){
    text = txt;
    invalidate();
}

void EditText::setText(const std::string& txt){
    setText(txt.c_str());
}


void EditText::onDraw(Canvas& c){
    Rect bounds{};
    paint.getTextBounds(text, 0, text.length(), bounds);
    c.drawText(text, 0.5*bounds.height(), 0.5f*(getHeight() - bounds.height()), paint);
}

void EditText::onKeyEvent(int key, KeyAction action){
    if(key == Key::BACKSPACE){
        if(text.length() == 0) return;
        text = text.substr(0, text.length() - 1);
    }else{
        text += (char)key;
    }
    invalidate();
}