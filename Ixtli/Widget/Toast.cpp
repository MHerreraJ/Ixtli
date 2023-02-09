#include <iostream>
#include <memory>

#include <Ixtli/View/Context.h>
#include <Ixtli/Widget/TextView.h>
#include <Ixtli/Widget/Toast.h>


using namespace Ixtli;


Toast::Toast(Context* context): duration(LENGTH_SHORT), text(), context(context), view(nullptr){
}

void Toast::show(){
    if(duration == LENGTH_LONG || duration == LENGTH_SHORT){
        if(context){
            context->notify(this);
        }
    }
}

std::shared_ptr<View> Toast::getView(){
    if(!view){
        view = std::make_shared<View>();
        view->setBackgroundColor(Color::DARK_GRAY);

        auto textView = std::make_shared<TextView>();
        textView->setText(text);
        textView->setLayoutParams(MarginLayoutParams(LayoutParams::MATCH_PARENT));
        textView->setBackgroundColor(Color::TRANSPARENT);
        textView->setTextColor(Color::WHITE);
        textView->setTextSize(20);
        view->addView(textView);
    }
    return view;
}

Toast Toast::makeText(Context* context, const std::string& text, int duration){
    Toast retVal(context);
    retVal.setText(text);
    retVal.setDuration(duration);
    return retVal;
}
