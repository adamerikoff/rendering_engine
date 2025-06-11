#include "app.h"
#include <float.h>
#include <math.h>
// --- Function Implementations ---

/**
 * @brief Initializes the SDL application by creating a window and renderer.
 * @param app A pointer to the App struct to initialize.
 * @param title The title of the window.
 * @param width The width of the window.
 * @param height The height of the window.
 * @return 0 on success, 1 on failure.
 */
int App_Init(App* app, const char* title, int width, int height) {
    app->window = NULL;
    app->renderer = NULL;
    
    app->isRunning = 0; // Not running yet

    app->canvasHeight = height;
    app->canvasWidth = width;

    app->maxAdjustedHeight = height / 2;
    app->maxAdjustedWidth = width / 2;

    // --- Camera Initialization ---
    // Camera position (origin of the camera, usually {0,0,0} for simple raytracing)
    app->camera.position.x = 0.0f;
    app->camera.position.y = 0.0f;
    app->camera.position.z = 0.0f;

    // Viewport width in world units.
    // Common to set to 1.0, and then other scene objects are scaled relative to this.
    app->camera.viewportWidth = 1.0f;
    
    // Viewport height in world units, calculated to match canvas aspect ratio
    // to prevent image distortion.
    app->camera.viewportHeight = app->camera.viewportWidth * ((float)height / width);
    
    // Distance from camera to viewport. This defines the Z-coordinate of the viewport.
    // A smaller 'd' gives a wider field of view (more "fish-eye").
    // A larger 'd' gives a narrower field of view (more "telephoto").
    app->camera.d = 1.0f;

    app->backgroundColor = (Color) {.r = 0xEB, .g = 0xF0, .b = 0xBD, .a = 0x00};

    // Allocate memory for 'num_spheres' structs of type Sphere on the heap
    app->spheres = (Sphere*)malloc(3 * sizeof(Sphere));
    // Always check if malloc succeeded!
    if (app->spheres == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 1;
    }
    app->spheres[0] = (Sphere) {
        .position = (Vector3) {.x = 5, .y = 5, .z = 50},
        .radius = 5,
        .color = (Color) {.r = 0x3C, .g = 0x70, .b = 0x8F, .a = 0x00}
    };
    app->spheres[1] = (Sphere) {
        .position = (Vector3) {.x = -2, .y = 10, .z = 40},
        .radius = 8,
        .color = (Color) {.r = 0xC8, .g = 0x70, .b = 0xA, .a = 0x00}
    };
    app->spheres[2] = (Sphere) {
        .position = (Vector3) {.x = 20, .y = 20, .z = 70},
        .radius = 3,
        .color = (Color) {.r = 0xC3, .g = 0xB2, .b = 0x8F, .a = 0x00}
    };

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    app->window = SDL_CreateWindow(title,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  width,
                                  height,
                                  SDL_WINDOW_SHOWN);
    if (app->window == NULL) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
    if (app->renderer == NULL) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(app->window);
        SDL_Quit();
        return 1;
    }

    // Set draw color to black and clear the screen
    SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(app->renderer);
    SDL_RenderPresent(app->renderer);

    app->isRunning = 1; // Set running flag to true
    return 0; // Success
}

/**
 * @brief Runs the main event loop of the SDL application.
 * @param app A pointer to the App struct.
 */
void App_Run(App* app) {
    SDL_Event e;
    while (app->isRunning) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                app->isRunning = 0; // Set quit flag to exit loop
            }
        }
        // 2. Update Game State/Logic
        // This is where you would update positions of objects, handle physics, etc.
        // For example:
        // App_UpdateGameLogic(app);

        // 3. Rendering
        // Clear the screen at the beginning of each frame
        SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0xFF); // Set background color (e.g., black)
        SDL_RenderClear(app->renderer);

        // --- Raytracing Pixel Loop --- 
        for (int x_pixel = -app->maxAdjustedWidth; x_pixel < app->maxAdjustedWidth; x_pixel++) {
            for (int y_pixel = -app->maxAdjustedHeight; y_pixel < app->maxAdjustedHeight; y_pixel++) {
                // Get the 3D point on the viewport for this pixel using the camera function
                Vector3 viewport_point = Camera_GetViewportPointForPixel(
                                          &app->camera,
                                          x_pixel, y_pixel,
                                          app->canvasWidth, 
                                          app->canvasHeight
                                      );

                // --- For Demonstration: Simple Color Based on Viewport Coordinates ---
                // (Adjust the color calculation to use viewport_point's components)
                Color color = App_TraceRay(app, app->camera.position, viewport_point, 0.001f, FLT_MAX);

                App_DrawPixel(app, x_pixel, y_pixel, color);
            }
        }

        // Present the rendered content to the screen
        SDL_RenderPresent(app->renderer);

        // Add a small delay to control frame rate (e.g., aiming for ~60 FPS)
        SDL_Delay(16);
    }
}

/**
 * @brief Set color for a specific pixel on the screen.
 * @param app A pointer to the App struct.
 */
void App_DrawPixel(App* app, int x_adjusted, int y_adjusted, Color color) {
    // 1. Set the drawing color for the renderer
    SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);

    // Convert centered x, y to SDL's top-left (0,0) coordinate system
    int canvasX = x_adjusted + (app->canvasWidth / 2);
    int canvasY = (app->canvasHeight / 2) - 1 - y_adjusted;



    // It's good practice to add bounds checks here, though not strictly required if
    // your loops are correct.
    if (canvasX >= 0 && canvasX < app->canvasWidth &&
        canvasY >= 0 && canvasY < app->canvasHeight) {
        SDL_RenderDrawPoint(app->renderer, canvasX, canvasY);
    } else {
        printf("OUT OF BOUND: %d %d", canvasX, canvasY);
    }
}

/**
 * @brief Cleans up all SDL resources.
 * @param app A pointer to the App struct to clean up.
 * @param app A pointer to the App struct to clean up.
 * @param app A pointer to the App struct to clean up.
 * @param app A pointer to the App struct to clean up.
 */
Color App_TraceRay(App* app, Vector3 ray_origin, Vector3 ray_direction, float t_min, float t_max) {
    Sphere* closest_sphere = NULL; // Pointer to the closest sphere hit
    float closest_t = t_max;       // Initialize with the maximum possible 't' value

    // Normalize the ray direction. This is crucial for correct distance (t) calculations
    // and consistent behavior in intersection tests.
    ray_direction = Vector3Normalize(ray_direction);

    // Loop through all spheres in the scene to find the closest intersection
    for (int i = 0; i < 3; i++) {
        // Calculate intersection roots for the current sphere
        IntersectionRoots roots = App_IntersectRaySphere(ray_origin, ray_direction, &(app->spheres[i]));

        // Check the first root (root1)
        if (roots.root1 > t_min && roots.root1 < closest_t) {
            closest_t = roots.root1;           // Update closest 't'
            closest_sphere = &(app->spheres[i]); // Store pointer to the current sphere
        }

        // Check the second root (root2)
        // It's important to check both, as root2 might be closer if root1 is negative (behind ray origin)
        // or just beyond the current closest_t.
        if (roots.root2 > t_min && roots.root2 < closest_t) {
            closest_t = roots.root2;           // Update closest 't'
            closest_sphere = &(app->spheres[i]); // Store pointer to the current sphere
        }
    }

    // After checking all spheres, determine what to return
    if (closest_sphere == NULL) {
        return app->backgroundColor;
    } else {
        return closest_sphere->color;
    }
}

IntersectionRoots App_IntersectRaySphere(Vector3 ray_origin, Vector3 ray_direction, Sphere* sphere) {
    IntersectionRoots roots = {FLT_MAX, FLT_MAX}; // Initialize with no valid roots

    Vector3 oc = Vector3Difference(ray_origin, sphere->position);

    // Quadratic equation coefficients: at^2 + bt + c = 0
    float a = Vector3Dot(ray_direction, ray_direction); // Equivalent to |ray_direction|^2
    float b = 2.0f * Vector3Dot(oc, ray_direction);
    float c = Vector3Dot(oc, oc) - sphere->radius * sphere->radius;

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

/**
 * @brief Cleans up all SDL resources.
 * @param app A pointer to the App struct to clean up.
 */
void App_Cleanup(App* app) {
    // Clean up camera resources (even if currently empty)
    Camera_Cleanup(&app->camera);

    if (app->renderer != NULL) {
        SDL_DestroyRenderer(app->renderer);
        app->renderer = NULL;
    }
    if (app->window != NULL) {
        SDL_DestroyWindow(app->window);
        app->window = NULL;
    }
    SDL_Quit();
}