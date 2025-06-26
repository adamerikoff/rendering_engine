#include "./scene.h"

int scene_init(Scene* scene) {
    scene->objects = (ObjectList*)malloc(sizeof(ObjectList));

    if (scene->objects == NULL) {
        perror("Failed to allocate memory for scene->objects");
        return -1;
    }

    if (objectList_init(scene->objects) != 0) {
        free(scene->objects);
        scene->objects = NULL;
        fprintf(stderr, "Error: Failed to initialize scene->objects.\n");
        return -1;
    }

    Object sphere1 = object_new_sphere(vector3_new(0.0f, -1.0f, 3.0f), color_new(255.0f, 0.0f, 0.0f), 1.0f, 500);
    Object sphere2 = object_new_sphere(vector3_new(-2.0f, 0.0f, 4.0f), color_new(0.0f, 255.0f, 0.0f), 1.0f, 10);
    Object sphere3 = object_new_sphere(vector3_new(2.0f, 0.0f, 4.0f), color_new(0.0f, 0.0f, 255.0f), 1.0f, 500);
    Object sphere4 = object_new_sphere(vector3_new(0.0f, -5001.0f, 0.0f), color_new(255.0f, 0.0f, 0.0f), 5000.0f, 1000);

    objectList_add(scene->objects, sphere1);
    objectList_add(scene->objects, sphere2);
    objectList_add(scene->objects, sphere3);
    objectList_add(scene->objects, sphere4);

    scene->lights = (LightList*)malloc(sizeof(LightList));

    if (scene->lights == NULL) {
        perror("Failed to allocate memory for scene->lights");
        return -1;
    }

    if (lightList_init(scene->lights) != 0) {
        free(scene->objects);
        scene->objects = NULL;
        fprintf(stderr, "Error: Failed to initialize scene->lights.\n");
        return -1;
    }

    Light light1 = light_new_ambient(0.2f);
    Light light2 = light_new_point(vector3_new(2.0f, 1.0f, 0.0f), 0.6f);
    Light light3 = light_new_directional(vector3_new(1.0f, 4.0f, 4.0f), 0.2f);

    lightList_add(scene->lights, light1);
    lightList_add(scene->lights, light2);
    lightList_add(scene->lights, light3);

    return 0;
}

void scene_clean_up(Scene* scene) {
    if (scene == NULL) {
        // Nothing to clean up if the scene pointer is NULL
        return;
    }

    // Clean up objects list
    if (scene->objects != NULL) {
        // Assuming objectList_clean_up deallocates any internal arrays within ObjectList
        objectList_free(scene->objects);
        free(scene->objects);
        scene->objects = NULL; // Set to NULL after freeing to prevent double-free
    }

    // Clean up lights list
    if (scene->lights != NULL) {
        // Assuming lightList_clean_up deallocates any internal arrays within LightList
        lightList_free(scene->lights);
        free(scene->lights);
        scene->lights = NULL; // Set to NULL after freeing to prevent double-free
    }
}
