#include "./object.h"

Object object_new_sphere(Vector3 center, Color color, float radius, int specularity, float reflectivity) {
    Object obj;
    obj.type = OBJECT_TYPE_SPHERE;
    obj.position = center;
    obj.color = color;
    obj.specularity = specularity;
    obj.data.sphereData.radius = radius;
    obj.reflectivity = reflectivity;
    return obj;
}

#define INITIAL_OBJECT_CAPACITY 8 // Starting capacity for object list

int objectList_init(ObjectList* objectList) {
    if (objectList == NULL) {
        fprintf(stderr, "Error: objectList_init received a NULL objectList pointer.\n");
        return -1;
    }

    objectList->objects = NULL;
    objectList->capacity = 0;
    objectList->count = 0;

    return 0;
}

int objectList_add(ObjectList* list, Object obj) {
    if (list == NULL) {
        return -1; // Invalid list
    }

    // If capacity is full, reallocate
    if (list->count == list->capacity) {
        int new_capacity = (list->capacity == 0) ? INITIAL_OBJECT_CAPACITY : list->capacity * 2;
        Object* new_objects = (Object*)realloc(list->objects, new_capacity * sizeof(Object));

        if (new_objects == NULL) {
            // Reallocation failed
            return -1;
        }
        list->objects = new_objects;
        list->capacity = new_capacity;
    }

    // Add the new object
    list->objects[list->count] = obj;
    list->count++;

    return 0; // Success
}

void objectList_free(ObjectList* list) {
    if (list != NULL && list->objects != NULL) {
        free(list->objects);
        list->objects = NULL;
        list->capacity = 0;
        list->count = 0;
    }
}

Object* objectList_get(ObjectList* list, int index) {
    if (list != NULL && index < list->count) {
        return &list->objects[index];
    }
    return NULL;
}
