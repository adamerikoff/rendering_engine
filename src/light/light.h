#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h> // For malloc, realloc, free
#include <string.h> // For memcpy


#include "../vector/vector.h"


#ifndef _LIGHT_H_
#define _LIGHT_H_

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

Light light_new_ambient(float intensity);
Light light_new_point(Vector3 position, float intensity);
Light light_new_directional(Vector3 direction, float intensity);

typedef struct LightList {
    Light* lights;
    size_t capacity; // Total allocated space
    size_t count;    // Number of active lights
} LightList;

int lightList_init(LightList* lightLits);
int lightList_add(LightList* lightLits, Light light);
void lightList_free(LightList* lightLits);
Light* lightList_get(LightList* lightLits, size_t index);

#endif
