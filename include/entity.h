#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "obj.h"
#include "vector.h"
#include "collisions.h"

typedef struct Entity_S
{
    int inuse;
    int uid;    /**<unique id of this entity*/
    char name[128];
    Vec3D position;
    Vec3D velocity;
    Vec3D acceleration;
    Vec3D rotation;
    Vec3D scale;
    Vec4D color;
    Cube bounds;
    Obj *objModel;
}Entity;


#endif
