#ifndef BALL_H
#define BALL_H


#include <Ixtli/Graphics/Canvas.h>

namespace Ixtli{

class Ball{
private:
    int id;
    Paint background;
    Paint foreground;
    float radius;
    PointF position;
    PointF velocity;
    float mass;

public:
    Ball(int id);
    virtual ~Ball() = default;


    inline Ball& setBackground(const Paint& p){
        background = p;
        return *this;
    }

    inline Ball& setForeground(const Paint& p){
        foreground = p;
        return *this;
    }

    inline Ball& setRadius(float r){
        radius = r;
        return *this;
    }

    inline Ball& setPosition(const PointF& p){
        position = p;
        return *this;
    }

    inline Ball& setVelocity(const PointF& v){
        velocity = v;
        return *this;
    }

    inline int getID() const
        { return id; }

    inline const PointF& getPosition() const
        { return position; }
    
    inline const PointF& getVelocity() const
        { return velocity; }
    
    inline float getRadius() const
        { return radius; }

    inline void setMass(float m)
        { mass = m; }

    inline float getMass() const
        { return mass; }    



    bool inside(const PointF& p) const{
        return position.distanceSquaredTo(p) < radius*radius;
    }
    bool collides(const Ball& b) const;

    void draw(Canvas&);
};

}

#endif //BALL_H