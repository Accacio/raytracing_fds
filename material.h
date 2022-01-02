#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "vec3.h"
#include "ray.h"

#include "hittable.h"

typedef enum
{
  LAMBERTIAN,
  METAL,
  DIELECTRIC,
  DIFFUSE_LIGHT,
} materials;

typedef struct lambertian
{
  int type;
  color albedo;
} lambertian;

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

typedef struct _dielectric
{
  int type;
  float ir;
} dielectric;

dielectric
create_dielectric (float ir)
{
  dielectric ret = { 0. };
  ret.type = DIELECTRIC;
  ret.ir = ir;
  return ret;
}

float
reflectance (float cos, float ir)
{
  float r0 = (1. - ir) / (1. + ir);
  r0 *= r0;
  return r0 + (1 - r0) * pow ((1 - cos), 5);
}

int
dieletric_scatter (dielectric dielectric, hit_record *rec, ray _ray,
                   color *attenuation, ray *scattered)
{
  *attenuation = (color) { 1., 1., 1. };
  float refraction_ratio
      = rec->front_face ? (1.0) / dielectric.ir : dielectric.ir;

  vec3 unit_direction = vec3normalized (_ray.direction);
  float cos_theta = fmin (
      vec3dot (vec3multscalar (unit_direction, -1.0), rec->normal), 1.0);
  float sin_theta = sqrt (1.0 - cos_theta * cos_theta);

  int cannot_refract = refraction_ratio * sin_theta > 1.0;
  vec3 direction = { 0. };
  if (cannot_refract
      || reflectance (cos_theta, refraction_ratio) > random_float ())
    {
      direction = vec3reflect (unit_direction, rec->normal);
    }
  else
    {
      direction = vec3refract (unit_direction, rec->normal, refraction_ratio);
    }

  *scattered = (ray) { rec->p, direction };
  return 1;
}

typedef struct _diffuse_light
{
  int type;
  color emit;
} diffuse_light;

diffuse_light
create_diffuse_light (color _color)
{
  diffuse_light ret = { 0. };
  ret.type = DIFFUSE_LIGHT;
  ret.emit = _color;
  return ret;
}

color
diffuse_emit (diffuse_light light)
{
  return light.emit;
}

typedef union _material
{
  int type;
  lambertian lambertian;
  metal metal;
  dielectric dielectric;
  diffuse_light diffuse_light;
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
      break;
    case DIELECTRIC:
      return dieletric_scatter (*(dielectric *) material, rec, _ray,
                                attenuation, scattered);
      break;
    default:
      return 0;
    }
}

color
material_emit (material *material)
{
  switch (material->type)
    {
    case DIFFUSE_LIGHT:
      return diffuse_emit (*(diffuse_light *) material);
      break;
    default:
      return (color) { 0., 0., 0. };
    }
}

#endif // MATERIAL_H_
