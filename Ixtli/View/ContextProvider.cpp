#include <chrono>
#include <thread>
#include <unordered_set>
#include <unordered_map>

#include <GL/freeglut.h>
#include <Ixtli/View/Context.h>
#include <Ixtli/Events/MouseEvent.h>
#include <Ixtli/Events/KeyboardEvent.h>
#include <Ixtli/View/ContextProvider.h>

using namespace Ixtli;

static std::mutex glMutex;
static std::mutex mouseMutex;

static std::mutex trackerMutex;

static ContextProvider* contextProvider = nullptr;
static std::unordered_map<int, std::shared_ptr<Window>> tracker;
static std::unordered_set<int> windowUpdateRequest;

ContextProvider* ContextProvider::getProvider(){
    if(!contextProvider){
        int argc = 0;
        char** argv = nullptr;
        glutInit(&argc, argv);
        //glutSetOption(GLUT_MULTISAMPLE, 8);
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

        contextProvider = new ContextProvider();
    }
    return contextProvider;
}

void ContextProvider::transferMainControl(){
    if(!mainControl){
        mainControl = true;
        glutMainLoop();
    }
}


void Ixtli::ContextDisplayHandler(){

    int wID = glutGetWindow();
    if(tracker.count(wID) < 1) {
        return;
    }

    glPushMatrix();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    tracker[wID]->onWindowReshapeEvent();

    glFlush();
    glPopMatrix();
}

void Ixtli::ContextResizeHandler(int w, int h){
    int wID = glutGetWindow();
    if(tracker.count(wID) < 1) {
        return;
    }
    tracker[wID]->onWindowResizedEvent(w, h);
}


void Ixtli::ContextMouseHandler(int button, int state, int x, int y){
    int wID = glutGetWindow();
    if(tracker.count(wID) < 1) {
        return;
    }

    MouseAction action = MouseAction::NONE;
    MouseButton btn = MouseButton::NONE;

    if(state == GLUT_DOWN){
        action = MouseAction::PRESSED_DOWN;
    }else if(state == GLUT_UP){
        action = MouseAction::PRESSED_UP;
    }
    if(button == GLUT_LEFT_BUTTON){
        btn = MouseButton::LEFT;
    }else if(button == GLUT_RIGHT_BUTTON){
        btn = MouseButton::RIGHT;
    }else if(button == GLUT_MIDDLE_BUTTON){
        btn = MouseButton::MIDDLE;
    }else if(button == 3){
        btn = MouseButton::MIDDLE;
        action = MouseAction::SCROLL_UP;
    }else if(button == 4){
        btn = MouseButton::MIDDLE;
        action = MouseAction::SCROLL_DOWN;
    }
    //wMouseButtonTracker[wID] = btn;
    //wMouseEventTracker[wID] = action;
    
    auto thread = std::thread(&Window::onWindowMouseEvent, tracker[wID], btn, action, x, y);
    thread.detach();
    //tracker[wID]->onWindowMouseEvent(btn, action, x, y);
    //glutPostRedisplay();
}

void Ixtli::ContextMouseMotionHandler(int x, int y){
    if(mouseMutex.try_lock()){
        int wID = glutGetWindow();
        if(tracker.count(wID) < 1) {
            return;
        }
        MouseButton button = MouseButton::NONE;
        auto thread = std::thread(&Window::onWindowMouseEvent, tracker[wID], button, MouseAction::MOVE, x, y);
        thread.detach();
        //tracker[wID]->onWindowMouseEvent(button, MouseAction::MOVE, x, y);
        //glutPostRedisplay();
        mouseMutex.unlock();
    }
}

void Ixtli::ContextKeyDownHandler(unsigned char key, int x, int y){
    int wID = glutGetWindow();
    if(tracker.count(wID) < 1) {
        return;
    }
    tracker[wID]->onWindowKeyPressedEvent(key, KeyAction::KEY_DOWN);
}

void Ixtli::ContextKeyUpHandler(unsigned char key, int x, int y){
    int wID = glutGetWindow();
    if(tracker.count(wID) < 1) {
        return;
    }
    tracker[wID]->onWindowKeyPressedEvent(key, KeyAction::KEY_UP);
}

void Ixtli::ContextSpecialKeyHandler(int key, int x, int y){
    int wKey = static_cast<int>(Key::NUL);
    switch(key){
        case GLUT_KEY_LEFT:
        case GLUT_KEY_UP:
        case GLUT_KEY_RIGHT:
        case GLUT_KEY_DOWN:
        case GLUT_KEY_HOME:
        case GLUT_KEY_END:
            wKey = -static_cast<int>(key);
        break;

        default: break;
    }

    int wID = glutGetWindow();
    if(tracker.count(wID) < 1) {
        return;
    }
    tracker[wID]->onWindowKeyPressedEvent(wKey, KeyAction::KEY_DOWN);
}

void Ixtli::ContextSpecialKeyUpHandler(int key, int x, int y){
    int wKey = static_cast<int>(Key::NUL);
    switch(key){
        case GLUT_KEY_LEFT:
        case GLUT_KEY_UP:
        case GLUT_KEY_RIGHT:
        case GLUT_KEY_DOWN:
        case GLUT_KEY_HOME:
        case GLUT_KEY_END:
            wKey = -static_cast<int>(key);
        break;

        default: break;
    }

    int wID = glutGetWindow();
    if(tracker.count(wID) < 1) {
        return;
    }
    tracker[wID]->onWindowKeyPressedEvent(wKey, KeyAction::KEY_UP);
}

void Ixtli::ContextCloseHandler(){
    int wID = glutGetWindow();
    if(tracker.count(wID) < 1) {
        return;
    }
    auto window = tracker[wID];
    window->onWindowClosed();
    tracker.erase(wID);
}

void Ixtli::ContextIdleHandler(){
    trackerMutex.lock();
    for(auto& id : windowUpdateRequest){
        if(tracker.count(id) > 0){
            glutPostWindowRedisplay(id);
        }
    }
    windowUpdateRequest.clear();
    trackerMutex.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void ContextProvider::windowContextCreate(std::shared_ptr<Window> win, int w, int h, const Point& pos, const std::string& title){
    int winID = -1;

    glutInitWindowSize(w, h);
    glutInitWindowPosition(pos.x, pos.y);
    winID = glutCreateWindow(title.c_str());
    //glClearColor(background.r_f(), background.g_f(), background.b_f(), background.a_f());
    glClearColor(0, 0, 0, 0);
    glShadeModel(GL_SMOOTH);
    glutDisplayFunc(ContextDisplayHandler);
    glutReshapeFunc(ContextResizeHandler);
    glutMouseFunc(ContextMouseHandler);
    glutMotionFunc(ContextMouseMotionHandler);
    glutKeyboardFunc(ContextKeyDownHandler);
    glutKeyboardUpFunc(ContextKeyUpHandler);
    glutSpecialFunc(ContextSpecialKeyHandler);
    glutSpecialUpFunc(ContextSpecialKeyUpHandler);
    glutCloseFunc(ContextCloseHandler);
    glutIdleFunc(ContextIdleHandler);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);        

    win->left = pos.x;
    win->top = pos.y;
    win->width = w;
    win->height = h;
    win->title = title;
    dynamic_cast<Context*>(win.get())->contextID = winID;
    tracker[winID] = win;
    win->onCreate();
}

void ContextProvider::windowInvalidate(int wID){
    if(tracker.count(wID) > 0){
        trackerMutex.lock();
        if(windowUpdateRequest.count(wID) == 0){
            windowUpdateRequest.insert(wID);
        }
        trackerMutex.unlock();
    }
}