#ifndef RAY_H_
#define RAY_H_

#include "vec3.h"

typedef struct ray
{
  point3 origin;
  vec3 direction;

} ray;

point3
rayat (ray ray, float t)
{
  point3 ret = vec3sum (ray.origin, vec3multscalar (ray.direction, t));
  return ret;
}

#endif // RAY_H_
