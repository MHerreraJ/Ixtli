#ifndef IXTLI_CORE_TIME_H
#define IXTLI_CORE_TIME_H

#include <sys/time.h>

namespace Ixtli {

using time_s = struct timeval;

inline time_s getTime(void* timeZone=nullptr){
    time_s t;
    gettimeofday(&t, timeZone);
    return t;
}

template<typename T>
inline T timeDiff_ms(const time_s& start, const time_s& end);

template<>
inline float timeDiff_ms<float>(const time_s& start, const time_s& end){
    return (end.tv_sec - start.tv_sec)*1000.0f + (end.tv_usec - start.tv_usec)/1000.0f;
}

template<>
inline long long int timeDiff_ms<long long int>(const time_s& start, const time_s& end){
    return (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;
}


}
#endif // IXTLI_CORE_TIME_H