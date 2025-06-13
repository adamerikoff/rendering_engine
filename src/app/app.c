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
        1.0, // viewport_width
        1.0  // viewport_height (assuming square viewport based on CanvasToViewport usage)
    );

    app->background_color = Color_New(255, 255, 255, 255);

    app->scene = Scene_New();
    
    // Add spheres to the scene
    ObjectList_Add(
        app->scene->object_list, 
        Object_NewSphere(
            Vector3_New(0.0, -1.0, 4.0),
            Color_New(255, 0, 0, 255), // Red sphere
            1.0
        )
    );
    ObjectList_Add(
        app->scene->object_list, 
        Object_NewSphere(
            Vector3_New(-2.0, 0.0, 5.0),
            Color_New(0, 255, 0, 255), // Green sphere
            1.0
        )
    );
    ObjectList_Add(
        app->scene->object_list, 
        Object_NewSphere(
            Vector3_New(2.0, 0.0, 5.0),
            Color_New(0, 0, 255, 255), // Blue sphere
            1.0
        )
    );
    ObjectList_Add(
        app->scene->object_list, 
        Object_NewSphere(
            Vector3_New(0.0, -5001.0, 0.0),
            Color_New(255, 255, 0, 255), // Yellow ground sphere
            5000.0
        )
    );

    // Add lights to the scene
    LightList_Add(
        app->scene->light_list,
        Light_NewAmbient(0.2) // Ambient light
    );
    LightList_Add(
        app->scene->light_list,
        Light_NewPoint(
            Vector3_New(5.0, 1.0, 0.0),
            0.6 // Point light
        )
    );
    LightList_Add(
        app->scene->light_list,
        Light_NewDirectional(
            Vector3_New(1.0, 4.0, 4.0), // Direction of the light rays
            0.2 // Directional light
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
    SDL_Event event; // Renamed 'e' to 'event'
    
    // Main application loop
    while (app->isRunning) {
        // ======================
        // EVENT PROCESSING
        // ======================
        while (SDL_PollEvent(&event) != 0) { // Using 'event'
            if (event.type == SDL_QUIT) {
                app->isRunning = 0; // Exit loop on quit event
            } else if (event.type == SDL_KEYDOWN) {
                // A keyboard key was pressed
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: // Check if the pressed key is the Escape key
                        app->isRunning = 0; // Set flag to exit the main loop
                        break;
                    // Add other key handling here if needed
                }
            }
        }

        // ======================
        // RENDERING
        // ======================
        // Clear screen for new frame
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
        SDL_RenderClear(app->renderer);

        int canvas_half_width = app->canvas_width / 2; // Renamed 'canvas_width_mid'
        int canvas_half_height = app->canvas_height / 2; // Renamed 'canvas_height_mid'

        for (int pixel_x = -canvas_half_width; pixel_x < canvas_half_width; pixel_x++) { // Renamed 'x' to 'pixel_x'
            for (int pixel_y = -canvas_half_height; pixel_y < canvas_half_height; pixel_y++) { // Renamed 'y' to 'pixel_y'
                // Calculate ray direction from camera through the current pixel on the viewport
                Vector3 ray_direction = Camera_CanvasToViewport(app->camera, pixel_x, pixel_y, app->canvas_width, app->canvas_width); // Renamed 'direction' to 'ray_direction'
                
                // Trace the ray into the scene to find the color
                Color pixel_color = App_TraceRay(app, ray_direction, 0.1, FLT_MAX); // Renamed 'color' to 'pixel_color'
                
                // Draw the computed color at the pixel location
                App_DrawPixel(app, pixel_x, pixel_y, pixel_color);
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
void App_DrawPixel(App* app, int x_centered, int y_centered, Color color) { // Renamed x, y to x_centered, y_centered
    // Set the drawing color for the renderer using the provided Color structure's RGBA components.
    SDL_SetRenderDrawColor(app->renderer, color.r, color.g, color.b, color.a);

    // Convert the user-provided centered (x, y) coordinates to SDL's top-left (0,0) coordinate system.
    // SDL's origin (0,0) is typically the top-left corner of the window/renderer.
    // The canvas's center x is found by app->canvasWidth / 2. Adding 'x' shifts it right for positive 'x'.
    int sdl_x = (app->canvas_width / 2) + x_centered; // Renamed 'canvasX' to 'sdl_x'
    // The canvas's center y is found by app->canvasHeight / 2. Subtracting 'y' moves it up for positive 'y'
    // (since SDL's Y-axis increases downwards). Subtracting 1 ensures the pixel is correctly placed
    // given SDL's pixel rendering behavior.
    int sdl_y = (app->canvas_height / 2) - y_centered - 1; // Renamed 'canvasY' to 'sdl_y'

    // Perform bounds checking to ensure the calculated pixel coordinates are within the canvas's visible area.
    // If the pixel is outside these bounds, drawing it would have no effect or could potentially cause issues.
    if (sdl_x >= 0 && sdl_x < app->canvas_width &&
        sdl_y >= 0 && sdl_y < app->canvas_height) {
        // If the pixel is within bounds, draw the point on the renderer at the calculated SDL coordinates.
        SDL_RenderDrawPoint(app->renderer, sdl_x, sdl_y);
    } else {
        // If the pixel is out of bounds, print a message to the console for debugging purposes.
        printf("OUT OF BOUND: %d %d", sdl_x, sdl_y);
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

IntersectionRoots App_IntersectRaySphere(Vector3 ray_origin, Vector3 ray_direction, Object sphere_object) { // Renamed origin, direction, sphere
    IntersectionRoots intersection_t_values = {FLT_MAX, FLT_MAX}; // Renamed roots

    // Vector from ray origin to sphere center
    Vector3 origin_to_sphere_center = Vector3_Subtract(ray_origin, sphere_object.position); // Renamed oc

    // Quadratic equation coefficients: at^2 + bt + c = 0
    float a_coeff = Vector3_Dot(ray_direction, ray_direction); // Renamed a
    float b_coeff = 2.0f * Vector3_Dot(origin_to_sphere_center, ray_direction); // Renamed b
    float c_coeff = Vector3_Dot(origin_to_sphere_center, origin_to_sphere_center) - sphere_object.data.sphereData.radius * sphere_object.data.sphereData.radius; // Renamed c

    float discriminant = b_coeff * b_coeff - 4 * a_coeff * c_coeff;

    if (discriminant < 0) {
        // No real solutions, ray misses the sphere
        return intersection_t_values;
    }

    // Calculate the two possible 't' values
    intersection_t_values.root1 = (-b_coeff - sqrtf(discriminant)) / (2.0f * a_coeff);
    intersection_t_values.root2 = (-b_coeff + sqrtf(discriminant)) / (2.0f * a_coeff);

    return intersection_t_values;
}

Color App_TraceRay(App* app, Vector3 ray_direction, float t_min, float t_max) { // Renamed direction, min_t, max_t
    float closest_t_intersection = FLT_MAX; // Renamed closest_t
    Object* hit_object = NULL; // Renamed closest_sphere

    for (int i = 0; i < app->scene->object_list->count; i++) {
        Object current_object = app->scene->object_list->objects[i]; // Renamed to clearly identify current object
        IntersectionRoots current_roots = App_IntersectRaySphere(app->camera.position, ray_direction, current_object); // Renamed roots
        
        // Check if root1 is a valid intersection
        if (current_roots.root1 < closest_t_intersection && t_min < current_roots.root1 && current_roots.root1 < t_max) {
            closest_t_intersection = current_roots.root1;
            hit_object = &(app->scene->object_list->objects[i]);
        }
        // Check if root2 is a valid intersection (and closer than root1 if root1 was valid)
        if (current_roots.root2 < closest_t_intersection && t_min < current_roots.root2 && current_roots.root2 < t_max) {
            closest_t_intersection = current_roots.root2;
            hit_object = &(app->scene->object_list->objects[i]);
        }
    }   

    if (hit_object == NULL) {
        return app->background_color; // Ray didn't hit any object
    }

    // Calculate the exact 3D point where the ray hit the object
    Vector3 intersection_point = Vector3_Add(app->camera.position, Vector3_Scale(ray_direction, closest_t_intersection)); // Renamed point
    
    // Calculate the surface normal at the intersection point
    Vector3 surface_normal = Vector3_Subtract(intersection_point, hit_object->position); // Renamed normal

    // Compute the total light intensity at the intersection point
    float light_intensity = App_ComputeLightning(app, intersection_point, surface_normal); // Renamed intensity

    // Return the object's color multiplied by the calculated light intensity
    return Color_New(
        hit_object->color.r * light_intensity,
        hit_object->color.g * light_intensity,
        hit_object->color.b * light_intensity,
        hit_object->color.a
    );
}

float App_ComputeLightning(App* app, Vector3 surface_point, Vector3 surface_normal) { // Renamed point, normal
    float total_intensity = 0; // Renamed intensity
    
    // Normalize the surface normal once, as it's used repeatedly
    Vector3 normalized_surface_normal = Vector3_Normalize(surface_normal); 

    Vector3 light_direction_vector; // Renamed 'l' to be more descriptive

    for (int i = 0; i < app->scene->light_list->count; i++) {
        Light current_light = app->scene->light_list->lights[i]; // Renamed 'light' to 'current_light'

        switch (current_light.type) {
            case LIGHT_TYPE_AMBIENT:
                total_intensity += current_light.intensity;
                break;
            case LIGHT_TYPE_POINT:
                // Direction from the surface point towards the point light source
                light_direction_vector = Vector3_Subtract(current_light.data.pointData.position, surface_point);
                break;
            case LIGHT_TYPE_DIRECTIONAL:
                // Direction of the parallel rays for a directional light
                light_direction_vector = current_light.data.directionalData.direction;
                break;  
            default:
                printf("  Unknown Light Type encountered in App_ComputeLightning\n");
                break;
        }
        
        // For point and directional lights, calculate diffuse contribution
        // We normalize the light_direction_vector here before the dot product
        // This makes the dot product directly represent cos(theta)
        float normal_dot_light_direction = Vector3_Dot(normalized_surface_normal, Vector3_Normalize(light_direction_vector)); // Renamed n_dot_l
        
        // Only add diffuse light if the light source is on the "front" side of the surface
        if (normal_dot_light_direction > 0) {
            total_intensity += current_light.intensity * normal_dot_light_direction; // Simplified due to prior normalization of light_direction_vector
        }
    }
    return total_intensity;
}