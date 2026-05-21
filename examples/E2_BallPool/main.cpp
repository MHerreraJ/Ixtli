#include <Ixtli/View/ContextProvider.h>
#include <Ixtli/View/Window.h>
#include "Ball.h"
#include "BallPool.h"

using namespace Ixtli;

class PolishCow : public Window{
    public:
        PolishCow() : Window(){}


    void onCreate() override;
};


void PolishCow::onCreate(){
    auto root = setRootView();
    auto ball = std::make_shared<BallPool>();

    root->addView(ball);

}

int main(int argc, char** argv){
    auto provider = ContextProvider::getProvider();
    auto window = provider->createWindow<PolishCow>();
    provider->transferMainControl();
    return 0;
}