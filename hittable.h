#ifndef HITTABLE_H_
#define HITTABLE_H_
#include "vec3.h"
#include "ray.h"

typedef union _material material;

typedef struct _hit_record
{
  point3 p;
  vec3 normal;
  float t;
  int front_face;
  material *material;
} hit_record;

void
hit_set_face_normal (hit_record *rec, ray ray, vec3 outward_normal)
{
  rec->front_face = vec3dot (ray.direction, outward_normal) < 0;
  rec->normal = rec->front_face ? outward_normal
                                : vec3multscalar (outward_normal, -1.);
}

typedef struct _sphere
{
  int type;
  point3 center;
  float radius;
  material *material;
} sphere;

int
hit_sphere (sphere sphere, ray ray, float t_min, float t_max, hit_record *rec)
{
  vec3 oc = vec3sum (ray.origin, vec3multscalar (sphere.center, -1.0));
  float a = vec3normsquared (ray.direction);
  float half_b = vec3dot (oc, ray.direction);
  float c = vec3normsquared (oc) - sphere.radius * sphere.radius;
  float discriminant = half_b * half_b - a * c;
  if (discriminant < 0.)
    return 0;

  float sqrtd = sqrt (discriminant);
  float root = (-half_b - sqrtd) / a;
  if (root < t_min || t_max < root)
    {
      root = (-half_b + sqrtd) / a;
      if (root < t_min || t_max < root)
        return 0;
    }

  rec->t = root;
  rec->p = rayat (ray, rec->t);
  rec->material = sphere.material;
  vec3 outward_normal
      = vec3normalized (vec3sum (rec->p, vec3multscalar (sphere.center, -1.)));
  hit_set_face_normal (rec, ray, outward_normal);
  return 1;
}

#define SPHERE 1

sphere
create_sphere (point3 center, float radius, material *material)
{
  sphere ret = { 0. };
  ret.type = SPHERE;
  ret.center = center;
  ret.radius = radius;
  ret.material = material;
  return ret;
}

typedef union hittable
{
  int type;
  sphere sphere;
} hittable;

int
hittable_hit (hittable hittable, ray ray, float t_min, float t_max,
              hit_record *rec)
{
  switch (hittable.type)
    {
    case SPHERE:
      return hit_sphere (*(sphere *) &hittable, ray, t_min, t_max, rec);
      break;
    default:
      return 0;
    }
}

int
hittable_list_hit (hittable *hit_list, int hit_list_size, ray ray, float t_min,
                   float t_max, hit_record *rec)
{
  int hit_anything = 0;
  float closest_so_far = t_max;
  hit_record temp_rec;
  for (int i = 0; i < hit_list_size; i++)
    {
      hittable current = hit_list[i];
      if (hittable_hit (current, ray, t_min, closest_so_far, &temp_rec))
        {
          hit_anything = 1;
          closest_so_far = temp_rec.t;
          *rec = temp_rec;
        }
    }
  return hit_anything;
}

#endif // HITTABLE_H_
