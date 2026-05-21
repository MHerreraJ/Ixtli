#include <iostream>
#include <GL/freeglut.h>
#include <random>
#include <thread>
#include <chrono>
#include <sstream>
#include <sys/time.h>
#include <Ixtli/Events/MouseEvent.h>
#include <Ixtli/Widget/TextView.h>
#include <Ixtli/Widget/EditText.h>
#include <Ixtli/View/View.h>
#include <Ixtli/View/Window.h>
#include <Ixtli/View/ContextProvider.h>
#include <Ixtli/Widget/SliderBar.h>
#include <Ixtli/Widget/Toast.h>


using namespace Ixtli;

static float timediff_ms(struct timeval start, struct timeval end){
    return (end.tv_sec - start.tv_sec)*1000.0f + (end.tv_usec - start.tv_usec)/1000.0f;
}

class GraphCalculator : public Window, OnClickListener, OnSlideBarEventListener {
private:
    Color currentColor = Color::RED;
    Color currentColor2 = Color::GREEN;
    Color currentColor3 = Color::BLUE;

    UUID child1ID;
    UUID child2ID;
    UUID child3ID;
    UUID rootID;

    UUID textView;
    int sleepTimeMs=60000;
    

public:
    GraphCalculator() : Window(){}

     void onSlideBarValueChanged(View* v, int oldValue, int newValue) override{
        SliderBar* slider = dynamic_cast<SliderBar*>(v);
        int inverse = slider->getMaxValue() - newValue;
        float percent = (float)inverse/slider->getMaxValue();
        percent = percent*percent*percent;
        sleepTimeMs = 10 + 60000*percent;
        if(this->getToastCount() < 2){
            Toast::makeText(this, "New sleep time: " + std::to_string(sleepTimeMs), Toast::LENGTH_SHORT).show();
        }
    }

    void changeColorThread(){
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<unsigned int> dist(0, 0x00FFFFFF);
        
        struct timeval start{}, end{};
        for(;;){
            gettimeofday(&start, nullptr);
            gettimeofday(&end, nullptr);
            while(timediff_ms(start, end) < sleepTimeMs){
                gettimeofday(&end, nullptr);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            unsigned int r = 0xFF000000 | dist(mt);
            Color newColor(r);

            findViewByID(rootID)->setBackgroundColor(newColor);
            findViewByID(child1ID)->setBackgroundColor(newColor.inverse());
            //findViewByID(child2ID)->setBackgroundColor(0.5*newColor - 0.5*newColor.inverse());
            findViewByID(child2ID)->setBackgroundColor(Color(Color::WHITE).setAlpha(50));
            findViewByID(child3ID)->setBackgroundColor(Color(Color::RED).setAlpha(25));


            findViewByID<TextView>(textView)->setText(newColor.toString());
            findViewByID<TextView>(textView)->setTextColor(newColor.inverse());

        }
    }

    void onCreate() override{
        auto root = setRootView();
        root->setBackgroundColor(currentColor);
        rootID = root->getID();

        root->setOnClickListener([&](View* v, MouseButton btn, int x, int y){
            currentColor = currentColor.inverse();
            v->setBackgroundColor(currentColor);
            return true;
        });

        std::function<bool(View*, MouseButton, int, int)> listener 
            = std::bind(&GraphCalculator::clickListener, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        //root->setOnClickListener(listener);
        root->setOnClickListener(this);
        root->setLayoutParams(MarginLayoutParams(LayoutParams::MATCH_PARENT, 50, 50, 50, 50));

        std::shared_ptr<View> child = std::make_shared<View>();
        child->setBackgroundColor(currentColor2);
        child->setLayoutParams(MarginLayoutParams(LayoutParams::WRAP_CONTENT, 0, 0, 0, 0));
        child->setWidth(100);
        child->setHeight(100);
        child1ID = child->getID();
        //child->setOnClickListener(listener);
        child->setOnClickListener(this);

        root->addView(child);

        child = std::make_shared<View>();
        child->setBackgroundColor(currentColor3);
        child->setLayoutParams(MarginLayoutParams(LayoutParams::WRAP_CONTENT, 100, 0, 0, 0));
        child->setWidth(100);
        child->setHeight(100);
        child2ID = child->getID();
        //child->setOnClickListener(listener);
        child->setOnClickListener(this);
        root->addView(child);

        child = std::make_shared<View>();
        child->setBackgroundColor(Color::WHITE);
        child->setLayoutParams(MarginLayoutParams(LayoutParams::MATCH_PARENT, 20, 20, 20, 20));
        //child->setOnClickListener(listener);
        child->setOnClickListener(this);
        child3ID = child->getID();
        root->findViewByID(child2ID)->addView(child);

        
        auto tv = std::make_shared<TextView>();
        tv->setBackgroundColor(Color::TRANSPARENT);
        tv->setTextColor(Color::BLACK);
        tv->setTextSize(25.0f);
        tv->setText("Hola mundo");
        tv->setTextThickness(4.0f);
        tv->setLayoutParams(MarginLayoutParams(LayoutParams::MATCH_PARENT, 200, 0, 0, 0));
        //child->setWidth(300);
        tv->setHeight(50);
        tv->setOnClickListener(this);
        textView = tv->getID();
        root->addView(tv);

        auto editText = std::make_shared<EditText>();
        editText->setBackgroundColor(Color::fromARGB(155, 0, 0, 0));
        editText->setTextColor(Color::WHITE);
        editText->setTextSize(25.0f);
        editText->setTextThickness(2.0f);
        editText->setLayoutParams(MarginLayoutParams(LayoutParams::MATCH_PARENT, 200, 0, 50, 0));
        editText->setHeight(50);
        root->addView(editText);


        editText = std::make_shared<EditText>();
        std::cout << "EDIT TEXT 2" << editText->getID() << std::endl;
        editText->setBackgroundColor(Color::fromARGB(155, 255, 255, 255));
        editText->setTextColor(Color::BLACK);
        editText->setTextSize(25.0f);
        editText->setTextThickness(2.0f);
        editText->setLayoutParams(MarginLayoutParams(LayoutParams::MATCH_PARENT, 0, 0, 100, 0));
        editText->setHeight(50);
        root->addView(editText);


        auto slider = std::make_shared<SliderBar>();
        slider->addOnSlideBarEventListener(this);
        slider->setEmptyColor(Color::fromRGB(0,0,0));
        slider->setFilledColor(Color::fromRGB(255,255,255));
        slider->setSliderColor(Color::fromRGB(50,50,250));
        slider->setMaxValue(1000);
        slider->setLayoutParams(MarginLayoutParams(LayoutParams::MATCH_PARENT, 20, 20, 200, 0));
        slider->setHeight(20);

        root->addView(slider);


        std::thread thread(&GraphCalculator::changeColorThread, this);
        thread.detach();

        //root->setWidth()
    }

    bool clickListener(View* v, MouseButton btn, int x, int y){
        if(btn != MouseButton::LEFT) return false;
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<unsigned int> dist(0, 0x00FFFFFF);
        unsigned int r = 0xFF000000 | dist(mt);
        Color newColor(r);

        auto id = v->getID();
        if(id == rootID){
            currentColor = currentColor + newColor;
            v->setBackgroundColor(currentColor);
        }else if(id == child1ID){
            currentColor2 = 0.5*currentColor2+ newColor;
            v->setBackgroundColor(currentColor2);
        }else if(id == child2ID){
            currentColor3 = 0.5*currentColor3+ newColor;
            v->setBackgroundColor(currentColor3);
        }else if(id == child3ID){
            
            v->setBackgroundColor(r);
            //v->setBackgroundColor(currentColor.inverse() + (0.5*currentColor2 - 0.25*currentColor3.inverse()));
        }else if(id == textView){
            
            //std::stringstream strstream;
            //strstream << "Color: " << newColor;
            dynamic_cast<TextView*>(v)->setTextColor(newColor);
        }
        return true;
    }

    void onClick(View* v, MouseButton button, int x, int y){
        clickListener(v, button, x, y);
    }

   
};

int main(int argc, char** argv){
    auto provider = ContextProvider::getProvider();
    auto window = provider->createWindow<GraphCalculator>();


    provider->transferMainControl();
    return 0;
}