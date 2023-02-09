#include <iostream>
#include <GL/freeglut.h>

#include <Ixtli/Math.h>
#include <Ixtli/View/View.h>
#include <Ixtli/View/Window.h>

using namespace Ixtli;

View::View() : mutexDraw(), mutexEventMouse(), parent(nullptr), context(nullptr), viewport(), 
    orderedChilds(), childs(), width(0), height(0), updateLayout(false), layoutParams(), background(), 
    clickListener(), focusListener() {
        
    id = UUID::create();
}

bool View::isInParentViewport(const Viewport& vp) const {
    if( abs(layoutParams.getLeft()) > vp.width() ||
        abs(layoutParams.getRight()) > vp.width() ||
        abs(layoutParams.getTop()) > vp.height() ||
        abs(layoutParams.getBottom()) > vp.height() ) return false;
    
    return true;
}

void View::updateViewport(){
    if(!parent) return;
    updateViewport(parent->viewport);
}

void View::updateViewport(const Viewport& v){
    auto m = layoutParams;

    int left = min(max(v.left + m.getLeft(), v.left), max(min(v.right - m.getRight() - width, v.right), max(v.left, v.left + m.getLeft())));
    int bottom = max(min(v.top - m.getTop() - height, v.top - height), max(min(v.bottom + m.getBottom(), v.top), v.bottom));

    int w = min(width, max(0, v.width() - (m.getLeft() + m.getRight())));
    int h = min(height, max(0, v.height() - (m.getTop() + m.getBottom())));

    if(left + w > v.right){
        w = max(0, v.right - left);
    }
    if(bottom + h > v.top){
        h = max(0, v.top - bottom);
    }

    viewport = Viewport(left, bottom, w, h);
}

void View::onParentSetTransparencyEventRequest(float transparency){
    background->setTransparency(transparency);
    for(auto childID = orderedChilds.rbegin(); childID != orderedChilds.rend(); childID++){
        if(childs.count(*childID) == 0) continue;
        auto childView = childs[*childID];
        
        childView->onParentSetTransparencyEventRequest(transparency);
    }
}

void View::onParentFocusChangeEventRequest(bool hasFocus){
    onFocusChanged(hasFocus);
    focusListener(this, hasFocus);
}


void View::onParentDrawEventRequest(const Viewport& pvp){
    mutexDraw.lock();    
    if(updateLayout){
        onParentResizeEventRequest(pvp.width(), pvp.height());
        updateLayout = false;
    }

    if(width == 0 || height == 0 || !isInParentViewport(pvp)){
        mutexDraw.unlock();
        return;
    }

    Canvas surface{};
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(viewport.left, viewport.bottom, viewport.width(), viewport.height());
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    background->setBounds(Rect(0, 0, width, height));
    background->draw(surface);
    onDraw(surface);

    for(auto& viewID : orderedChilds){
        if(childs.count(viewID) == 0) continue;
        auto& childView = childs[viewID];
        childView->onParentDrawEventRequest(viewport);
    }
    glPopMatrix();
    mutexDraw.unlock();
}

void View::onParentResizeEventRequest(int parentWidth, int parentHeight){
    int oldW = width;
    int oldH = height;

    if(layoutParams.getWidth() == LayoutParams::MATCH_PARENT)
        width = max(0, min(parentWidth, parentWidth - (layoutParams.getLeft() + layoutParams.getRight())));

    if(layoutParams.getHeight() == LayoutParams::MATCH_PARENT)
        height = max(0, min(parentHeight, parentHeight - (layoutParams.getTop() + layoutParams.getBottom())));

    if(parent){
        updateViewport();
    }else{
        updateViewport(Viewport(0, 0, parentWidth, parentHeight));
    }


    if(oldW != width || oldH != height)
        onSizeChanged(width, height, oldW, oldH);  

    for(auto& child : childs){
        child.second->onParentResizeEventRequest(width, height);
    }
    invalidate();
}

bool View::onParentMouseEventRequest(MouseButton btn, MouseAction action, int x, int y){
    if(!mutexEventMouse.try_lock()) {
        return false;
    }
    mutexDraw.lock();
    
    bool handleClickListener = true; 
    for(auto childID = orderedChilds.rbegin(); childID != orderedChilds.rend(); childID++){
        if(childs.count(*childID) == 0) continue;
        auto childView = childs[*childID];
        if(childView->onParentMouseEventRequest(btn, action, x, y)){
            handleClickListener = false;
            break;
        }
    }

    if(viewport.contains(Point(x, y)) && handleClickListener){
        int mapY =  viewport.top - y;
        int mapX = x - viewport.left;

        onMouseEvent(btn, action, mapX,  mapY);
        if(handleClickListener && action == MouseAction::PRESSED_DOWN){
            clickListener(this, btn, mapX, mapY);
            if(context){
                context->requestFocus(getID());
            }
        }
        handleClickListener = true;
    }else{
        handleClickListener = false;
    }

    mutexDraw.unlock();
    mutexEventMouse.unlock();
    return handleClickListener;
}

void View::onParentKeyboardEventRequest(int key, KeyAction action){
    onKeyEvent(key, action);
}

bool View::hasFocus() const {
    if(!context){
        return false;
    }
    return context->hasFocus(getID());
}

void View::setBackgroundColor(const Color& c){
    background = std::shared_ptr<Drawable>(new ColorDrawable(c));
    invalidate();
}

void View::setWidth(int w){
    if(layoutParams.getWidth() == LayoutParams::MATCH_PARENT){
        layoutParams.setWidth(LayoutParams::WRAP_CONTENT);
    }
    width = w;
    updateLayout = true;
    invalidate();
}

void View::setHeight(int h){
    if(layoutParams.getHeight() == LayoutParams::MATCH_PARENT){
        layoutParams.setHeight(LayoutParams::WRAP_CONTENT);
    }
    height = h;
    updateLayout = true;
    invalidate();
}

void View::setLayoutParams(const MarginLayoutParams& margins){
    layoutParams = margins;
    updateLayout = true;
    invalidate();
}

void View::setTransparency(float transparency){
    onParentSetTransparencyEventRequest(transparency);
    invalidate();
}


bool View::addView(const std::shared_ptr<View>& v){
    bool added = false;
    do{
        if(!v || (v->parent && (v->parent != this)) || v->id == UUID::UUID_NONE ) break;
        v->parent = this;
        v->context = context;
        v->updateLayout = true;

        childs[v->id] = v;
        orderedChilds.push_back(v->id);
        added = true;
        
        if(context){
            context->requestFocus(v->getID());
        }

        if(v->acceptsKeyboardInput() && context){
            context->registerKeyboardInputView(v->getID());
        }

        v->onAttachedToWindow();
        invalidate();
    }while(0);
    return added;
}

std::shared_ptr<View> View::findViewByID(UUID i) const {
    if(childs.count(i) > 0){
        return childs.at(i);
    }

    std::shared_ptr<View> v = nullptr;
    for(auto& child : orderedChilds){
        v = childs.at(child)->findViewByID(i);
        if(v) return v;
    }
    return nullptr;
}

void View::invalidate(){
    if(context){
        context->invalidate();
    }
}

void View::onDraw(Canvas& canvas){

}