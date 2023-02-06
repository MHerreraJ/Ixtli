#ifndef IXTLI_VIEW_LAYOUT_PARAMS_H
#define IXTLI_VIEW_LAYOUT_PARAMS_H

namespace Ixtli{

class LayoutParams{
    public:
        enum FillMode : int {
            MATCH_PARENT = -1,
            WRAP_CONTENT = -2
        };
    private:
        FillMode width, height;

    public:
        LayoutParams() : width(MATCH_PARENT), height(MATCH_PARENT) {};
        LayoutParams(FillMode fill) : width(fill), height(fill){}
        LayoutParams(FillMode width, FillMode height) : width(width), height(height) {}

        inline FillMode getWidth() const
            { return width; }
        
        inline FillMode getHeight() const
            { return height; }
        
        inline void setWidth(FillMode w)
            { width = w; }

        inline void setHeight(FillMode h)
            { height = h; }
        
        inline static std::string toString(FillMode mode){
            switch (mode){
                case MATCH_PARENT: return "MATCH PARENT";
                case WRAP_CONTENT: return "WRAP CONTENT";
            }
            return "NONE";
        }
};


};

#endif //IXTLI_VIEW_LAYOUT_PARAMS_H