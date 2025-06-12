#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>

#include "../camera/camera.h"
#include "../vector/vector.h"
#include "../object/object.h"
#include "../light/light.h"
#include "../scene/scene.h"
// --- SDLApp Struct Definition ---


// This struct will hold all the necessary SDL components.
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int isRunning; // Flag to control the main loop

    int canvas_width;
    int canvas_height;

    Camera camera;

    Color background_color;
    Scene* scene;
} App;

// --- Function Prototypes ---
// Functions to initialize, run, and clean up the SDL application.
int App_Init(App* app, const char* title, int width, int height);
void App_Run(App* app);
void App_Cleanup(App* app);

void App_DrawPixel(App* app, int x, int y, Color color);


typedef struct IntersectionRoots {
    float root1;
    float root2;
} IntersectionRoots;

IntersectionRoots App_IntersectRaySphere(Vector3 origin, Vector3 direction, Object sphere);
Color App_TraceRay(App* app, Vector3 direction, float min_t, float max_t);