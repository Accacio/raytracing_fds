#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "vec3.h"
#include "ray.h"

#include "hittable.h"

typedef struct lambertian
{
  int type;
  color albedo;
} lambertian;

#define LAMBERTIAN 1

lambertian
create_lambertian (color albedo)
{
  lambertian ret = { 0. };
  ret.type = LAMBERTIAN;
  ret.albedo = albedo;
  return ret;
}

int
lambertian_scatter (lambertian lambertian, hit_record *rec, ray _ray,
                    color *attenuation, ray *scattered)
{
  vec3 scatter_direction = vec3sum (rec->normal, vec3random_unit_vector ());
  if (vec3near_zero (scatter_direction))
    scatter_direction = rec->normal;

  ray newray = { 0. };
  newray.origin = rec->p;
  newray.direction = scatter_direction;

  *scattered = newray;
  *attenuation = lambertian.albedo;
  return 1;
}

typedef struct _metal
{
  int type;
  color albedo;
  float fuzz;
} metal;

#define METAL 2

metal
create_metal (color albedo, float fuzz)
{
  metal ret = { 0. };
  ret.type = METAL;
  ret.albedo = albedo;
  ret.fuzz = fuzz < 1.0 ? fuzz : 1.0;
  return ret;
}

int
metal_scatter (metal metal, hit_record *rec, ray _ray, color *attenuation,
               ray *scattered)
{
  vec3 reflected = vec3reflect (vec3normalized (_ray.direction), rec->normal);

  ray newray = { 0. };
  newray.origin = rec->p;
  newray.direction = vec3sum (
      reflected, vec3multscalar (vec3random_in_unit_sphere (), metal.fuzz));

  *scattered = newray;
  *attenuation = metal.albedo;
  return 1;
}

typedef union _material
{
  int type;
  lambertian lambertian;
} material;

int
material_scatter (material *material, hit_record *rec, ray _ray,
                  color *attenuation, ray *scattered)
{
  switch (material->type)
    {
    case LAMBERTIAN:
      return lambertian_scatter (*(lambertian *) material, rec, _ray,
                                 attenuation, scattered);
      break;
    case METAL:
      return metal_scatter (*(metal *) material, rec, _ray, attenuation,
                            scattered);
    default:
      return 0;
    }
}

#endif // MATERIAL_H_
