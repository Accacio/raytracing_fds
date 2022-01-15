#ifndef COLOR_H_
#define COLOR_H_

#include <stdio.h>
#include <stdint.h>
#include "vec3.h"

#define MAX(a, b) (a) > (b) ? (a) : (b)

void
write_color (FILE *restrict __stream, color color,
             int samples)
{
  int ir, ig, ib;
  color = vec3multscalar (color, (float) 1 / samples);
  color.r = sqrt (color.r);
  color.g = sqrt (color.g);
  color.b = sqrt (color.b);

  ir = (int) (color.r * 255);
  ig = (int) (color.g * 255);
  ib = (int) (color.b * 255);
  fprintf (__stream, "%d %d %d\n", ir, ig, ib);
}

void
write_color_to_buffer (uint8_t *buffer, int cur,
                       color color, int samples)
{
  int ir, ig, ib;
  color = vec3multscalar (color, (float) 1 / samples);
  color.r = sqrt (color.r);
  color.g = sqrt (color.g);
  color.b = sqrt (color.b);

  buffer[cur++] = (uint8_t) (color.r * 255);
  buffer[cur++] = (uint8_t) (color.g * 255);
  buffer[cur] = (uint8_t) (color.b * 255);
}

void
write_rgba_color_to_buffer (uint8_t *buffer, int cur,
                            color color, int samples)
{
  int ir, ig, ib;
  color = vec3multscalar (color, (float) 1 / samples);
  color.r = sqrt (color.r);
  color.g = sqrt (color.g);
  color.b = sqrt (color.b);

  buffer[cur++] = (uint8_t) (color.b * 255);
  buffer[cur++] = (uint8_t) (color.g * 255);
  buffer[cur++] = (uint8_t) (color.r * 255);
  buffer[cur] = (uint8_t) (255);
}

void
write_accum_rgba_color_to_buffer (uint8_t *buffer, int cur,
                                  color color,
                                  int samples_per_pass,
                                  int samples)
{
  int ir, ig, ib;

  float b = (powf ((float) buffer[cur] / 255, 2) * samples
             + color.b)
            / (samples_per_pass + samples);
  float g
      = (powf ((float) buffer[cur + 1] / 255, 2) * samples
         + color.g)
        / (samples_per_pass + samples);
  float r
      = (powf ((float) buffer[cur + 2] / 255, 2) * samples
         + color.r)
        / (samples_per_pass + samples);

  uint8_t gamma_b = (uint8_t) (sqrt (b) * 255);
  uint8_t gamma_g = (uint8_t) (sqrt (g) * 255);
  uint8_t gamma_r = (uint8_t) (sqrt (r) * 255);

  buffer[cur]
      = samples == 0
            ? (uint8_t) (sqrt (color.b / samples_per_pass)
                         * 255)
            : gamma_b;
  cur++;
  buffer[cur]
      = samples == 0
            ? (uint8_t) (sqrt (color.g / samples_per_pass)
                         * 255)
            : gamma_g;
  cur++;
  buffer[cur]
      = samples == 0
            ? (uint8_t) (sqrt (color.r / samples_per_pass)
                         * 255)
            : gamma_r;
  cur++;
  buffer[cur] = (uint8_t) (255);
}

#endif // COLOR_H_
