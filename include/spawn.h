#ifndef __SPAWN_H__
#define __SPAWN_H__

#include "vector.h"

typdef struct
{
    Vec3D position;       /**<position to spawn at*/
    Vec3D rotation;       /**<rotation of the spawn*/
    char id[3];           /**<unique id for this item in the map*/
    char state[128];      /**<state for the entity to spawn with*/
    char target[128];     /**<target to patrol to / next position in a race*/
    char  classname[128]; /**<name of the entity to spawn*/
    char description[512];
    char tags[512];       /**<space separated tags for dynamic use*/
}Spawn;


#endif
