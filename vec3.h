#ifndef VEC3_H_
#define VEC3_H_

typedef struct vec3 {
    union {
        struct {
            float x;
            float y;
            float z;
        };
        struct {
            float r;
            float g;
            float b;
        };
    };
} vec3;

typedef vec3 point3;
typedef vec3 color;


#endif // VEC3_H_
