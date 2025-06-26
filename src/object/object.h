#pragma once

#include "../vector/vector.h"
#include "../color/color.h"

#ifndef _OBJECT_H_
#define _OBJECT_H_

typedef enum ObjectType {
    OBJECT_TYPE_SPHERE,
    OBJECT_TYPE_CUBE,
    OBJECT_TYPE_CUBOID
} ObjectType;

typedef struct SphereObjectData {
    float radius;
} SphereObjectData;

typedef struct Object {
    ObjectType type;
    Vector3 position;
    Color color;
    int specular;

    union {
        SphereObjectData sphereData;
    } data;
} Object;

typedef struct ObjectList {
    Object* objects;
    int capacity;
    int count;
} ObjectList;

Object object_new_sphere(Vector3 center, Color color, float radius, int specular);

int objectList_init(ObjectList* objectList);
int objectList_add(ObjectList* list, Object obj);
void objectList_free(ObjectList* list);
Object* objectList_get(ObjectList* list, int index);

#endif
