#ifndef CAMERA_H_
#define CAMERA_H_

#include "vec3.h"
#include "ray.h"

typedef struct _camera
{
  point3 origin;
  point3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;

} camera;

camera
create_camera (point3 look_from, point3 look_at, vec3 vup,
               float vfov, float aspect_ratio)
{
  float theta = degrees_to_radians (vfov);
  float h = tanf (theta / 2);

  float viewport_height = 2.0 * h;
  float viewport_width = aspect_ratio * viewport_height;

  vec3 w = vec3normalized (
      vec3sum (look_from, vec3multscalar (look_at, -1.)));
  vec3 u = vec3normalized (vec3cross (vup, w));
  vec3 v = vec3cross (w, u);

  camera ret = { 0 };
  ret.origin = look_from;
  ret.horizontal = vec3multscalar (u, viewport_width);
  ret.vertical = vec3multscalar (v, viewport_height);

  vec3 horzplusvert = vec3multscalar (
      vec3sum (ret.horizontal, ret.vertical), -.5);
  vec3 origplusw
      = vec3sum (ret.origin, vec3multscalar (w, -1.));

  ret.lower_left_corner = vec3sum (horzplusvert, origplusw);
  return ret;
}

camera
create_default_camera ()
{
  float aspect_ratio = 16. / 9.;
  float viewport_height = 2.0;
  float viewport_width = aspect_ratio * viewport_height;
  float focal_length = 1.0;

  point3 origin = { 0. };

  camera ret = { 0 };
  ret.horizontal = (vec3) { viewport_width, 0., 0. };
  ret.vertical = (vec3) { 0., viewport_height, 0. };

  ret.lower_left_corner
      = (vec3) { origin.x - ret.horizontal.x / 2,
                 origin.y - ret.vertical.y / 2,
                 -focal_length };
  return ret;
}

ray
camera_get_ray (camera camera, float s, float t)
{
  vec3 temp1
      = vec3sum (camera.lower_left_corner,
                 vec3multscalar (camera.horizontal, s));
  vec3 temp2 = vec3sum (
      temp1, vec3multscalar (camera.vertical, t));
  vec3 temp3 = vec3sum (
      temp2, vec3multscalar (camera.origin, -1.));
  ray ray = { 0. };
  ray.origin = camera.origin;
  ray.direction = temp3;
  return ray;
}

#endif // CAMERA_H_
