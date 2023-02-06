#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <GL/freeglut.h>
#include <Ixtli/View/ContextProvider.h>
#include <Ixtli/View/Window.h>

using namespace Ixtli;


void Window::onWindowRootChangedEvent(){
    if(!root) return;
    root->parent = nullptr;
    root->windowContext = this;
    root->onParentResizeEventRequest(width, height);
}

void Window::invalidate(){
    if(!pendingInvalidate){
        ContextProvider::getProvider()->windowInvalidate(id);
        pendingInvalidate = true;
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
    pendingInvalidate = false;
}

void Window::onWindowMouseEvent(MouseButton btn, MouseAction action, int x, int y){
    if(root){
        root->onParentMouseEventRequest(btn, action, x,  height - y);
    }
}

void Window::onWindowKeyPressedEvent(int key, KeyAction action){
    //std::cout << "PRESSED " << key << ": " << (char)key << std::endl;
    if(key == Key::TAB && focusableViews.size() > 0){
        if(focusedView == UUID::UUID_NONE){
            focusedView = focusableViews[0];
        }else{
            int idx = -1;
            for(size_t i = 0; i < focusableViews.size(); i++){
                if(focusedView == focusableViews[i]){
                    idx = i;
                    break;
                }
            }
            if(idx < 0){
                focusedView = focusableViews[0];
            }else{
                idx = (idx + 1) % focusableViews.size();
                focusedView = focusableViews[idx];
            }
        }
        return;
    }

    if(focusedView == UUID::UUID_NONE) return;
    
    auto view = findViewByID(focusedView);
    if(!view) return;

    view->onParentKeyboardEventRequest(key, action);

}

void Window::setFocus(UUID viewID){
    if(findViewByID(viewID) != nullptr){
        focusedView = viewID;
    }else{
        focusedView = UUID();
    }
}

void Window::addToFocusableViews(UUID viewID){
    focusableViews.push_back(viewID);
}