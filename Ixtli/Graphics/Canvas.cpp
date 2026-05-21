#include <iostream>
#include <cmath>
#include <GL/freeglut.h>
#include <Ixtli/Graphics/Canvas.h>

using namespace Ixtli;


#define UNIT_CIRCLE_VERTEX 180

class CanvasCache{
    private:
        CanvasCache() = default;
        CanvasCache(const CanvasCache&) = delete;
        CanvasCache& operator=(const CanvasCache&) = delete;
        CanvasCache(CanvasCache&&) = delete;
        CanvasCache& operator=(CanvasCache&&) = delete;
    
        bool circleCache;
        GLfloat unitCircle[UNIT_CIRCLE_VERTEX*2];

        void generateUnitCircle(){
            float theta = 2.0f*M_PI/((float)UNIT_CIRCLE_VERTEX);

            for(int i=0; i<UNIT_CIRCLE_VERTEX; i++){
                unitCircle[i*2] = cosf(theta*i);
                unitCircle[i*2+1] = sinf(theta*i);
            }            
        }

    public:
        static CanvasCache& getCache(){
            static CanvasCache cache;
            return cache;
        }

        GLfloat* getUnitCircleVertex(){
            if(!circleCache){
                generateUnitCircle();
                circleCache = true;
            }
            return unitCircle;    
        }



};



Canvas::~Canvas(){
    // mutexLayer->lock();
    while((*layerCount) > 0){
        glPopMatrix();
        (*layerCount)--;
    }
    (*layerCount) = -1;
    // mutexLayer->unlock();
}

void Canvas::save(){
    // mutexLayer->lock();
    if(layerCount < 0){
        // mutexLayer->unlock();
        return;
    }
    glPushMatrix();
    (*layerCount)++;
    // mutexLayer->unlock();
}

void Canvas::restore(){
    // mutexLayer->lock();
    if((*layerCount) <= 0){
        // mutexLayer->unlock();
        return;
    }
    glPopMatrix();
    (*layerCount)--;
    // mutexLayer->unlock();
}

void Canvas::scale(float x, float y){
    if(x == 1 && y == 1) return;
    glScalef(x, y, 1.0f);
}

void Canvas::translate(float x, float y){
    if(x == 0 && y == 0) return;
    glTranslatef(x, y, 0.0f);
}

void Canvas::drawRect(const RectF& rect, const Paint& paint){
    drawRect(rect.x1, rect.y1, rect.x2, rect.y2, paint);
}

void Canvas::drawRect(const Rect& rect, const Paint& paint){
    glPushMatrix();
    Color c = paint.getColor();
    glColor4ub(c.r(), c.g(), c.b(), c.a());
    glLineWidth(paint.strokeWidth);
    glBegin(GL_POLYGON);
        glVertex2i(rect.x1, rect.y1); //LEFT TOP CORNER
        glVertex2i(rect.x1, rect.y2);
        glVertex2i(rect.x2, rect.y2);
        glVertex2i(rect.x2, rect.y1);
    glEnd();
    glPopMatrix();
}


void Canvas::drawRect(float left, float top, float right, float bottom, const Paint& paint){
    glPushMatrix();
    Color c = paint.getColor();
    glColor4ub(c.r(), c.g(), c.b(), c.a());
    glLineWidth(paint.strokeWidth);
    glBegin(GL_POLYGON);
        glVertex2f(left, top); //LEFT TOP CORNER
        glVertex2f(left, bottom);
        glVertex2f(right, bottom);
        glVertex2f(right, top);
        glVertex2f(left, top);
    glEnd();
    glPopMatrix();
}


void Canvas::drawText(const std::string& text, float x, float y, const Paint& paint){
    Color color = paint.getColor();

    if(paint.hasLoadedFont()){
        paint.getFont()->drawText(text, x, y, color, max(1U, static_cast<unsigned int>(paint.textSize)));
        return;
    }

    void* font = paint.getTextFont();

    glPushMatrix();
    glColor4ub(color.r(), color.g(), color.b(), color.a());
    glRasterPos2f(x, y + paint.getTextHeight());
    glutBitmapString(font, reinterpret_cast<const unsigned char*>(text.c_str()));
    glPopMatrix();
}

void Canvas::drawText(const std::string& text, int start, int end, float x, float y, const Paint& paint){
    if(start < 0 || end < 0 || end < start || text.length() < static_cast<size_t>(end)) return;
    drawText(text.substr(start, end-start), x, y, paint);
}


void Canvas::drawLine(const PointF& start, const PointF& end, const Paint& paint){
    drawLine(start.x, start.y, end.x, end.y, paint);
}

void Canvas::drawLine(float startX, float startY, float stopX, float stopY, const Paint& paint){
    auto color = paint.getColor();
    auto pattern = paint.pathEffect.pattern;
    auto factor = paint.pathEffect.factor;

    glPushMatrix();
    glColor4ub(color.r(), color.g(), color.b(), color.a());
    glLineWidth(paint.strokeWidth);

    if(pattern != 0xFFFF){
        glPushAttrib(GL_ENABLE_BIT); 
        glLineStipple(factor, pattern);
        glEnable(GL_LINE_STIPPLE);
    }

    glBegin(GL_LINES);
        glVertex2f(startX, startY);
        glVertex2f(stopX, stopY);
    glEnd();

    if(pattern != 0xFFFF){
        glPopAttrib();    
    }
    glPopMatrix();
}

void Canvas::drawPoint(float x, float y, const Paint& paint){
    auto color = paint.getColor();

    glPushMatrix();
    glColor4ub(color.r(), color.g(), color.b(), color.a());
    glPointSize(1.25*paint.strokeWidth);

    glBegin(GL_POINTS);
        glVertex2f(x, y);
    glEnd();


    glPopMatrix();
}

void Canvas::drawPoints(const std::vector<float>& points, const Paint& paint){
    auto color = paint.getColor();

    if(points.size() < 2){
        return;
    }

    glPushMatrix();
    glColor4ub(color.r(), color.g(), color.b(), color.a());
    glPointSize(1.25*paint.strokeWidth);

    glBegin(GL_POINTS);
        int i;
        int to = points.size() - points.size()%2;
        for(i=0; i<to; i+=2){
            float x = points[i];
            float y = points[i+1];
            glVertex2f(x, y);
        }

    glEnd();


    glPopMatrix();
}

void Canvas::drawCircle(const PointF& center, float radius, const Paint& paint){
    drawCircle(center.x, center.y, radius, paint);
}

void Canvas::drawCircle(float x, float y, float radius, const Paint& paint){
    Color c = paint.getColor();
    GLfloat* circle = CanvasCache::getCache().getUnitCircleVertex();
    glPushMatrix();
        glTranslatef(x, y, 0);
        glScalef(radius, radius, 1);
        glLineWidth(paint.strokeWidth);
        glColor4ub(c.r(), c.g(), c.b(), c.a());
        glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(2, GL_FLOAT, 0, circle);
            if(paint.style == Paint::Style::FILL || paint.style == Paint::Style::FILL_AND_STROKE){
                glDrawArrays(GL_POLYGON, 0, UNIT_CIRCLE_VERTEX);
            }else{
                glDrawArrays(GL_LINE_LOOP, 0, UNIT_CIRCLE_VERTEX);
            }
        glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
}
