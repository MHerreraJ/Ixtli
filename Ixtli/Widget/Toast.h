#ifndef IXTLI_WIDGET_TOAST_H
#define IXTLI_WIDGET_TOAST_H

#include <iostream>
#include <memory>

#include <Ixtli/Uuid.h>
#include <Ixtli/View/View.h>

namespace Ixtli{

class Context;

class Toast{
    public:
        static constexpr int LENGTH_SHORT = 0;
        static constexpr int LENGTH_LONG = 1;

    private:
        int duration;
        std::string text;
        Context* context;
        std::shared_ptr<View> view;

    public:
        Toast(Context* context);
        virtual ~Toast() {}

        void show();

        inline void setDuration(int d)
            { duration = d; }

        inline int getDuration() const
            { return duration; }
        
        inline void setText(const std::string& txt)
            { text = txt; }
        
        inline const std::string& getText() const
            { return text; }

        virtual std::shared_ptr<View> getView();

        static Toast makeText(Context* context, const std::string& text, int duration);
};

}

#endif //IXTLI_WIDGET_TOAST_H