#include "Ball.h"

using namespace Ixtli;

Ball::Ball(int id) : id(id), background(), foreground(), radius(1.0f), position(0,0), velocity(17.0, 2.0f), mass(1.0f){
    background.setColor(Color::WHITE);
    foreground.setColor(Color::RED);
    foreground.setStyle(Paint::Style::STROKE);
    foreground.setStrokeWidth(2.0f);
}


void Ball::draw(Canvas& canvas){
    // canvas.drawCircle(position, radius, foreground);
    canvas.drawCircle(position, radius, background);
    Paint p{};
    p.setColor(Color::WHITE);
    Rect bounds{};
    p.getTextBounds(std::to_string((int)mass), bounds);


    canvas.drawText(std::to_string((int)mass), position.x - bounds.width()/2, position.y - bounds.height()/2, p);
}


bool Ball::collides(const Ball& b) const{
    return position.distanceSquaredTo(b.position) < (radius + b.radius)*(radius + b.radius);
}
