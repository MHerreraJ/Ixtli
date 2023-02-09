#ifndef IXTLI_EVENTS_VIEW_EVENT_H
#define IXTLI_EVENTS_VIEW_EVENT_H

#include <functional>

namespace Ixtli{

class View;

using IOnFocusChangeListener = std::function<void(View*, bool)>;

class OnFocusChangeListener{
public:
    virtual void onTextChanged(View* v, bool hasFocus) = 0;
};


class OnFocusChangeHandler{
public:
    enum Handler { NONE, FUNCTION_PTR, INTERFACE };
private:
    Handler handler;
    IOnFocusChangeListener fnPtr;
    OnFocusChangeListener* ifPtr;
public:
    OnFocusChangeHandler():handler(NONE), fnPtr(nullptr), ifPtr(nullptr) {}
    OnFocusChangeHandler(IOnFocusChangeListener fnPtr) : handler(fnPtr != nullptr ? FUNCTION_PTR : NONE), fnPtr(fnPtr), ifPtr(nullptr){}
    OnFocusChangeHandler(OnFocusChangeListener* interface) : handler(interface != nullptr ? INTERFACE : NONE), fnPtr(nullptr), ifPtr(interface){}

    inline explicit operator bool() const
        { return handler != NONE; }
    
    inline void operator()(View* v, bool hasFocus){ 
        switch (handler){
            case FUNCTION_PTR:{
                fnPtr(v, hasFocus); 
            }break;
            case INTERFACE:{
                ifPtr->onTextChanged(v, hasFocus); 
            }break;
            default: break;
        }
    }
};

};


#endif //IXTLI_EVENTS_VIEW_EVENT_H