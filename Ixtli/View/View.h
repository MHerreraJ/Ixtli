#ifndef IXTLI_VIEW_VIEW_H
#define IXTLI_VIEW_VIEW_H

#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

#include <Ixtli/Uuid.h>
#include <Ixtli/Core/Types.h>
#include <Ixtli/Events/MouseEvent.h>
#include <Ixtli/Events/KeyboardEvent.h>
#include <Ixtli/View/Viewport2D.h>
#include <Ixtli/Graphics/Color.h>
#include <Ixtli/Graphics/Drawable.h>
#include <Ixtli/Graphics/ColorDrawable.h>
#include <Ixtli/View/LayoutParams.h>
#include <Ixtli/View/MarginLayoutParams.h>

namespace Ixtli{

class Window;
class View;



class View : MouseEvent, KeyboardEvent {
    friend class Window;
    private:
        UUID id;
        Mutex mutexView;
        Mutex mutexDraw;
        Mutex mutexEventMouse;

        View* parent;
        Window* windowContext;
        Viewport viewport;

        std::vector<UUID> orderedChilds;
        std::unordered_map<UUID, std::shared_ptr<View>> childs;

        int width;
        int height;
        bool updateLayout;
        MarginLayoutParams layoutParams;
        std::shared_ptr<Drawable> background;


        bool isInParentViewport(const Viewport& vp) const ;
        void updateViewport();
        void updateViewport(const Viewport& vp);

        void onParentDrawEventRequest(const Viewport& vp);
        void onParentResizeEventRequest(int parentWidth, int parentHeight);
        bool onParentMouseEventRequest(MouseButton btn, MouseAction action, int x, int y);
        void onParentKeyboardEventRequest(int key, KeyAction action);


        OnClickListenerHandler clickListener;

    protected:
        virtual void onViewCreated(bool& textFocusable){ textFocusable = false; }

    public:
        View();
        virtual ~View() {}

    public:
        inline const UUID& getID() const
            { return id; }
        
        inline const MarginLayoutParams& getLayoutParams() const
            { return layoutParams; }

        inline const std::shared_ptr<Drawable> getBackground() const
            { return background; }            

        inline int getWidth() const
            { return width; }
        
        inline int getHeight() const 
            { return height; }
        
        inline void setOnClickListener(IOnClickListener listener)
            { clickListener = listener; }
        
        inline void setOnClickListener(OnClickListener* listener)
            { clickListener = listener; }

        void setBackgroundColor(const Color& c);
        void setWidth(int w);
        void setHeight(int h);
        void setLayoutParams(const MarginLayoutParams& margins);

        template <class V>
        inline bool addView(const std::shared_ptr<V>& v)
            { if(!v) return false; return addView(std::dynamic_pointer_cast<View>(v)); }

        bool addView(const std::shared_ptr<View>& v);

        std::shared_ptr<View> findViewByID(UUID childId) const;

        template<class V>
        inline std::shared_ptr<V> findViewByID(UUID childId) const
            { return std::dynamic_pointer_cast<V>(findViewByID(childId)); }
        

        void invalidate();
        
        virtual void onDraw(Canvas& canvas);
        virtual void onSizeChanged(int w, int h, int oldw, int oldh){}
        virtual void onKeyPressed(int key){}

        virtual void onKeyEvent(int key, KeyAction action) override {}
        virtual void onMouseEvent(MouseButton btn, MouseAction action, int x, int y) override {}
};

inline std::ostream& operator << (std::ostream& os, const View& v){
    os << "<View: " << v.getID() << ">" << std::endl;
    return os;
}


};

#endif //IXTLI_VIEW_VIEW_H