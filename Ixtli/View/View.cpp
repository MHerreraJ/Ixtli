#include <iostream>
#include <GL/freeglut.h>

#include <Ixtli/Math.h>
#include <Ixtli/View/View.h>
#include <Ixtli/View/Window.h>

using namespace Ixtli;

View::View() : mutexView(), mutexDraw(), mutexEventMouse(), parent(nullptr), windowContext(nullptr), viewport(), 
    orderedChilds(), childs(), width(0), height(0), updateLayout(false), layoutParams(), background(), 
    clickListener() {
        
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

void View::onParentDrawEventRequest(const Viewport& pvp){
    mutexDraw.lock();
    if(updateLayout){
        onParentResizeEventRequest(pvp.width(), pvp.height());
        updateLayout = false;
    }

    //std::cout << __LINE__ << " DRAWING " << getID() << std::endl;

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
}

bool View::onParentMouseEventRequest(MouseButton btn, MouseAction action, int x, int y){
    if(!mutexEventMouse.try_lock()) {
        return false;
    }
    /*if(!mutexDraw.try_lock()) {
        mutexEventMouse.unlock();
        return false;
    }*/
    mutexDraw.lock();
    
    bool handleClickListener = true; 
    for(auto childID = orderedChilds.rbegin(); childID != orderedChilds.rend(); childID++){
        if(childs.count(*childID) == 0) continue;
        auto childView = childs[*childID];
        if(childView->onParentMouseEventRequest(btn, action, x, y)){
            handleClickListener = false;
        }
    }

   
    if(viewport.contains(Point(x, y))){        
        onMouseEvent(btn, action, x,  height - y);
        if(handleClickListener && action == MouseAction::PRESSED_DOWN){
            if(clickListener){
                clickListener(this, btn, x, y);
            }
            if(windowContext){
                windowContext->setFocus(getID());
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

void View::setBackgroundColor(const Color& c){
    background = std::shared_ptr<Drawable>(new ColorDrawable(c));
    invalidate();
}

void View::setWidth(int w){
    if(layoutParams.getWidth() == LayoutParams::MATCH_PARENT){
        layoutParams.setWidth(LayoutParams::WRAP_CONTENT);
    }
    width = w;
    //updateViewport();
    updateLayout = true;
    invalidate();
}

void View::setHeight(int h){
    if(layoutParams.getHeight() == LayoutParams::MATCH_PARENT){
        layoutParams.setHeight(LayoutParams::WRAP_CONTENT);
    }
    height = h;
    updateLayout = true;
    //updateViewport();
    invalidate();
}

void View::setLayoutParams(const MarginLayoutParams& margins){
    layoutParams = margins;
    updateLayout = true;
    invalidate();
}


bool View::addView(const std::shared_ptr<View>& v){
    bool added = false;
    do{
        if(!v || (v->parent && (v->parent != this)) || v->id == UUID::UUID_NONE ) break;
        v->parent = this;
        v->windowContext = windowContext;
        v->updateLayout = true;

        childs[v->id] = v;
        orderedChilds.push_back(v->id);
        added = true;
        
        if(windowContext){
            windowContext->setFocus(v->getID());
        }
        bool focusable = false;
        v->onViewCreated(focusable);

        if(focusable && windowContext){
            windowContext->addToFocusableViews(v->getID());
        }

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
    if(windowContext){
        windowContext->invalidate();
    }
}

void View::onDraw(Canvas& canvas){
    background->setBounds(Rect(0, 0, width, height));
    background->draw(canvas);
}