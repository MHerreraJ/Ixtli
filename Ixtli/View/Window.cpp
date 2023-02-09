#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <GL/freeglut.h>
#include <Ixtli/View/ContextProvider.h>
#include <Ixtli/View/Window.h>

using namespace Ixtli;

Window::Window() : Context(), title(""), left(0), top(0), width(0), height(0), focusedViewID(), root(nullptr), toastView(nullptr), pendingInvalidate(false){
    std::thread(&Window::toastDisplayThread, this).detach();
}


void Window::onWindowRootChangedEvent(){
    if(!root) return;
    root->parent = nullptr;
    root->context = this;
    root->onParentResizeEventRequest(width, height);
}

void Window::invalidate(){
    if(!pendingInvalidate){
        ContextProvider::getProvider()->windowInvalidate(getID());
        pendingInvalidate = true;
    }
}

void Window::notify(Toast* toast){
    if(toast){
        messageQueue.push({*toast, toast->getDuration() == Toast::LENGTH_LONG? 3000 : 1500});
    }
}

void Window::toastDisplayThread(){
    for(;;){
        if(messageQueue.size() > 0){
            auto message = messageQueue.front();
            messageQueue.pop();

            auto time = message.second;
            auto tUp = time/6;
            auto tDown = time/6;
            auto tVis = time - tUp - tDown;

            toastView = message.first.getView();
            toastView->setTransparency(0);
            invalidate();

            size_t t = 0;
            size_t dtms = 30;
            float alpha = 0;
            float daUp = ((float)dtms)/tUp;
            float daDown = ((float)dtms)/tDown;

            while(t < tUp){
                t += dtms;
                alpha += daUp;
                toastView->setTransparency(alpha);
                invalidate();
                std::this_thread::sleep_for(std::chrono::milliseconds(dtms));
            }

            toastView->setTransparency(1.0);
            invalidate();
            std::this_thread::sleep_for(std::chrono::milliseconds(tVis));

            t = 0;
            alpha = 1.0;
            while(t < tDown){
                t += dtms;
                alpha -= daDown;
                toastView->setTransparency(alpha);
                invalidate();
                std::this_thread::sleep_for(std::chrono::milliseconds(dtms));
            }


            //std::this_thread::sleep_for(std::chrono::milliseconds(message.second));
        
            toastView = nullptr;
            invalidate();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}


void Window::onWindowResizedEvent(int w, int h){
    width = w;
    height = h;
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, w, h, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if(root){
        root->onParentResizeEventRequest(w, h);
    }
}

void Window::onWindowReshapeEvent(){
    if(root){
        root->onParentDrawEventRequest(Viewport(0, 0, width, height));
    }
    if(toastView){
        toastView->setWidth(width/2);
        toastView->setHeight(50);
        toastView->setLayoutParams(MarginLayoutParams(LayoutParams::WRAP_CONTENT, width/4, width/4, height-100, 0));
        toastView->onParentDrawEventRequest(Viewport(0,0, width, height));
    }
    pendingInvalidate = false;
}

void Window::onWindowMouseEvent(MouseButton btn, MouseAction action, int x, int y){
    if(root){
        root->onParentMouseEventRequest(btn, action, x,  height - y);
    }
}

void Window::onWindowKeyPressedEvent(int key, KeyAction action){
    if(key == Key::TAB && action == KeyAction::KEY_DOWN && focusableViews.size() > 0){
        UUID lastFocus = focusedViewID;

        if(focusedViewID == UUID::UUID_NONE){
            focusedViewID = focusableViews[0];
        }else{
            int idx = -1;
            for(size_t i = 0; i < focusableViews.size(); i++){
                if(focusedViewID == focusableViews[i]){
                    idx = i;
                    break;
                }
            }
            if(idx < 0){
                focusedViewID = focusableViews[0];
            }else{
                idx = (idx + 1) % focusableViews.size();
                focusedViewID = focusableViews[idx];
            }
        }
        if(lastFocus != focusedViewID){
            auto view = findViewByID(focusedViewID);
            auto focusedView = findViewByID(lastFocus);

            if(focusedView){
                focusedView->onParentFocusChangeEventRequest(false);
            }
            if(view){
                view->onParentFocusChangeEventRequest(true);
            }
        }
        return;
    }

    if(focusedViewID == UUID::UUID_NONE) return;
    
    auto view = findViewByID(focusedViewID);
    if(!view) return;

    view->onParentKeyboardEventRequest(key, action);

}

void Window::requestFocus(UUID viewID){
    if(viewID == focusedViewID) return;
    
    auto view = findViewByID(viewID);
    auto focusedView = findViewByID(focusedViewID);
    if(focusedView){
        focusedView->onParentFocusChangeEventRequest(false);
    }
    
    if(view != nullptr){
        focusedViewID = viewID;
        view->onParentFocusChangeEventRequest(true);
    }else{        
        focusedViewID = UUID();
    }
}

void Window::registerKeyboardInputView(UUID viewID){
    focusableViews.push_back(viewID);
}