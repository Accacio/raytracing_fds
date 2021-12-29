#ifndef COLOR_H_
#define COLOR_H_

#include <stdio.h>
#include "vec3.h"

void
write_color(color color)
{
    int ir,ig,ib;
    ir = (int) (color.r*255);
    ig = (int) (color.g*255);
    ib = (int) (color.b*255);
    printf("%d %d %d\n",ir,ig,ib);
}

#endif // COLOR_H_
