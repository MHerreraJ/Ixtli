#ifndef IXTLI_VIEW_MARGIN_LAYOUT_PARAMS_H
#define IXTLI_VIEW_MARGIN_LAYOUT_PARAMS_H

#include <Ixtli/View/LayoutParams.h>

namespace Ixtli{

class MarginLayoutParams : public LayoutParams {
    private:
        int left, right, top, bottom;

    public:
        MarginLayoutParams() : LayoutParams(), left(0), right(0), top(0), bottom(0) {}
        MarginLayoutParams(FillMode fill) : LayoutParams(fill, fill), left(0), right(0), top(0), bottom(0) {}
        MarginLayoutParams(FillMode width, FillMode height) : LayoutParams(width, height), left(0), right(0), top(0), bottom(0) {}

        MarginLayoutParams(int left, int right, int top, int bottom) : LayoutParams(), left(left), right(right), top(top), bottom(bottom) {}

        MarginLayoutParams(FillMode fill, int left, int right, int top, int bottom) : LayoutParams(fill, fill), left(left), right(right), top(top), bottom(bottom) {}
        MarginLayoutParams(FillMode width, FillMode height, int left, int right, int top, int bottom) : LayoutParams(width, height), left(left), right(right), top(top), bottom(bottom) {}


        inline int getLeft() const 
            { return left; }
        
        inline int getRight() const 
            { return right; }
        
        inline int getTop() const 
            { return top; }
        
        inline int getBottom() const 
            { return bottom; }
        
};

inline std::ostream& operator << (std::ostream& os, const MarginLayoutParams& m){
    os << "(w: " << LayoutParams::toString(m.getWidth()) << ", h: " << LayoutParams::toString(m.getHeight()) \
        << ", left: " << m.getLeft() << ", right: " << m.getRight() << ", top: " << m.getTop() << ", bottom: " << m.getBottom() << ")";
    return os;
}

};

#endif //IXTLI_VIEW_MARGIN_LAYOUT_PARAMS_H