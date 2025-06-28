#include "light.h"

#define INITIAL_LIGHT_CAPACITY 4 // Starting capacity for light list

int lightList_init(LightList* lightList) {
    if (lightList == NULL) {
        fprintf(stderr, "Error: lightList_init received a NULL objectList pointer.\n");
        return -1;
    }

    lightList->lights = NULL;
    lightList->capacity = 0;
    lightList->count = 0;

    return 0;
}

int lightList_add(LightList* list, Light light) {
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

void lightList_free(LightList* list) {
    if (list != NULL && list->lights != NULL) {
        free(list->lights);
        list->lights = NULL;
        list->capacity = 0;
        list->count = 0;
    }
}

Light* lightList_get(LightList* list, size_t index) {
    if (list != NULL && index < list->count) {
        return &list->lights[index];
    }
    return NULL; // Index out of bounds or invalid list
}

Light light_new_ambient(float intensity) {
    Light light;
    light.type = LIGHT_TYPE_AMBIENT;
    light.intensity = intensity;
    return light;
}

Light light_new_point(Vector3 position, float intensity) {
    Light light;
    light.type = LIGHT_TYPE_POINT;
    light.intensity = intensity;
    light.data.pointData.position = position;
    return light;
}

Light light_new_directional(Vector3 direction, float intensity) {
    Light light;
    light.type = LIGHT_TYPE_DIRECTIONAL;
    light.intensity = intensity;
    light.data.directionalData.direction = direction;
    return light;
}
