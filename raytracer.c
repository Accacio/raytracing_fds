#include <stdio.h>
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "hittable.h"
#include "utils.h"
#include "camera.h"

color
ray_color(ray ray,hittable * world,int world_size)
{
    hit_record rec = {0.};
    if(hittable_list_hit(world,world_size, ray,
                         0,INFINITY,
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
    int samples_per_pixel = 100;

    int world_size = 2;
    hittable world[world_size];
    world[0] = (hittable) create_sphere((point3){0.,-100.5,-1.}, 100);
    world[1] = (hittable) create_sphere((point3){0.,0.,-1.}, .5);

    /* Camera */
    camera camera = create_default_camera();
    printf("%d %d 255\n",width,height);

    for (int i = height-1; i >= 0; i--) {
        fprintf(stderr,"Remaining %d\n",i);
        for (int j = 0; j < width; j++) {

            color pixel_color = {0};
            for (int s = 0; s < samples_per_pixel; s++) {

                float u = ((double) j+random_float(0.,2.))/(width-1);
                float v = ((double) i+random_float(0.,2.))/(height-1);
                ray ray = camera_get_ray(camera,u,v);
                pixel_color = vec3sum(pixel_color, ray_color(ray,world,world_size));
            }
            write_color(stdout,pixel_color,samples_per_pixel);
        }
    }
    fprintf(stderr,"Done\n");

    return 0;
}
