#include "Uuid.h"
#include <cstring>
#include <uuid/uuid.h>

using namespace Ixtli;

UUID::UUID(Source source){
    uuid_t uid;
    switch (source){
        case Source::TIME:
            uuid_generate_time(uid);
            break;
        case Source::RANDOM:
            uuid_generate_random(uid);
            break;
        default:
            uuid_generate(uid);
    }      
    uuid_generate(uid);
    Ixtli::UUID_t a;

    size_t readed = 0;
    unsigned char* ptr = uid;

    id = 0;
    while(readed + sizeof(id) <= sizeof(uid)){
        std::memcpy(&a, ptr + readed, sizeof(id));
        readed += sizeof(id);
        id ^= ~a;
    }
    if(readed < sizeof(uid)){
        a = 0;
        std::memcpy(&a, ptr + readed, sizeof(uid) - readed);
        id ^= ~a;
    }
}