#pragma once

#include <stdlib.h>
#include <stdio.h>
#include "../vector/vector.h"

typedef enum LightType {
    LIGHT_TYPE_AMBIENT,
    LIGHT_TYPE_POINT,
    LIGHT_TYPE_DIRECTIONAL
} LightType;

typedef struct PointLightData {
    Vector3 position;
} PointLightData;

typedef struct DirectionalLightData {
    Vector3 direction;
} DirectionalLightData;

typedef struct Light {
    LightType type;
    float intensity;

    union {
        PointLightData pointData;
        DirectionalLightData directionalData;
    } data;
} Light;

Light Light_NewAmbient(float intensity);
Light Light_NewPoint(Vector3 position, float intensity);
Light Light_NewDirectional(Vector3 direction, float intensity);

typedef struct LightList {
    Light* lights;
    size_t capacity; // Total allocated space
    size_t count;    // Number of active lights
} LightList;

LightList LightList_New(void);
int LightList_Add(LightList* list, Light light);
void LightList_Free(LightList* list);
Light* LightList_Get(LightList* list, size_t index);