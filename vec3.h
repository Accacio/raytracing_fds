#ifndef VEC3_H_
#define VEC3_H_

#include <stdio.h>
#include <math.h>

typedef struct vec3 {
    union {
        struct {
            float x;
            float y;
            float z;
        };
        struct {
            float r;
            float g;
            float b;
        };
    };
} vec3;

typedef vec3 point3;
typedef vec3 color;

vec3
vec3multscalar(vec3 vec, float a)
{
    vec3 ret = {a*vec.x,a*vec.y,a*vec.z};
    return  ret;
}

vec3
vec3sum(vec3 veca, vec3 vecb)
{
    vec3 ret = {veca.x+vecb.x,veca.y+vecb.y,veca.z+vecb.z};
    return  ret;
}


float
vec3normsquared(vec3 vec)
{
    return vec.x*vec.x+vec.y*vec.y+vec.z*vec.z;
}


float
vec3norm(vec3 vec)
{
    float f = sqrt(vec3normsquared(vec));

    return f;
}

vec3
vec3normalized(vec3 vec)
{
    float norm = vec3norm(vec);
    vec3 ret = {vec.x/norm,vec.y/norm,vec.z/norm};

    return ret;
}



void
printvec3(vec3 vec)
{
    printf("%.2f %.2f %.2f\n",vec.x,vec.y,vec.z);
}

#endif // VEC3_H_
