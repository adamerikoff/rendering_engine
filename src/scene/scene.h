#pragma once

#include "../light/light.h"
#include "../object/object.h"

typedef struct Scene {
    ObjectList* object_list;
    LightList* light_list;
} Scene;

Scene* Scene_New(void);
void Scene_Free(Scene** scene_ptr);
