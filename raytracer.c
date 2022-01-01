#include <stdio.h>
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "hittable.h"
#include "utils.h"
#include "camera.h"
#include "material.h"

color
ray_color (ray _ray, hittable *world, int world_size, int depth)
{
  hit_record rec = { 0. };
  if (depth <= 0)
    return (color) { 0., 0., 0. };

  if (hittable_list_hit (world, world_size, _ray, 0.0001, INFINITY, &rec))
    {

      color attenuation;
      ray scattered;
      if (material_scatter (rec.material, &rec, _ray, &attenuation,
                            &scattered))
        return vec3multelementwise (
            attenuation, ray_color (scattered, world, world_size, depth - 1));
      /* point3 temp1 = vec3sum(rec.p, rec.normal); */
      /* point3 target =vec3sum(temp1, vec3random_in_unit_sphere()); // hacky
       */
      /* point3 target =vec3sum(temp1, vec3random_unit_vector()); // Lambertian
       * sphere */
      /* point3 target =vec3sum(rec.p, vec3random_in_hemisphere(rec.normal));
       * // alternate diffuse */
      return (color) { 0., 0., 0. };
    }

  vec3 unit_direction = vec3normalized (_ray.direction);
  float t = 0.5 * (unit_direction.y + 1.0);
  return vec3sum (vec3multscalar ((color) { 1., 1., 1. }, 1.0 - t),
                  vec3multscalar ((color) { 0.5, 0.7, 1. }, t));
}

int
main (int argc, char *argv[])
{
  printf ("P3\n");

  /* Image */
  float aspect_ratio = 16. / 9.;
  int width = 400;
  int height = (int) (width / aspect_ratio);
  int samples_per_pixel = 100;
  int max_depth = 20;

  int world_size = 4;
  hittable world[world_size];

  lambertian material_ground = create_lambertian ((vec3) { 0.8, 0.8, 0.0 });
  lambertian material_center = create_lambertian ((vec3) { 0.7, 0.3, 0.3 });

  metal material_left = create_metal ((vec3) { 0.8, 0.8, 0.8 }, 0.3);
  metal material_right = create_metal ((vec3) { 0.8, 0.6, 0.2 }, 1.0);

  world[0] = (hittable) create_sphere ((point3) { 0., -100.5, -1. }, 100,
                                       (material *) &material_ground);
  world[1] = (hittable) create_sphere ((point3) { 0., 0., -1. }, .5,
                                       (material *) &material_center);
  world[2] = (hittable) create_sphere ((point3) { -1., 0., -1. }, .5,
                                       (material *) &material_left);
  world[3] = (hittable) create_sphere ((point3) { 1., 0., -1. }, .5,
                                       (material *) &material_right);

  /* Camera */
  camera camera = create_default_camera ();
  printf ("%d %d 255\n", width, height);

  for (int i = height - 1; i >= 0; i--)
    {
      fprintf (stderr, "Remaining %d\n", i);
      for (int j = 0; j < width; j++)
        {

          color pixel_color = { 0 };
          for (int s = 0; s < samples_per_pixel; s++)
            {

              float u
                  = ((double) j + random_float_min_max (0., 2.)) / (width - 1);
              float v = ((double) i + random_float_min_max (0., 2.))
                        / (height - 1);
              ray ray = camera_get_ray (camera, u, v);
              pixel_color = vec3sum (
                  pixel_color, ray_color (ray, world, world_size, max_depth));
            }
          write_color (stdout, pixel_color, samples_per_pixel);
        }
    }
  fprintf (stderr, "Done\n");

  return 0;
}
