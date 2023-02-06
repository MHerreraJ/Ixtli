#ifndef IXTLI_EVENTS_MOUSE_EVENT_H
#define IXTLI_EVENTS_MOUSE_EVENT_H

#include <functional>

namespace Ixtli{

class View;

enum class MouseButton{
    LEFT, 
    RIGHT, 
    MIDDLE, 
    NONE
};

enum class MouseAction{
    NONE,
    PRESSED_DOWN,
    PRESSED_UP, 
    MOVE,
    SCROLL_UP, 
    SCROLL_DOWN,
};

class MouseEvent{
    public:
        virtual void onMouseEvent(MouseButton button, MouseAction action, int x, int y) = 0;
};

using IOnClickListener = std::function<void(View*, MouseButton, int , int)>;

class OnClickListener{
public:
    virtual void onClick(View* v, MouseButton, int, int) = 0;
};


class OnClickListenerHandler{
public:
    enum Handler { NONE, FUNCTION_PTR, INTERFACE };
private:
    Handler handler;
    IOnClickListener fnPtr;
    OnClickListener* ifPtr;
public:
    OnClickListenerHandler():handler(NONE), fnPtr(nullptr), ifPtr(nullptr) {}
    OnClickListenerHandler(IOnClickListener fnPtr) : handler(fnPtr != nullptr ? FUNCTION_PTR : NONE), fnPtr(fnPtr), ifPtr(nullptr){}
    OnClickListenerHandler(OnClickListener* interface) : handler(interface != nullptr ? INTERFACE : NONE), fnPtr(nullptr), ifPtr(interface){}

    inline explicit operator bool() const
        { return handler != NONE; }
    
    inline void operator()(View* v, MouseButton button, int x, int y){ 
        switch (handler){
            case FUNCTION_PTR:{
                fnPtr(v, button, x, y); 
            }break;
            case INTERFACE:{
                ifPtr->onClick(v, button, x, y); 
            }break;
            default: break;
        }
    }
};

};

#endif //IXTLI_EVENTS_MOUSE_EVENT_H