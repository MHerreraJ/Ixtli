#ifndef IXTLI_VIEW_WINDOW_H
#define IXTLI_VIEW_WINDOW_H

#include <vector>
#include <Ixtli/Uuid.h>
#include <Ixtli/View/View.h>
#include <Ixtli/Core/Types.h>
#include <Ixtli/Graphics/Point2D.h>

namespace Ixtli{

class ContextProvider;
void ContextDisplayHandler();
void ContextResizeHandler(int w, int h);
void ContextMouseHandler(int button, int action, int x, int y);
void ContextMouseMotionHandler(int x, int y);
void ContextKeyboardHandler(unsigned char key, int x, int y);
void ContextSpecialKeyHandler(int key, int x, int y);
void ContextSpecialKeyUpHandler(int key, int x, int y);
void ContextCloseHandler();
void ContextIdleHandler();

class Window{
    private:
        int id;
        Mutex drawMutex;
        Mutex mouseMutex;
        std::string title;

        int left, top;
        int width, height;

        UUID focusedView;
        std::vector<UUID> focusableViews;
        std::shared_ptr<View> root;
        bool pendingInvalidate;

        void onWindowRootChangedEvent();
        void onWindowResizedEvent(int w, int h);
        void onWindowReshapeEvent();
        void onWindowMouseEvent(MouseButton btn, MouseAction action, int x, int y);
        void onWindowKeyPressedEvent(int key, KeyAction action);

    public:
        Window() : id(0), drawMutex(), mouseMutex(), title(""), left(0), top(0), width(0), height(0), focusedView(), root(nullptr), pendingInvalidate(false){};
    public:
        virtual ~Window() {} 

    public:
        inline int getID() const
            { return id; }

        template <typename V = View, typename ...Args>
        inline std::shared_ptr<V> setRootView(Args&&... args){
            auto child = std::make_shared<V>(args ...);
            root = std::dynamic_pointer_cast<View>(child);
            onWindowRootChangedEvent();
            return child;
        }

        inline std::shared_ptr<View> getRootView()
            { return root; }

        template<class V = View>
        inline std::shared_ptr<V> findViewByID(UUID childId) const { 
            if(root == nullptr) return nullptr;

            if(root->id == childId) return std::dynamic_pointer_cast<V>(root); 
            return std::dynamic_pointer_cast<V>(root->findViewByID(childId)); 
        }

        void setFocus(UUID viewID);
        void addToFocusableViews(UUID viewID);

        void invalidate();

        virtual void onCreate() {}
        virtual void onWindowClosed() {}

    friend void ContextDisplayHandler();
    friend void ContextResizeHandler(int w, int h);
    friend void ContextMouseHandler(int button, int action, int x, int y);
    friend void ContextMouseMotionHandler(int x, int y);
    friend void ContextKeyboardHandler(unsigned char key, int x, int y);
    friend void ContextCloseHandler();
    friend void ContextIdleHandler();

    friend class ContextProvider;
};

};

#endif //IXTLI_VIEW_WINDOW_H