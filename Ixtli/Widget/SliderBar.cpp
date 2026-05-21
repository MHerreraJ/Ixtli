#include <Ixtli/Widget/SliderBar.h>

using namespace Ixtli;

void SliderBar::setMinValue(int min){
    minValue = min;
    invalidate();
}

void SliderBar::setMaxValue(int max){
    maxValue = max;
    invalidate();
}

void SliderBar::setValue(int val){
    value = val;
    invalidate();
}


void SliderBar::notifyListeners(int oldValue, int newValue){
    for(auto& l : listeners){
        l->onSlideBarValueChanged(this, oldValue, newValue);
    }
}

void SliderBar::addOnSlideBarEventListener(OnSlideBarEventListener* listener){
    if(!listener) return;
    for(auto& l : listeners){
        if(l == listener) return;
    }
    listeners.push_back(listener);
}

void SliderBar::removeOnSlideBarEventListener(OnSlideBarEventListener* listener){
    if(!listener) return;
    for(auto it = listeners.begin(); it != listeners.end(); it++){
        if(*it == listener){
            listeners.erase(it);
            return;
        }
    }
}

void SliderBar::onDraw(Canvas& c){
    Rect bounds{};
    float percent = ((float)(value - minValue))/((float)(maxValue - minValue));
    float barHeight = getHeight()*0.9f;

    c.drawRect(0, getHeight() - barHeight, getWidth(), barHeight, emptyPaint);
    c.drawRect(0, getHeight() - barHeight, percent*getWidth(), barHeight, filledPaint);
    c.drawCircle(percent*getWidth(), getHeight()/2, getHeight()/2, sliderPaint);
}

void SliderBar::onMouseEvent(MouseButton btn, MouseAction action, int x, int y){
    if(action == MouseAction::PRESSED_DOWN || (action == MouseAction::MOVE && isPressed)){
        int oldValue = value;
        float percent = (float)x/getWidth();
        int newValue = minValue + (maxValue - minValue)*percent;
        if(newValue < minValue) newValue = minValue;
        if(newValue > maxValue) newValue = maxValue;
        value = newValue;
        isPressed = true;
        if(oldValue == value) return;
        notifyListeners(oldValue, value);
        invalidate();
    }else if(action == MouseAction::PRESSED_UP){
        isPressed = false;
        unfocus();
    }
}
