#ifndef IXTLI_WIDGET_LINEAR_LAYOUT_H
#define IXTLI_WIDGET_LINEAR_LAYOUT_H

#include <Ixtli/View/View.h>

namespace Ixtli{

    class LinearLayout : public View {
        private:
            bool horizontal;

        public:
            LinearLayout(bool horizontal = true) : View(), horizontal(horizontal) {}
            virtual ~LinearLayout() {}

            // void onDraw(Canvas& c) override;

            // void onSizeChanged(int w, int h, int oldw, int oldh) override;
    };


}

#endif //IXTLI_WIDGET_LINEAR_LAYOUT_H