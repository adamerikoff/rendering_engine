#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>

#include "../engine/engine.h"
#include "../camera/camera.h"
#include "../scene/scene.h"
#include "../canvas/canvas.h"

#ifndef _APP_H_
#define _APP_H_

// Define the Application struct
typedef struct Application {
    Canvas* canvas;

    SDL_Window* window;

    Engine* engine;

    Scene* scene;

    Camera* camera;

    int is_running;
    
} Application;

int application_init(Application* app, Canvas* canvas);

void application_loop(Application* app);

void application_clean_up(Application* app);

void application_exit(Application* app);

#endif
