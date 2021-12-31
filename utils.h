#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>


float
random_float(float min, float max)
{
    return min + (max-min)*rand()/(RAND_MAX+1.0);
}

#endif // UTILS_H_
