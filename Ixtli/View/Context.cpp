#include <iostream>
#include <Ixtli/Widget/Toast.h>
#include <Ixtli/View/Context.h>

using namespace Ixtli;

void Context::notify(Toast* toast){
    if(!toast) return;
    std::cout << "NOTIFY: " << toast->getText() << std::endl;
}
