#pragma once

#include <stdlib.h>
#include <stdio.h>
#include "../vector/vector.h"

typedef enum ObjectType {
    OBJECT_TYPE_SPHERE,
    OBJECT_TYPE_CUBE,
    OBJECT_TYPE_CUBOID
} ObjectType;

typedef struct CubeObjectData {
    float size;
} CubeObjectData;

typedef struct CuboidObjectData {
    float width;
    float height;
    float length;
} CuboidObjectData;

typedef struct SphereObjectData {
    float radius;
} SphereObjectData;

typedef struct Object {
    ObjectType type;
    Vector3 position;
    Color color;
    int specular;

    union {
        CubeObjectData cubeData;
        SphereObjectData sphereData;
        CuboidObjectData cuboidData;
    } data;
} Object;

Object Object_NewCube(Vector3 center, Color color, float size, int specular);
void Object_PrintCubeData(CubeObjectData cubeData);
Object Object_NewCuboid(Vector3 center, Color color, float width, float height, float length, int specular);
void Object_PrintCuboidData(CuboidObjectData cuboidData);
Object Object_NewSphere(Vector3 center, Color color, float radius, int specular);
void Object_PrintSphereData(SphereObjectData sphereData);
void Object_Print(const char* label, Object obj);

typedef struct ObjectList {
    Object* objects;
    size_t capacity;
    size_t count;
} ObjectList;

ObjectList ObjectList_New(void);
int ObjectList_Add(ObjectList* list, Object obj);
void ObjectList_Free(ObjectList* list);
Object* ObjectList_Get(ObjectList* list, size_t index);
