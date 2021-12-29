#include <stdio.h>
#include "vec3.h"
#include "color.h"

int main(int argc, char *argv[]) {
    printf("P3\n");
    int width = 256;
    int height = 256;
    printf("%d %d 255\n",width,height);
    for (int i = height-1; i >= 0; i--) {
        fprintf(stderr,"Remaining %d\n",i);
        for (int j = 0; j < width; j++) {

            color color =
                {((double) j)/(width-1),  // r
                 ((double) i)/(height-1), // g
                 .25};                    // b
            write_color(color);
        }
    }
    fprintf(stderr,"Done\n");

    return 0;
}
