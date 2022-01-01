#ifndef VEC3_H_
#define VEC3_H_

#include <stdio.h>
#include <math.h>
#include "utils.h"

typedef struct vec3
{
  union
  {
    struct
    {
      float x;
      float y;
      float z;
    };
    struct
    {
      float r;
      float g;
      float b;
    };
  };
} vec3;

typedef vec3 point3;
typedef vec3 color;

vec3
vec3multscalar (vec3 vec, float a)
{
  return (vec3) { a * vec.x, a * vec.y, a * vec.z };
}

vec3
vec3sumscalar (vec3 vec, float a)
{
  return (vec3) { a + vec.x, a + vec.y, a + vec.z };
}

vec3
vec3sum (vec3 veca, vec3 vecb)
{
  return (vec3) { veca.x + vecb.x, veca.y + vecb.y, veca.z + vecb.z };
}

float
vec3normsquared (vec3 vec)
{
  return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

float
vec3norm (vec3 vec)
{
  return sqrt (vec3normsquared (vec));
}

vec3
vec3normalized (vec3 vec)
{
  float norm = vec3norm (vec);
  return (vec3) { vec.x / norm, vec.y / norm, vec.z / norm };
}

vec3
vec3multelementwise (vec3 vec1, vec3 vec2)
{
  return (vec3) { vec1.x * vec2.x, vec1.y * vec2.y, vec1.z * vec2.z };
}

int
vec3near_zero (vec3 vec)
{
  float s = 1e-8;
  return ((fabsf (vec.x) < s) && (fabsf (vec.y) < s) && fabsf (vec.z) < s);
}

float
vec3dot (vec3 vec1, vec3 vec2)
{
  return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

vec3
vec3reflect (vec3 v, vec3 n)
{
  return vec3sum (v, vec3multscalar (n, -2 * vec3dot (v, n)));
}

vec3
vec3random ()
{
  return (vec3) { random_float (), random_float (), random_float () };
}

vec3
vec3random_min_max (float min, float max)
{
  return (vec3) { random_float_min_max (min, max),
                  random_float_min_max (min, max),
                  random_float_min_max (min, max) };
}

vec3
vec3random_in_unit_sphere ()
{
  while (1)
    {
      vec3 p = vec3random_min_max (-1., 1.);
      if (vec3norm (p) >= 1)
        continue;
      return p;
    }
}

vec3
vec3random_unit_vector ()
{
  return vec3normalized (vec3random_in_unit_sphere ());
}

vec3
vec3random_in_hemisphere (vec3 normal)
{
  vec3 in_unit_sphere = vec3random_in_unit_sphere ();
  if (vec3dot (in_unit_sphere, normal) > 0.0)
    return in_unit_sphere;
  else
    return vec3multscalar (in_unit_sphere, -1.);
}

void
printvec3 (FILE *restrict __stream, vec3 vec)
{
  fprintf (__stream, "%.2f %.2f %.2f\n", vec.x, vec.y, vec.z);
}

#endif // VEC3_H_
