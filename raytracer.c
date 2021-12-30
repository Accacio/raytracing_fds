#include <stdio.h>
#include "vec3.h"
#include "color.h"
#include "ray.h"

float
hit_sphere(point3 center,float radius, ray ray)
{
    vec3 oc = vec3sum(ray.origin, vec3multscalar(center, -1.0));
    float a = vec3normsquared(ray.direction);
    float half_b = vec3dot(oc, ray.direction);
    float c = vec3normsquared(oc) - radius*radius;
    float discriminant = half_b*half_b-a*c;
    if(discriminant<0.)
    {
        return -1.0;
    } else
    {
        return (-half_b-sqrt(discriminant))/a;
    }
}

color
ray_color(ray ray)
{
    point3 center = {0.,0.,-1.};
    float t = hit_sphere(center,0.5,ray);
    if(t > 0)
    {
        vec3 N = vec3normalized(vec3sum(rayat(ray, t),
                                        vec3multscalar(center,-1.)));
        N.x+=1;
        N.y+=1;
        N.z+=1;
        N=vec3multscalar(N,0.5);
        return N;
    }
    vec3 unit_direction = vec3normalized(ray.direction);
    t = 0.5*(unit_direction.y+1.0);
    color white = {1.,1.,1.};
    color blue = {0.5,0.7,1.};
    return vec3sum(vec3multscalar(white, 1.0-t),vec3multscalar(blue, t));
}

int main(int argc, char *argv[]) {
    printf("P3\n");

    /* Image */
    float aspect_ratio = 16./9.;
    int width = 400;
    int height = (int) (width/aspect_ratio);

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

            ray ray = {0};
            ray.direction.x = temp3.x;
            ray.direction.y = temp3.y;
            ray.direction.z = temp3.z;

            color color = ray_color(ray);
            write_color(color);
        }
    }
    fprintf(stderr,"Done\n");

    return 0;
}
