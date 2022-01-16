#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "hittable.h"
#include "utils.h"
#include "camera.h"
#include "material.h"
#include "image.h"

color
ray_color (ray _ray, hittable_list *world, int depth,
           unsigned int *seed)
{
  hit_record rec = { 0. };
  if (depth <= 0)
    return (color) { 0., 0., 0. };

  if (hittable_list_hit (world, _ray, 0.0001, INFINITY,
                         &rec))
    {

      color attenuation;
      ray scattered;
      color emitted = material_emit (rec.material);
      if (!material_scatter (rec.material, &rec, _ray,
                             &attenuation, &scattered,
                             seed))
        {
          return emitted;
        }
      else
        {
          return vec3sum (emitted,
                          vec3multelementwise (
                              attenuation,
                              ray_color (scattered, world,
                                         depth - 1, seed)));
        }
      /* point3 temp1 = vec3sum(rec.p, rec.normal); */
      /* point3 target =vec3sum(temp1,
       * vec3random_in_unit_sphere()); // hacky
       */
      /* point3 target =vec3sum(rec.p,
       * vec3random_in_hemisphere(rec.normal));
       * // alternate diffuse */
      return (color) { 0., 0., 0. };
    }

  vec3 unit_direction = vec3normalized (_ray.direction);
  float t = 0.5 * (unit_direction.y + 1.0);
  /* return vec3sum (vec3multscalar ((color) { 1., 1., 1.
   * }, 1.0 - t), */
  /*                 vec3multscalar ((color) { 0.5, 0.7, 1.
   * }, t)); */
  return (color) { 0., 0., 0. };
}

void
render_image (unsigned int *seed, image *image,
              int initial_height, int final_height,
              int samples_per_pass, int samples_per_pixel,
              camera camera, hittable_list *world,
              int max_depth)
{
  for (int i = initial_height; i < final_height; i++)
    {
      /* fprintf (stderr, "Remaining %d\n", image->height -
       * 1 - i); */
      for (int j = 0; j < image->width; j++)
        {
          int cur = (i * image->width + j) * 4;
          color pixel_color = { 0 };
          for (int s = 0; s < samples_per_pass; s++)
            {

              float u
                  = ((double) j
                     + random_float_min_max (seed, 0., 2.))
                    / (image->width - 1);
              float v = 1.
                        - ((double) i
                           + random_float_min_max (seed, 0.,
                                                   2.))
                              / (image->height - 1);
              ray ray = camera_get_ray (camera, u, v, seed);
              pixel_color = vec3sum (
                  pixel_color,
                  ray_color (ray, world, max_depth, seed));
            }

          write_accum_rgba_color_to_buffer (
              (uint8_t *) image->data, cur, pixel_color,
              samples_per_pass, samples_per_pixel);
        }
    }
}

typedef struct _render_context
{
  unsigned int seed;
  image *image;
  int initial_height;
  int final_height;
  int samples_per_pass;
  int samples_per_pixel;
  camera camera;
  hittable_list *world;
  int max_depth;
} render_context;

render_context
create_render_context (unsigned int seed, image *image,
                       int initial_height, int final_height,
                       int samples_per_pass,
                       int samples_per_pixel, camera camera,
                       hittable_list *world, int max_depth)
{
  render_context ret = { 0 };
  ret.seed = seed;
  ret.image = image;
  ret.initial_height = initial_height;
  ret.final_height = final_height;
  ret.samples_per_pass = samples_per_pass;
  ret.samples_per_pixel = samples_per_pixel;
  ret.camera = camera;
  ret.world = world;
  ret.max_depth = max_depth;
  return ret;
}

void *
routine (void *arg)
{
  render_context *context = (render_context *) arg;
  render_image (
      &context->seed, context->image,
      context->initial_height, context->final_height,
      context->samples_per_pass, context->samples_per_pixel,
      context->camera, context->world, context->max_depth);
  return NULL;
}

hittable_list *
create_random_world (unsigned int *seed)
{
  int world_size = 150;
  hittable_list *world = malloc (sizeof (hittable_list));

  initialize_hittable_list (world, world_size);

  material *material_ground = malloc (sizeof (material));
  *material_ground = (material) create_lambertian (
      (vec3) { 0.5, 0.5, 0.0 });

  hittable sph = (hittable) create_sphere (
      (point3) { 0., -1000.5, 0. }, 1000, material_ground);
  hittable_list_add (world, &sph);

  material *material_sun = malloc (sizeof (material));
  *material_sun = (material) create_diffuse_light (
      (color) { 1., 1., 1. });

  material *default_material = malloc (sizeof (material));
  *default_material = (material) create_lambertian (
      (color) { 0., .7, 0. });

  hittable sph2 = (hittable) create_sphere (
      (point3) { 0., 20000., -1. }, 10000, material_sun);

  hittable_list_add (world, &sph2);

  for (int i = -5; i < 5; i++)
    {
      for (int j = -5; j < 5; j++)
        {
          float choose_material = random_float (seed);
          float radius
              = random_float_min_max (seed, .2, .6);
          point3 center = (point3) {
            i * (1.2 + 0.5) + 0.9 * random_float (seed),
            radius - .5,
            j * (1.2 + 0.5) + 0.9 * random_float (seed)
          };
          if (vec3norm (
                  vec3sum (center, (point3) { 4, 0.2, 0 }))
              > 0.9)
            {
              if (choose_material < 0.5)
                {
                  material *default_material
                      = malloc (sizeof (material));
                  *default_material
                      = (material) create_lambertian (
                          (color) { random_float (seed),
                                    random_float (seed),
                                    random_float (seed) });

                  sph = (hittable) create_sphere (
                      center, radius, default_material);
                  hittable_list_add (world, &sph);
                }
              else if (choose_material < 0.9)
                {
                  material *default_material
                      = malloc (sizeof (material));
                  *default_material
                      = (material) create_metal (
                          (color) { random_float (seed),
                                    random_float (seed),
                                    random_float (seed) },
                          random_float_min_max (seed, 0.,
                                                0.5));

                  sph = (hittable) create_sphere (
                      center, radius, default_material);
                  hittable_list_add (world, &sph);
                }
              else
                {
                  material *default_material
                      = malloc (sizeof (material));
                  *default_material
                      = (material) create_dielectric (1.5);
                  sph = (hittable) create_sphere (
                      center, radius, default_material);
                  hittable_list_add (world, &sph);
                }
            }
        }
    }

  return world;
}

hittable_list *
create_default_world ()
{
  hittable_list *world = malloc (sizeof (hittable_list));
  initialize_hittable_list (world, 5);

  material *material_ground = malloc (sizeof (material));
  *material_ground = (material) create_lambertian (
      (vec3) { 0.8, 0.8, 0.0 });
  material *material_left = malloc (sizeof (material));
  *material_left = (material) create_dielectric (1.5);

  material *material_left1 = malloc (sizeof (material));
  *material_left1 = (material) create_dielectric (1.5);

  material *material_center = malloc (sizeof (material));
  *material_center = (material) create_diffuse_light (
      (color) { 1., 1., 1. });

  material *material_right = malloc (sizeof (material));
  *material_right = (material) create_metal (
      (vec3) { 0.8, 0.3, 0.2 }, .0);

  hittable sph = (hittable) create_sphere (
      (point3) { 0., -100.5, -1. }, 100, material_ground);
  hittable_list_add (world, &sph);

  sph = (hittable) create_sphere (
      (point3) { 0., 200., -1. }, 100, material_center);
  hittable_list_add (world, &sph);

  sph = (hittable) create_sphere ((point3) { -1., 0., -1. },
                                  .5, material_left);
  hittable_list_add (world, &sph);

  sph = (hittable) create_sphere ((point3) { -1., 0., -1. },
                                  -.3, material_left1);
  hittable_list_add (world, &sph);

  sph = (hittable) create_sphere ((point3) { 1., 0., -1. },
                                  .5, material_right);
  hittable_list_add (world, &sph);

  return world;
}

void
destroy_world (hittable_list *world)
{
  // delete material
  for (int i = 0; i < world->size; i++)
    {
      material *mat = (world->data[i]).sphere.material;
      free (mat);
    }
  // delete object
  free (world->data);
  // delete world
  free (world);
}

int
main (int argc, char *argv[])
{

  float aspect_ratio = 3. / 2.;
  int width = 1200;
  int height = (int) (width / aspect_ratio);

  unsigned long mask = CWBackPixel | CWEventMask;
  XSetWindowAttributes attributes = { 0 };
  attributes.background_pixel = 0x6495ed;
  attributes.event_mask
      = ButtonPressMask | ButtonReleaseMask | KeyPressMask
        | KeyReleaseMask | ResizeRedirectMask;

  Display *display = XOpenDisplay (NULL);
  Window window = XCreateWindow (
      display, DefaultRootWindow (display), 0, 0, width,
      height, 0, CopyFromParent, InputOutput,
      CopyFromParent, mask, &attributes);

  Atom wm_delete_window
      = XInternAtom (display, "WM_DELETE_WINDOW", 0);
  XSetWMProtocols (display, window, &wm_delete_window, 1);

  Atom wm_window_type_utility = XInternAtom (
      display, "_NET_WM_WINDOW_TYPE_UTILITY", 0);

  Atom wm_window_type
      = XInternAtom (display, "_NET_WM_WINDOW_TYPE", 0);
  XChangeProperty (
      display, window, wm_window_type, XA_ATOM, 32,
      PropModeReplace,
      (unsigned char *) &wm_window_type_utility, 1);

  XMapWindow (display, window);

  int bytes_per_line = 4 * width;
  int buffer_size = bytes_per_line * height;
  uint8_t buffer[buffer_size];

  image image = { 0 };
  image.data = &buffer[0];
  memset (image.data, 0, buffer_size);
  image.width = width;
  image.height = height;

  XImage *ximage
      = XCreateImage (display, DefaultVisual (display, 0),
                      DefaultDepth (display, 0), ZPixmap, 0,
                      (char *) image.data, width, height,
                      32, bytes_per_line);

  /* Image */
  int samples_per_pixel = 0;
  int samples_per_pass = 5;
  int max_depth = 20;

  /* hittable_list *world = create_default_world (); */

  unsigned int seed = 1;
  hittable_list *world = create_random_world (&seed);

  point3 look_from = (vec3) { 3, 5, 10 };
  point3 look_at = (vec3) { -1, 0, -1 };
  vec3 vup = (vec3) { 0, 1, 0 };

  float dist_to_focus = vec3norm (
      vec3sum (look_from, vec3multscalar (look_at, -1)));
  float aperture = .1;

  /* Camera */
  camera camera = create_camera (look_from, look_at, vup,
                                 20, aspect_ratio, aperture,
                                 dist_to_focus);

  int nthreads = 20;
  pthread_t pool[nthreads];
  render_context context_list[nthreads];

  /* Image Head */
  /* printf ("P6\n"); */
  /* printf ("%d %d 255\n", width, height); */
  /* fwrite (&buffer[0], sizeof (buffer), 1, stdout); */
  /* fprintf (stderr, "Done\n"); */

  for (int i = 0; i < nthreads; i++)
    {
      context_list[i] = create_render_context (
          i, &image, i * height / nthreads,
          (i + 1) * height / nthreads, samples_per_pass,
          samples_per_pixel, camera, world, max_depth);
    }

  int running = 1;
  while (running)
    {

      for (int i = 0; i < nthreads; i++)
        {
          context_list[i] = create_render_context (
              context_list[i].seed, &image,
              i * height / nthreads,
              (i + 1) * height / nthreads, samples_per_pass,
              samples_per_pixel, camera, world, max_depth);
        }

      for (int i = 0; i < nthreads; i++)
        {
          pthread_create (&pool[i], NULL, routine,
                          &context_list[i]);
        }

      for (int i = 0; i < nthreads; i++)
        {
          pthread_join (pool[i], NULL);
        }

      samples_per_pixel += samples_per_pass;
      printf ("%d\n", samples_per_pixel);

      XEvent event;
      while (XPending (display))
        {
          XNextEvent (display, &event);
          switch (event.type)
            {
            case ResizeRequest:
              XResizeWindow (display, window, width,
                             height);
              break;
            case ButtonRelease:
              break;
            case KeyPress:
              switch (XLookupKeysym (&event.xkey, 0))
                {
                case XK_Escape:
                  running = 0;
                  break;
                case XK_c:
                  memset (image.data, 0, buffer_size);
                  samples_per_pixel = 0;
                  break;
                case XK_h:
                  camera.origin.x -= .1;
                  memset (image.data, 0, buffer_size);
                  samples_per_pixel = 0;
                  break;
                case XK_l:
                  camera.origin.x += .1;
                  memset (image.data, 0, buffer_size);
                  samples_per_pixel = 0;
                  break;
                case XK_j:
                  camera.origin.z -= .1;
                  memset (image.data, 0, buffer_size);
                  samples_per_pixel = 0;
                  break;
                case XK_k:
                  camera.origin.z += .1;
                  memset (image.data, 0, buffer_size);
                  samples_per_pixel = 0;
                  break;
                default:
                  break;
                }
            case ClientMessage:
              if (event.xclient.data.l[0]
                  == wm_delete_window)
                running = 0;
              break;
            default:
              break;
            }
        }

      XPutImage (display, window, DefaultGC (display, 0),
                 ximage, 0, 0, 0, 0, width, height);
    }

  destroy_world (world);
  return 0;
}
