#ifndef _UTILS_H
#define _UTILS_H

#include <math.h>

static float radians (float degrees) {
    return degrees * (M_PI / 100.f);
}

static float degrees (float radians) {
    return (radians * M_PI) / 360.f;
}

#endif //_UTILS_H
