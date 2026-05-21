#include <thread>
#include <chrono>
#include <iostream>
#include <random>
#include "BallPool.h"

using namespace Ixtli;

BallPool::BallPool() : View(), idProvider(0), pool(), runThreads(true), runningThread(false),
    lastTick(getTime()){
    rng = std::mt19937(std::random_device{}());
    dist = std::uniform_real_distribution<float>(25.0f, 300.0f);
    color_dist = std::uniform_int_distribution<unsigned int>(0, 0xFFFFFFFF);
    selectedBall = -1;
}

BallPool::~BallPool(){
    runThreads = false;
    while(runningThread);
}

void BallPool::onAttachedToWindow(){
    if(!runningThread){
        auto t = std::thread(&BallPool::physicsThread, this);
        t.detach();
    }
}

void BallPool::onDraw(Canvas& canvas){
    for(auto& ball : pool){
        ball.draw(canvas);
    }
}

void BallPool::onMouseEvent(MouseButton btn, MouseAction action, int x, int y){
    if(action == MouseAction::PRESSED_DOWN){
        for(auto& ball:pool){
            if(ball.getPosition().distanceSquaredTo(PointF(x, y)) < ball.getRadius()*ball.getRadius()){
                selectedBall = ball.getID();
                lastMousePos = PointF(x, y);
                lastMouseEvent = getTime();
                return;
            }
        }
        if(btn == MouseButton::LEFT){
            Ball b{idProvider++};
            b.setRadius(0.25*dist(rng)).setPosition(PointF(x, y)).setVelocity(PointF(dist(rng)*0.5f, 0.0f));
            b.setMass((int)b.getRadius());
            Color c(color_dist(rng));
            c.setAlpha(255);
            b.setBackground(Paint().setColor(c));
            pool.push_back(std::move(b));
            invalidate();
        }else if(btn == MouseButton::RIGHT){
            Ball b{idProvider++};
            b.setRadius(0.25*dist(rng)).setPosition(PointF(x, y)).setVelocity(PointF(-dist(rng)*0.5f, 0.0f));
            b.setMass((int)b.getRadius());
            Color c(color_dist(rng));
            c.setAlpha(255);
            b.setBackground(Paint().setColor(c));
            pool.push_back(std::move(b));
            invalidate();
        }
    }else if(action == MouseAction::MOVE){
        for(auto& ball:pool){
            if(ball.getPosition().distanceSquaredTo(PointF(x, y)) < ball.getRadius()*ball.getRadius()){
                if(ball.getID() == selectedBall){
                    PointF pos = PointF(x, y);
                    float dt = timeDiff_ms<float>(lastMouseEvent, getTime());
                    ball.setPosition(pos);
                    ball.setVelocity(2.0f*(1.0f/dt)*(pos - lastMousePos));
                    invalidate();
                    lastMouseEvent = getTime();
                    lastMousePos = pos;
                    return;
                }
            }
        }
    }else if(action==MouseAction::PRESSED_UP){
        for(auto& ball:pool){
            if(ball.getID() == selectedBall){
                PointF pos = PointF(x, y);
                float dt = timeDiff_ms<float>(lastMouseEvent, getTime());
                ball.setPosition(pos);
                ball.setVelocity(2.0f*(1.0f/dt)*(pos - lastMousePos));
                selectedBall = -1;
                invalidate();
                lastMouseEvent = getTime();
                lastMousePos = pos;
                return;
            }
        }
        selectedBall = -1;
    }
}

void BallPool::onKeyEvent(int key, KeyAction action){
    if(action == KeyAction::KEY_DOWN){
        if(key == static_cast<int>(Key::SPACE)){
            for(auto& ball : pool){   
                const PointF& vel = ball.getVelocity();
                int dirx = (vel.x > 0) ? 1 : -1;
                int diry = (vel.y > 0) ? 1 : -1;
                ball.setVelocity(ball.getVelocity()+PointF(dirx*(dist(rng)), diry*(dist(rng))));
            }
            invalidate();
        }else if(key == 'r'){
            pool.clear();
            invalidate();
        }
    }
}

#define RESISTANCE 0.9f

void BallPool::onTick(long long int dt){
    std::vector<PointF> newPositions(pool.size());
    std::vector<PointF> newVelocities(pool.size());
    
    for(size_t i=0; i<pool.size(); i++){
        size_t count = 1;
        PointF finalPos = pool[i].getPosition();
        PointF finalVel = pool[i].getVelocity();

        Ball& ball = pool[i];
        if(selectedBall == ball.getID()){
            newPositions[i] =  finalPos;
            newVelocities[i] = finalVel;
            continue;
        }
        PointF pos = ball.getPosition();
        PointF vel = ball.getVelocity();
        float t = ((float)dt)/100.0f;
        
        pos += vel*t;
        vel.y += (9.8f*getHeight()/1000.0f)*t;
        if(fabs(vel.y) < 5.5f && pos.y + ball.getRadius() >= getHeight()){
            vel.y = 0.0f;
        }
        
        if(pos.y + ball.getRadius() >= getHeight()){
            pos.y = getHeight() - ball.getRadius();
            vel.y = -fabs(vel.y)*RESISTANCE*0.9f;
        }else if(pos.y - ball.getRadius() <= 0){
            pos.y = ball.getRadius();
            vel.y = fabs(vel.y)*RESISTANCE;
        }

        if(pos.x + ball.getRadius() > getWidth()){
            pos.x = getWidth() - ball.getRadius();
            vel.x = -vel.x*RESISTANCE;
        }else if(pos.x - ball.getRadius() < 0){
            pos.x = ball.getRadius();
            vel.x = -vel.x*RESISTANCE;
        }

        if(pos.y + ball.getRadius() >= getHeight() && fabs(vel.y) < 1.0f){
            vel.x*=0.999f;
        }

        if(fabs(vel.x) < 0.1f){
            vel.x = 0.0f;
        }

        finalPos = pos;
        finalVel = vel;

        for(size_t j=0; j<pool.size(); j++){
            Ball& ball2 = pool[j];
            if(&ball == &ball2) continue;
            if(ball.collides(ball2)){
                PointF v1 = ball.getVelocity();
                PointF v2 = ball2.getVelocity();
                PointF p1 = ball.getPosition();
                PointF p2 = ball2.getPosition();
                PointF n = (p1 - p2);
                n = n*(1.0f/n.length());
                float m1 = ball.getMass();
                float m2 = ball2.getMass();
                float v1n = v1.dot(n);
                float v2n = v2.dot(n);
                float v1t = v1.dot(n);
                float v2t = v2.dot(n);
                float v1n_ = (v1n*(m1 - m2) + 2*m2*v2n)/(m1 + m2);
                float v2n_ = (v2n*(m2 - m1) + 2*m1*v1n)/(m1 + m2);
                v1 = v1 - RESISTANCE*n*(v1n - v1n_);
                v2 = v2 - RESISTANCE*n*(v2n - v2n_);

                finalPos += n;
                
                finalVel += v1;
                count++;
                // ball.setVelocity(v1);
                // ball2.setVelocity(v2);
                // ball.setPosition(ball.getPosition() + n);
                // ball2.setPosition(ball2.getPosition() - n);
            }
        }
        // newPositions[i] =  (1/(float)count)*finalPos;
        newPositions[i] =  finalPos;
        newVelocities[i] = (1/(float)count)*finalVel;
    }
    for(size_t i=0; i<pool.size(); i++){
        pool[i].setPosition(newPositions[i]);
        pool[i].setVelocity(newVelocities[i]);
    }
    invalidate();
}

void BallPool::physicsThread(){
    runningThread = true;
    while(runThreads){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto now = getTime();
        auto dt = timeDiff_ms<long long int>(lastTick, now);
        onTick(dt);
        lastTick = getTime();
    }
    runningThread = false;
}