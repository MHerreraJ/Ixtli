#ifndef IXTLI_GRAPHICS_CANVAS_H
#define IXTLI_GRAPHICS_CANVAS_H

#include <mutex>
#include <memory>
#include <Ixtli/Core/Types.h>
#include <Ixtli/Graphics/Paint.h>
#include <Ixtli/Graphics/Rectangle.h>

namespace Ixtli{

class Canvas{
    private:
        std::shared_ptr<Mutex> mutexLayer;
        std::shared_ptr<int> layerCount;
    public:
        Canvas() : mutexLayer(std::make_shared<Mutex>()), layerCount(std::make_shared<int>(0)) {};
        virtual ~Canvas();
    
        virtual void save();
        virtual void restore();

        virtual void scale(float x, float y);
        virtual void translate(float x, float y);

        virtual void drawRect(const Rect& rect, const Paint& paint);
        virtual void drawRect(const RectF& rect, const Paint& paint);
        virtual void drawRect(float left, float top, float right, float bottom, const Paint& paint);

        virtual void drawText(const std::string& text, float x, float y, const Paint& paint);

        virtual void drawLine(const PointF& start, const PointF& end, const Paint& paint);
        virtual void drawLine(float startX, float startY, float stopX, float stopY, const Paint& paint);

};

};

#endif //IXTLI_GRAPHICS_CANVAS_H