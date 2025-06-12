#include "app.h"
#include <float.h> // For FLT_MAX
#include <math.h>  // For sqrtf, fminf
#include <stdio.h> // For fprintf, printf
#include <SDL2/SDL.h>   // For SDL functions

// ============================================================================
// APPLICATION INITIALIZATION
// ============================================================================

/**
 * @brief Initializes the SDL application with window, renderer, and scene setup
 * @param app Pointer to App struct to initialize
 * @param title Window title string
 * @param width Window width in pixels
 * @param height Window height in pixels
 * @return 0 on success, 1 on failure
 */

int App_Init(App* app, const char* title, int width, int height) {
    // Initialize app state
    app->window = NULL;
    app->renderer = NULL;
    app->isRunning = 0; // Application not running yet
    
    // Set up canvas dimensions
    app->canvas_height = height;
    app->canvas_width = width;
    
    // ======================
    // INTERNAL SCENE INITIALIZATION
    // ======================

    app->camera = Camera_New(
        Vector3_New(0.0, 0.0, 0.0),
        1.0,
        1.0
    );

    app->background_color = Color_New(0, 0, 0, 255);

    app->scene = Scene_New();
    
    ObjectList_Add(
        app->scene->object_list, 
        Object_NewSphere(
            Vector3_New(0.0, 1.0, 3.0),
            Color_New(255, 0, 0, 255),
            1.0
        )
    );
    ObjectList_Add(
        app->scene->object_list, 
        Object_NewSphere(
            Vector3_New(-2.0, 0.0, 4.0),
            Color_New(0, 255, 0, 255),
            1.0
        )
    );
    ObjectList_Add(
        app->scene->object_list, 
        Object_NewSphere(
            Vector3_New(2.0, 0.0, 4.0),
            Color_New(0, 0, 255, 255),
            1.0
        )
    );
    ObjectList_Add(
        app->scene->object_list, 
        Object_NewSphere(
            Vector3_New(0.0, -5001.0, 0.0),
            Color_New(255, 255, 0, 255),
            5000.0
        )
    );


    // ======================
    // SDL INITIALIZATION
    // ======================
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create application window
    app->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );
    
    if (!app->window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create hardware-accelerated renderer
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
    if (!app->renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(app->window);
        SDL_Quit();
        return 1;
    }

    // Clear screen to black
    SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(app->renderer);
    SDL_RenderPresent(app->renderer);

    app->isRunning = 1; // Mark application as running
    return 0; // Initialization successful
}

// ============================================================================
// MAIN APPLICATION LOOP
// ============================================================================

/**
 * @brief Runs the main application event and rendering loop
 * @param app Pointer to the App struct containing application state
 */
void App_Run(App* app) {
    SDL_Event e;
    
    // Main application loop
    while (app->isRunning) {
        // ======================
        // EVENT PROCESSING
        // ======================
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                app->isRunning = 0; // Exit loop on quit event
            }
        }

        // ======================
        // RENDERING
        // ======================
        // Clear screen for new frame
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
        SDL_RenderClear(app->renderer);

        int canvas_width_mid = app->canvas_width / 2;
        int canvas_height_mid = app->canvas_height / 2;

        for (int x = -canvas_width_mid; x < canvas_width_mid; x++) {
            for (int y = -canvas_height_mid; y < canvas_height_mid; y++) {
                Vector3 direction = Camera_CanvasToViewport(app->camera, x, y, app->canvas_width, app->canvas_width);
                Color color = App_TraceRay(app, direction, 0.1, FLT_MAX);
                App_DrawPixel(app, x, y, color);
            }
        }

        // Display rendered frame
        SDL_RenderPresent(app->renderer);
        
        // Cap frame rate to ~60 FPS
        SDL_Delay(60);
    }
}

// ============================================================================
// RENDERING FUNCTIONS
// ============================================================================

/**
 * @brief Draws a single pixel on the application's renderer at a specified (x, y) coordinate with a given color.
 *
 * This function takes coordinates relative to the center of the canvas (where (0,0) is the center),
 * converts them to SDL's top-left (0,0) coordinate system, and then attempts to draw the pixel.
 * It also includes bounds checking to ensure the pixel is within the canvas dimensions.
 *
 * @param app A pointer to the App structure, containing the renderer and canvas dimensions.
 * @param x The x-coordinate of the pixel, relative to the center of the canvas.
 * @param y The y-coordinate of the pixel, relative to the center of the canvas.
 * @param color The Color structure containing the RGBA values for the pixel.
 */
void App_DrawPixel(App* app, int x, int y, Color color) {
    // Set the drawing color for the renderer using the provided Color structure's RGBA components.
    SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);

    // Convert the user-provided centered (x, y) coordinates to SDL's top-left (0,0) coordinate system.
    // SDL's origin (0,0) is typically the top-left corner of the window/renderer.
    // The canvas's center x is found by app->canvasWidth / 2. Adding 'x' shifts it right for positive 'x'.
    int canvasX = (app->canvas_width / 2) + x;
    // The canvas's center y is found by app->canvasHeight / 2. Subtracting 'y' moves it up for positive 'y'
    // (since SDL's Y-axis increases downwards). Subtracting 1 ensures the pixel is correctly placed
    // given SDL's pixel rendering behavior.
    int canvasY = (app->canvas_height / 2) - y - 1;

    // Perform bounds checking to ensure the calculated pixel coordinates are within the canvas's visible area.
    // If the pixel is outside these bounds, drawing it would have no effect or could potentially cause issues.
    if (canvasX >= 0 && canvasX < app->canvas_width &&
        canvasY >= 0 && canvasY < app->canvas_height) {
        // If the pixel is within bounds, draw the point on the renderer at the calculated SDL coordinates.
        SDL_RenderDrawPoint(app->renderer, canvasX, canvasY);
    } else {
        // If the pixel is out of bounds, print a message to the console for debugging purposes.
        printf("OUT OF BOUND: %d %d", canvasX, canvasY);
    }
}

// ============================================================================
// CLEANUP FUNCTIONS
// ============================================================================

/**
 * @brief Cleans up all application resources
 * @param app Pointer to App struct to clean up
 */
void App_Cleanup(App* app) {
    // Clean up SDL resources in reverse creation order
    if (app->renderer) {
        SDL_DestroyRenderer(app->renderer);
        app->renderer = NULL;
    }
    
    if (app->window) {
        SDL_DestroyWindow(app->window);
        app->window = NULL;
    }
    
    SDL_Quit();
}


// ============================================================================
// RAYTRACING FUNCTIONS
// ============================================================================

IntersectionRoots App_IntersectRaySphere(Vector3 origin, Vector3 direction, Object sphere) {
    IntersectionRoots roots = {FLT_MAX, FLT_MAX};

    Vector3 oc = Vector3_Subtract(origin, sphere.position);

    // Quadratic equation coefficients: at^2 + bt + c = 0
    float a = Vector3_Dot(direction, direction); // Equivalent to |ray_direction|^2
    float b = 2.0f * Vector3_Dot(oc, direction);
    float c = Vector3_Dot(oc, oc) - sphere.data.sphereData.radius * sphere.data.sphereData.radius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        // No real solutions, ray misses the sphere
        return roots;
    }

    // Calculate the two possible 't' values
    roots.root1 = (-b - sqrtf(discriminant)) / (2.0f * a);
    roots.root2 = (-b + sqrtf(discriminant)) / (2.0f * a);

    return roots;
}

Color App_TraceRay(App* app, Vector3 direction, float min_t, float max_t) {
    float closest_t = FLT_MAX;
    Object* closest_sphere = NULL;

    for (int i = 0; i < app->scene->object_list->count; i++) {
        IntersectionRoots roots = App_IntersectRaySphere(app->camera.position, direction, app->scene->object_list->objects[i]);
        
        if (roots.root1 < closest_t && min_t < roots.root1 && roots.root1 < max_t) {
            closest_t = roots.root1;
            closest_sphere = &(app->scene->object_list->objects[i]);
        }
        if (roots.root2 < closest_t && min_t < roots.root2 && roots.root2 < max_t) {
            closest_t = roots.root2;
            closest_sphere = &(app->scene->object_list->objects[i]);
        }
    }   

    if (closest_sphere == NULL) {
        return app->background_color;
    }

    return closest_sphere->color;
}