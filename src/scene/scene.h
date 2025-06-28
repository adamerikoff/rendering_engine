#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>

#include "../object/object.h"
#include "../light/light.h"
#include "../vector/vector.h"
#include "../color/color.h"

#ifndef _SCENE_H_
#define _SCENE_H_

typedef struct Scene {
    ObjectList* objects;
    LightList* lights;
    Color background_color;
} Scene;

int scene_init(Scene* scene);
void scene_clean_up(Scene* scene);
#endif
