#ifndef IXTLI_VIEW_CONTEXT_H
#define IXTLI_VIEW_CONTEXT_H

#include <iostream>

namespace Ixtli{

class Toast;
class ContextProvider;

class Context{
    private:
        int contextID;

    public:
        Context() : contextID(-1){};
        virtual ~Context() {}

        inline int getID() const
            { return contextID; }
        
        virtual void notify(Toast* toast) ;


        friend class ContextProvider;
};

};


#endif //IXTLI_VIEW_CONTEXT_H