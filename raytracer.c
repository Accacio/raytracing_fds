#include <stdio.h>
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "hittable.h"

color
ray_color(ray ray,hittable * world,int world_size)
{
    hit_record rec = {0.};
    if(hittable_list_hit(world,world_size, ray,
                         /* TODO use INFINITY */
                         0,100000.,
                         &rec))
    {
        return vec3multscalar(vec3sumscalar(rec.normal,1),0.5);
    }

    vec3 unit_direction = vec3normalized(ray.direction);
    float t = 0.5*(unit_direction.y+1.0);
    return vec3sum(vec3multscalar((color){1.,1.,1.}, 1.0-t),
        vec3multscalar((color) {0.5,0.7,1.}, t));
}

int main(int argc, char *argv[]) {
    printf("P3\n");

    /* Image */
    float aspect_ratio = 16./9.;
    int width = 400;
    int height = (int) (width/aspect_ratio);


    int world_size = 2;
    hittable world[world_size];
    world[0] = (hittable) create_sphere((point3){0.,-100.5,-1.}, 100);
    world[1] = (hittable) create_sphere((point3){0.,0.,-1.}, .5);

    /* Camera */
    float viewport_height = 2.0;
    float viewport_width = aspect_ratio*viewport_height;
    float focal_length = 1.0;

    point3 origin = {0.};
    vec3 horizontal = {viewport_width,0.,0.};
    vec3 vertical = {0.,viewport_height,0.};

    point3 lower_left_corner =
        {
        origin.x-horizontal.x/2,
        origin.y-vertical.y/2,
        -focal_length
    };

    printf("%d %d 255\n",width,height);

    for (int i = height-1; i >= 0; i--) {
        fprintf(stderr,"Remaining %d\n",i);
        for (int j = 0; j < width; j++) {

            float u,v;
            u = ((double) j)/(width-1);
            v = ((double) i)/(height-1);

            vec3 temp1 = vec3sum(lower_left_corner, vec3multscalar(horizontal, u));
            vec3 temp2 = vec3sum(temp1, vec3multscalar(vertical, v));
            vec3 temp3 = vec3sum(temp2, vec3multscalar(origin, -1.));

            ray ray = {0.};
            ray.direction = temp3;

            color color = ray_color(ray,world,world_size);
            write_color(color);
        }
    }
    fprintf(stderr,"Done\n");

    return 0;
}
