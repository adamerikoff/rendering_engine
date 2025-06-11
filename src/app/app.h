#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>

#include "../camera/camera.h"
#include "../vector/vector.h"
#include "../objects/sphere.h"
// --- SDLApp Struct Definition ---


// This struct will hold all the necessary SDL components.
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int isRunning; // Flag to control the main loop

    int canvasWidth;
    int canvasHeight;

    int maxAdjustedWidth;
    int maxAdjustedHeight;

    Camera camera;      // The camera for the scene

    Color backgroundColor;

    Sphere* spheres;
} App;

// --- Function Prototypes ---
// Functions to initialize, run, and clean up the SDL application.
int App_Init(App* app, const char* title, int width, int height);
void App_Run(App* app);
void App_Cleanup(App* app);

void App_DrawPixel(App* app, int x_adjusted, int y_adjusted, Color color);
Color App_TraceRay(App* app, Vector3 ray_origin, Vector3 ray_direction, float t_min, float t_max);
IntersectionRoots App_IntersectRaySphere(Vector3 origin, Vector3 direction, Sphere* sphere );