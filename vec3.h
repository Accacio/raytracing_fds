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
vec3refract (vec3 uv, vec3 n, float etai_over_etat)
{
  float cos_theta = fmin (vec3dot (vec3multscalar (uv, -1.0), n), 1.0);
  vec3 cos_theta_n = vec3multscalar (n, cos_theta);
  vec3 r_out_perp = vec3multscalar (vec3sum (uv, cos_theta_n), etai_over_etat);
  vec3 r_out_parallel = vec3multscalar (
      n, -sqrt (fabsf ((float) 1. - vec3normsquared (r_out_perp))));
  return vec3sum (r_out_perp, r_out_parallel);
}

vec3
vec3random (unsigned int *seed)
{
  return (vec3) { random_float (seed), random_float (seed),
                  random_float (seed) };
}

vec3
vec3random_min_max (unsigned int *seed, float min, float max)
{
  return (vec3) { random_float_min_max (seed, min, max),
                  random_float_min_max (seed, min, max),
                  random_float_min_max (seed, min, max) };
}

vec3
vec3random_in_unit_sphere (unsigned int *seed)
{
  while (1)
    {
      vec3 p = vec3random_min_max (seed, -1., 1.);
      if (vec3norm (p) >= 1)
        continue;
      return p;
    }
}

vec3
vec3random_unit_vector (unsigned int *seed)
{
  return vec3normalized (vec3random_in_unit_sphere (seed));
}

vec3
vec3random_in_hemisphere (vec3 normal, unsigned int *seed)
{
  vec3 in_unit_sphere = vec3random_in_unit_sphere (seed);
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
