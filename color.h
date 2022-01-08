#ifndef COLOR_H_
#define COLOR_H_

#include <stdio.h>
#include <stdint.h>
#include "vec3.h"

void
write_color (FILE *restrict __stream, color color, int samples)
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
write_color_to_buffer (uint8_t *buffer, int cur, color color, int samples)
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

#endif // COLOR_H_
