// --- app.h header (updated for new names) ---
#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>

#include "../camera/camera.h"
#include "../vector/vector.h"
#include "../object/object.h"
#include "../light/light.h"
#include "../scene/scene.h"

// --- SDLApp Struct Definition ---
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

void App_DrawPixel(App* app, int x_centered, int y_centered, Color color); // Updated parameter names

typedef struct IntersectionRoots {
    float root1;
    float root2;
} IntersectionRoots;

// Updated parameter names
IntersectionRoots App_IntersectRaySphere(Vector3 ray_origin, Vector3 ray_direction, Object sphere_object); 
// Updated parameter names
Color App_TraceRay(App* app, Vector3 ray_direction, float t_min, float t_max); 
// Updated parameter names
float App_ComputeLightning(App* app, Vector3 surface_point, Vector3 surface_normal, int specular_exponent, Vector3 ray_direction_from_camera);