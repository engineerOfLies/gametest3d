#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include "vector.h"

typedef struct
{
    float x,y,z;
    float w,h,d;
}Cube;


int cube_cube_intersection(
    Cube a,
    Cube b);

#endif
