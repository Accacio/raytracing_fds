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
    return  (vec3) {a*vec.x,a*vec.y,a*vec.z};
}

vec3
vec3sumscalar(vec3 vec, float a)
{
    return  (vec3) {a+vec.x,a+vec.y,a+vec.z};
}

vec3
vec3sum(vec3 veca, vec3 vecb)
{
    return  (vec3) {veca.x+vecb.x,veca.y+vecb.y,veca.z+vecb.z};
}

float
vec3normsquared(vec3 vec)
{
    return vec.x*vec.x+vec.y*vec.y+vec.z*vec.z;
}


float
vec3norm(vec3 vec)
{
    return sqrt(vec3normsquared(vec));
}

vec3
vec3normalized(vec3 vec)
{
    float norm = vec3norm(vec);
    return (vec3) {vec.x/norm,vec.y/norm,vec.z/norm};
}

float
vec3dot(vec3 vec1, vec3 vec2)
{
    return vec1.x*vec2.x+vec1.y*vec2.y+vec1.z*vec2.z;
}


void
printvec3(FILE * restrict __stream,vec3 vec)
{
    fprintf(__stream,"%.2f %.2f %.2f\n",vec.x,vec.y,vec.z);
}

#endif // VEC3_H_
