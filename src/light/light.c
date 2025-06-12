#include "light.h"
#include <stdlib.h> // For malloc, realloc, free
#include <string.h> // For memcpy

#define INITIAL_LIGHT_CAPACITY 4 // Starting capacity for light list

LightList LightList_New(void) {
    LightList list;
    list.lights = NULL;
    list.capacity = 0;
    list.count = 0;
    return list;
}

int LightList_Add(LightList* list, Light light) {
    if (list == NULL) {
        return -1; // Invalid list
    }

    // If capacity is full, reallocate
    if (list->count == list->capacity) {
        size_t new_capacity = (list->capacity == 0) ? INITIAL_LIGHT_CAPACITY : list->capacity * 2;
        Light* new_lights = (Light*)realloc(list->lights, new_capacity * sizeof(Light));

        if (new_lights == NULL) {
            // Reallocation failed
            return -1;
        }
        list->lights = new_lights;
        list->capacity = new_capacity;
    }

    // Add the new light
    list->lights[list->count] = light;
    list->count++;

    return 0; // Success
}

void LightList_Free(LightList* list) {
    if (list != NULL && list->lights != NULL) {
        free(list->lights);
        list->lights = NULL;
        list->capacity = 0;
        list->count = 0;
    }
}

Light* LightList_Get(LightList* list, size_t index) {
    if (list != NULL && index < list->count) {
        return &list->lights[index];
    }
    return NULL; // Index out of bounds or invalid list
}

Light Light_NewAmbient(float intensity) {
    Light light;
    light.type = LIGHT_TYPE_AMBIENT;
    light.intensity = intensity;
    return light;
}

Light Light_NewPoint(Vector3 position, float intensity) {
    Light light;
    light.type = LIGHT_TYPE_POINT;
    light.intensity = intensity;
    light.data.pointData.position = position;
    return light;
}

Light Light_NewDirectional(Vector3 direction, float intensity) {
    Light light;
    light.type = LIGHT_TYPE_DIRECTIONAL;
    light.intensity = intensity;
    light.data.directionalData.direction = direction;
    return light;
}