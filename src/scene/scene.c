#include "scene.h"
#include <stdlib.h> // For malloc, free, memcpy
#include <string.h> // For memcpy

Scene* Scene_New(void) {
    Scene* new_scene = (Scene*)malloc(sizeof(Scene));
    if (new_scene == NULL) {
        perror("Failed to allocate Scene");
        return NULL;
    }

    // Allocate and initialize the ObjectList
    new_scene->object_list = (ObjectList*)malloc(sizeof(ObjectList));
    if (new_scene->object_list == NULL) {
        perror("Failed to allocate ObjectList for Scene");
        free(new_scene); // Clean up partially allocated Scene
        return NULL;
    }
    *new_scene->object_list = ObjectList_New(); // Call your ObjectList constructor

    // Allocate and initialize the LightList
    new_scene->light_list = (LightList*)malloc(sizeof(LightList));
    if (new_scene->light_list == NULL) {
        perror("Failed to allocate LightList for Scene");
        ObjectList_Free(new_scene->object_list); // Clean up already allocated ObjectList
        free(new_scene->object_list); // Free the ObjectList struct itself
        free(new_scene); // Clean up partially allocated Scene
        return NULL;
    }
    *new_scene->light_list = LightList_New(); // Call your LightList constructor

    return new_scene;
}

void Scene_Free(Scene** scene_ptr) { // Takes a pointer to a pointer
    if (scene_ptr == NULL || *scene_ptr == NULL) {
        return;
    }

    Scene* scene = *scene_ptr; // Get the actual Scene pointer

    // Free the ObjectList and its contents
    if (scene->object_list != NULL) {
        ObjectList_Free(scene->object_list); // Free the *contents* (the objects array)
        free(scene->object_list);           // Free the ObjectList struct itself
        scene->object_list = NULL;
    }

    // Free the LightList and its contents
    if (scene->light_list != NULL) {
        LightList_Free(scene->light_list); // Free the *contents* (the lights array)
        free(scene->light_list);         // Free the LightList struct itself
        scene->light_list = NULL;
    }

    // Finally, free the Scene struct itself
    free(scene);
    *scene_ptr = NULL; // Set the caller's pointer to NULL to prevent dangling pointers
}