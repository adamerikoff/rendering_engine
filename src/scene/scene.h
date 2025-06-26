#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>

#include "../object/object.h"
#include "../light/light.h"
#include "../vector/vector.h"

#ifndef _SCENE_H_
#define _SCENE_H_

typedef struct Scene {
    ObjectList* objects;
    LightList* lights;
} Scene;

int scene_init(Scene* scene);
void scene_clean_up(Scene* scene);
#endif
