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
              int samples_per_pixel, camera camera,
              hittable_list *world, int max_depth)
{
  for (int i = initial_height; i < final_height; i++)
    {
      /* fprintf (stderr, "Remaining %d\n", image->height -
       * 1 - i); */
      for (int j = 0; j < image->width; j++)
        {
          int cur = (i * image->width + j) * 4;
          color pixel_color = { 0 };
          for (int s = 0; s < samples_per_pixel; s++)
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
              ray ray = camera_get_ray (camera, u, v);
              pixel_color = vec3sum (
                  pixel_color,
                  ray_color (ray, world, max_depth, seed));
            }

          write_accum_rgba_color_to_buffer (
              (uint8_t *) image->data, cur, pixel_color,
              samples_per_pixel);
        }
    }
}

typedef struct _render_context
{
  unsigned int seed;
  image *image;
  int initial_height;
  int final_height;
  int samples_per_pixel;
  camera camera;
  hittable_list *world;
  int max_depth;
} render_context;

render_context
create_render_context (unsigned int seed, image *image,
                       int initial_height, int final_height,
                       int samples, camera camera,
                       hittable_list *world, int max_depth)
{
  render_context ret = { 0 };
  ret.seed = seed;
  ret.image = image;
  ret.initial_height = initial_height;
  ret.final_height = final_height;
  ret.samples_per_pixel = samples;
  ret.camera = camera;
  ret.world = world;
  ret.max_depth = max_depth;
  return ret;
}

void *
routine (void *arg)
{
  render_context *context = (render_context *) arg;
  render_image (&context->seed, context->image,
                context->initial_height,
                context->final_height,
                context->samples_per_pixel, context->camera,
                context->world, context->max_depth);
  return NULL;
}

int
main (int argc, char *argv[])
{

  float aspect_ratio = 16. / 9.;
  int width = 800;
  int height = (int) (width / aspect_ratio);

  unsigned long mask = CWBackPixel | CWEventMask;
  XSetWindowAttributes attributes = { 0 };
  attributes.background_pixel = 0x6495ed;
  attributes.event_mask = ButtonPressMask
                          | ButtonReleaseMask | KeyPressMask
                          | KeyReleaseMask;

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
  XChangeProperty (display, window, wm_window_type, XA_ATOM,
                   32, PropModeReplace,
                   (unsigned char *) &wm_window_type_utility,
                   1);

  XMapWindow (display, window);

  int bytes_per_line = 4 * width;
  uint8_t buffer[bytes_per_line * height];

  image image = { 0 };
  image.data = &buffer[0];
  memset (image.data, 0, 4 * width * height);
  image.width = width;
  image.height = height;

  XImage *ximage
      = XCreateImage (display, DefaultVisual (display, 0),
                      DefaultDepth (display, 0), ZPixmap, 0,
                      (char *) image.data, width, height,
                      32, bytes_per_line);

  /* Image */
  int samples_per_pixel = 1;
  int max_depth = 5;

  int world_size = 5;
  hittable objects[world_size];
  hittable_list world;
  world.data = &objects[0];
  world.size = world_size;

  material material_ground = (material) create_lambertian (
      (vec3) { 0.8, 0.8, 0.0 });
  material material_left
      = (material) create_dielectric (1.5);
  material material_center
      = (material) create_diffuse_light (
          (color) { 1., 1., 1. });

  material material_right = (material) create_metal (
      (vec3) { 0.8, 0.3, 0.2 }, .0);

  world.data[0] = (hittable) create_sphere (
      (point3) { 0., -100.5, -1. }, 100, &material_ground);
  world.data[1] = (hittable) create_sphere (
      (point3) { 0., 200., -1. }, 100, &material_center);
  world.data[3] = (hittable) create_sphere (
      (point3) { -1., 0., -1. }, .5, &material_left);
  world.data[2] = (hittable) create_sphere (
      (point3) { -1., 0., -1. }, -.3, &material_left);
  world.data[4] = (hittable) create_sphere (
      (point3) { 1., 0., -1. }, .5, &material_right);

  /* Camera */
  camera camera = create_default_camera ();

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
          (i + 1) * height / nthreads, samples_per_pixel,
          camera, &world, max_depth);
    }

  int running = 1;
  while (running)
    {

      for (int i = 0; i < nthreads; i++)
        {
          context_list[i] = create_render_context (
              context_list[i].seed, &image,
              i * height / nthreads,
              (i + 1) * height / nthreads,
              samples_per_pixel, camera, &world, max_depth);
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

      XEvent event;
      while (XPending (display))
        {
          XNextEvent (display, &event);
          switch (event.type)
            {
            case ButtonRelease:
              break;
            case KeyPress:
              switch (XLookupKeysym (&event.xkey, 0))
                {
                case XK_Escape:
                  running = 0;
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

  return 0;
}
