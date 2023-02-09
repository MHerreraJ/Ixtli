#include <iostream>
#include <GL/freeglut.h>

#include <Ixtli/Graphics/Rectangle.h>
#include <Ixtli/Graphics/Paint.h>
#include <Ixtli/Widget/TextView.h>


using namespace Ixtli;

void TextView::onParentSetTransparencyEventRequest(float transparency){
    getBackground()->setTransparency(transparency);
    paint.setTransparency(transparency);
}

void TextView::setText(const char* txt){
    std::string before = text;
    text = txt;
    onTextChanged(this, before, text);
    invalidate();
}

void TextView::setText(const std::string& txt){
    setText(txt.c_str());
}


void TextView::onDraw(Canvas& c){
    Rect bounds{};
    paint.getTextBounds(text, 0, text.length(), bounds);
    c.drawText(text, 0, 0.5f*(getHeight() - bounds.height()), paint);
}