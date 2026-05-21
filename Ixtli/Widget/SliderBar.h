#ifndef IXTLI_WIDGET_SLIDER_BAR_H
#define IXTLI_WIDGET_SLIDER_BAR_H

#include <vector>
#include <Ixtli/View/View.h>

namespace Ixtli{

class OnSlideBarEventListener{
    public:
        virtual ~OnSlideBarEventListener() {}
        virtual void onSlideBarValueChanged(View* v, int oldValue, int newValue) = 0;
};

class SliderBar : public View {
    protected:
        Paint emptyPaint;
        Paint filledPaint;
        Paint sliderPaint;
        int minValue;
        int maxValue;
        int value;
        bool isPressed;
        std::vector<OnSlideBarEventListener*> listeners;
        void notifyListeners(int oldValue, int newValue);
    
    public:
        SliderBar() : View(), emptyPaint(), filledPaint(), sliderPaint(), minValue(0), maxValue(100), value(0), isPressed(false), listeners() {}
        virtual ~SliderBar() {}

        void setMinValue(int min);
        void setMaxValue(int max);
        void setValue(int val);

        inline int getMinValue() const
            { return minValue; }

        inline int getMaxValue() const
            { return maxValue; }
        
        inline int getValue() const
            { return value; }

        void onDraw(Canvas& c) override;

        void onMouseEvent(MouseButton btn, MouseAction action, int x, int y) override;

        void setFilledColor(const Color& c)
            { filledPaint.setColor(c); invalidate(); }
        
        void setEmptyColor(const Color& c)
            { emptyPaint.setColor(c); invalidate(); }
        
        void setSliderColor(const Color& c)
            { sliderPaint.setColor(c); invalidate(); }

        // void onSizeChanged(int w, int h, int oldw, int oldh) override;


        void addOnSlideBarEventListener(OnSlideBarEventListener* listener);

        void removeOnSlideBarEventListener(OnSlideBarEventListener* listener);

    

};

}

#endif //IXTLI_WIDGET_SLIDER_BAR_H