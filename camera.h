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
                 origin.y - ret.vertical.y / 2, -focal_length };
  return ret;
}

ray
camera_get_ray (camera camera, float u, float v)
{
  vec3 temp1 = vec3sum (camera.lower_left_corner,
                        vec3multscalar (camera.horizontal, u));
  vec3 temp2 = vec3sum (temp1, vec3multscalar (camera.vertical, v));
  vec3 temp3 = vec3sum (temp2, vec3multscalar (camera.origin, -1.));
  ray ray = { 0. };
  ray.direction = temp3;
  return ray;
}

#endif // CAMERA_H_
