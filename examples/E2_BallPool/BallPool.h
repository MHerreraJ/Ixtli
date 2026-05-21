#ifndef BALLS_POOL_H
#define BALLS_POOL_H

#include <random>
#include <Ixtli/Core/IxtliTime.h>
#include <Ixtli/View/View.h>
#include <Ixtli/Graphics/Canvas.h>

#include "Ball.h"
namespace Ixtli{

class BallPool : public View{
private:
    int idProvider;
    std::vector<Ball> pool;

    bool runThreads;
    bool runningThread;
    time_s lastTick;

    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    std::uniform_int_distribution<unsigned int> color_dist;

    void physicsThread();
    void onTick(long long int dt);
    int selectedBall;
    
    time_s lastMouseEvent;
    PointF lastMousePos;

protected:
    void onAttachedToWindow() override;

public:
    BallPool();
    virtual ~BallPool();
    void onDraw(Canvas& canvas) override;

    void onMouseEvent(MouseButton btn, MouseAction action, int x, int y) override;
    void onKeyEvent(int key, KeyAction action) override;


};
}
#endif //BALLS_POOL_H