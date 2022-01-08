#include <stdio.h>
#include <stdint.h>
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
      color emitted = material_emit (rec.material);
      if (!material_scatter (rec.material, &rec, _ray, &attenuation,
                             &scattered))
        {
          return emitted;
        }
      else
        {
          return vec3sum (emitted,
                          vec3multelementwise (
                              attenuation, ray_color (scattered, world,
                                                      world_size, depth - 1)));
        }
      /* point3 temp1 = vec3sum(rec.p, rec.normal); */
      /* point3 target =vec3sum(temp1, vec3random_in_unit_sphere()); // hacky
       */
      /* point3 target =vec3sum(rec.p, vec3random_in_hemisphere(rec.normal));
       * // alternate diffuse */
      return (color) { 0., 0., 0. };
    }

  vec3 unit_direction = vec3normalized (_ray.direction);
  float t = 0.5 * (unit_direction.y + 1.0);
  /* return vec3sum (vec3multscalar ((color) { 1., 1., 1. }, 1.0 - t), */
  /*                 vec3multscalar ((color) { 0.5, 0.7, 1. }, t)); */
  return (color) { 0., 0., 0. };
}

int
main (int argc, char *argv[])
{

  /* Image */
  float aspect_ratio = 16. / 9.;
  int width = 400;
  int height = (int) (width / aspect_ratio);
  int samples_per_pixel = 100;
  int max_depth = 5;

  int world_size = 5;
  hittable world[world_size];

  material material_ground
      = (material) create_lambertian ((vec3) { 0.8, 0.8, 0.0 });
  material material_left = (material) create_dielectric (1.5);
  material material_center
      = (material) create_diffuse_light ((color) { 1., 1., 1. });

  material material_right
      = (material) create_metal ((vec3) { 0.8, 0.3, 0.2 }, .0);

  world[0] = (hittable) create_sphere ((point3) { 0., -100.5, -1. }, 100,
                                       &material_ground);
  world[1] = (hittable) create_sphere ((point3) { 0., 200., -1. }, 100,
                                       &material_center);
  world[3] = (hittable) create_sphere ((point3) { -1., 0., -1. }, .5,
                                       &material_left);
  world[2] = (hittable) create_sphere ((point3) { -1., 0., -1. }, -.3,
                                       &material_left);
  world[4] = (hittable) create_sphere ((point3) { 1., 0., -1. }, .5,
                                       &material_right);

  /* Camera */
  camera camera = create_default_camera ();

  /* Image Head */
  printf ("P6\n");
  printf ("%d %d 255\n", width, height);

  uint8_t buffer[3 * width * height];
  for (int i = height - 1; i >= 0; i--)
    {
      fprintf (stderr, "Remaining %d\n", i);

      for (int j = 0; j < width; j++)
        {
          int cur = (((height - 1) - i) * width + j) * 3;
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

          write_color_to_buffer ((uint8_t *) &buffer, cur, pixel_color,
                                 samples_per_pixel);
        }
    }
  fwrite (&buffer[0], sizeof (buffer), 1, stdout);
  fprintf (stderr, "Done\n");

  return 0;
}
