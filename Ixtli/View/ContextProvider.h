#ifndef IXTLI_VIEW_CONTEXT_PROVIDER_H
#define IXTLI_VIEW_CONTEXT_PROVIDER_H

#include <iostream>
#include <memory>
#include <Ixtli/View/Window.h>
#include <Ixtli/Graphics/Point2D.h>

namespace Ixtli{

class ContextProvider{
    private:
        bool mainControl;
        ContextProvider() : mainControl(false) {}
        ~ContextProvider(){}

        void windowContextCreate(std::shared_ptr<Window> win, int w, int h, const Point& pos, const std::string& title);

    public:
        void transferMainControl();
        void windowInvalidate(int wID);

        template <class W = Window, typename ...Args>
        std::shared_ptr<W> createWindow(int w = 1024, int h = 720, const Point& pos=Point(100, 100), const std::string& title="Window", Args&&... args){
            static_assert(std::is_base_of<Window, W>::value, "Must derive from window.");
            auto window = std::make_shared<W>(args...);
            auto ptr = std::dynamic_pointer_cast<Window>(window);

            windowContextCreate(ptr, w, h, pos, title);
            return window;
        }

        static ContextProvider* getProvider();

};

};

#endif //IXTLI_VIEW_CONTEXT_PROVIDER_H
