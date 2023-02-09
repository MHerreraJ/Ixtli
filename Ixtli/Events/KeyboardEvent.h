#ifndef IXTLI_EVENTS_KEYBOARD_EVENT_H
#define IXTLI_EVENTS_KEYBOARD_EVENT_H

#include <iostream>
#include <functional>

namespace Ixtli{

class View;

enum class Key : int {
    NUL = 0,
    BACKSPACE = 8,
    TAB = 9,
    ENTER = 10,
    CARRY_RETURN = 13,
    ESCAPE= 27,
    SPACE = 32,
    DEL = 127,
    LEFT = -0x64,
    UP = -0x65,
    RIGHT = -0x66,
    DOWN = -0x67,
    HOME = -0x6A,
    END = -0x6B
};

inline bool isSpecialKey(int k){
    return k < 0;
}

enum class KeyAction{
    KEY_DOWN,
    KEY_UP,
};

class KeyboardEvent{
    public:
        virtual void onKeyEvent(int key, KeyAction action) = 0;
};

inline bool operator == (Key eKey, int key)
    { return static_cast<int>(eKey) == key; }

inline bool operator == (int key, Key eKey)
    { return static_cast<int>(eKey) == key; }

inline bool operator != (Key eKey, int key)
    { return static_cast<int>(eKey) != key; }

inline bool operator != (int key, Key eKey)
    { return static_cast<int>(eKey) != key; }


using IOnTextChanged = std::function<void(View*, const std::string&, const std::string&)>;

class OnTextChangedListener{
public:
    virtual void onTextChanged(View* v, const std::string& before, const std::string& after) = 0;
};


class OnTextChangedHandler{
public:
    enum Handler { NONE, FUNCTION_PTR, INTERFACE };
private:
    Handler handler;
    IOnTextChanged fnPtr;
    OnTextChangedListener* ifPtr;
public:
    OnTextChangedHandler():handler(NONE), fnPtr(nullptr), ifPtr(nullptr) {}
    OnTextChangedHandler(IOnTextChanged fnPtr) : handler(fnPtr != nullptr ? FUNCTION_PTR : NONE), fnPtr(fnPtr), ifPtr(nullptr){}
    OnTextChangedHandler(OnTextChangedListener* interface) : handler(interface != nullptr ? INTERFACE : NONE), fnPtr(nullptr), ifPtr(interface){}

    inline explicit operator bool() const
        { return handler != NONE; }
    
    inline void operator()(View* v, const std::string& before, const std::string& after){ 
        switch (handler){
            case FUNCTION_PTR:{
                fnPtr(v, before, after); 
            }break;
            case INTERFACE:{
                ifPtr->onTextChanged(v, before, after); 
            }break;
            default: break;
        }
    }
};

};

#endif // IXTLI_EVENTS_KEYBOARD_EVENT_H