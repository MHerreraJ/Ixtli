#include <iostream>
#include <GL/freeglut.h>
#include <Ixtli/Graphics/Canvas.h>

using namespace Ixtli;

Canvas::~Canvas(){
    mutexLayer->lock();
    while((*layerCount) > 0){
        glPopMatrix();
        (*layerCount)--;
    }
    (*layerCount) = -1;
    mutexLayer->unlock();
}

void Canvas::save(){
    mutexLayer->lock();
    if(layerCount < 0){
        mutexLayer->unlock();
        return;
    }
    glPushMatrix();
    (*layerCount)++;
    mutexLayer->unlock();
}

void Canvas::restore(){
    mutexLayer->lock();
    if((*layerCount) <= 0){
        mutexLayer->unlock();
        return;
    }
    glPopMatrix();
    (*layerCount)--;
    mutexLayer->unlock();
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
    glColor4ub(paint.color.r(), paint.color.g(), paint.color.b(), paint.color.a());
    glLineWidth(paint.strokeWidth);
    glBegin(GL_POLYGON);
        glVertex2i(rect.x1, rect.y1); //LEFT TOP CORNER
        glVertex2i(rect.x1, rect.y2);
        glVertex2i(rect.x2, rect.y2);
        glVertex2i(rect.x2, rect.y1);
        glVertex2i(rect.x1, rect.y1);
    glEnd();
    glPopMatrix();
}


void Canvas::drawRect(float left, float top, float right, float bottom, const Paint& paint){
    glPushMatrix();
    glColor4ub(paint.color.r(), paint.color.g(), paint.color.b(), paint.color.a());
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
    float ratio = paint.textSize / 104.76;
    auto& color = paint.color;
    //std::cout << "Draw text " << text << " at " << PointF(x, y) << std::endl;

    glPushMatrix();
    glLineWidth(paint.strokeWidth);
    glColor4ub(color.r(), color.g(), color.b(), color.a());
    glTranslatef(x, y, 0);
    glScalef(ratio, -ratio, 1.0f);
    glTranslatef(0, -119.05, 0);
    glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char*)(text.c_str()));
    glPopMatrix();
}

void Canvas::drawLine(const PointF& start, const PointF& end, const Paint& paint){
    drawLine(start.x, start.y, end.x, end.y, paint);
}

void Canvas::drawLine(float startX, float startY, float stopX, float stopY, const Paint& paint){
    auto color = paint.color;
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