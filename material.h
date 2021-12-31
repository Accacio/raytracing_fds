#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "vec3.h"
#include "ray.h"

#include "hittable.h"

typedef struct lambertian
{
    int type;
    color albedo;
} lambertian ;


#define LAMBERTIAN 1

lambertian
create_lambertian(color albedo)
{
    lambertian ret = {0.};
    ret.type = LAMBERTIAN;
    ret.albedo = albedo;
    return ret;
}

int
lambertian_scatter(lambertian lambertian,
                   hit_record * rec,
                       ray _ray,
                       color * attenuation,
                       ray * scattered)
{
    vec3 scatter_direction = vec3sum(rec->normal, vec3random_unit_vector());
    if(vec3near_zero(scatter_direction))
        scatter_direction = rec->normal;

    ray newray = {0.};
    newray.origin = rec->p;
    newray.direction = scatter_direction;

    *scattered = newray;
    *attenuation = lambertian.albedo;
    return 1;
}

typedef union _material
{
    int type;
    lambertian lambertian;
} material;

#endif // MATERIAL_H_
