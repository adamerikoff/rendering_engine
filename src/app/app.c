#include "./app.h"

// Initializes the application, SDL, window, and render engine.
int application_init(Application* app, Canvas* canvas) {
    // Ensure app and canvas are not NULL
    if (app == NULL || canvas == NULL) {
        fprintf(stderr, "Error: application_init received NULL pointer for app or canvas.\n");
        return 1; // Indicate failure
    }

    // Assign the canvas to the application struct
    app->canvas = canvas;

    // CRUCIAL: Initialize all resource pointers to NULL.
    // This allows application_clean_up to safely check which resources need cleanup.
    app->window = NULL;
    app->engine = NULL;
    app->scene = NULL;
    app->camera = NULL;
    app->is_running = 0; // Not running yet

    printf("Initializing SDL...\n");
    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        application_clean_up(app); // Call cleanup on failure
        return 1;
    }

    printf("Creating window...\n");
    // Create a window
    app->window = SDL_CreateWindow(
        "Simple SDL Window with Renderer", // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        app->canvas->width,                // width, in pixels (using app->canvas)
        app->canvas->height,               // height, in pixels (using app->canvas)
        SDL_WINDOW_SHOWN                   // flags - SDL_WINDOW_SHOWN ensures it's visible
    );

    if (app->window == NULL) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        application_clean_up(app); // Call cleanup on failure
        return 1;
    }

    printf("Allocating engine...\n");
    // Allocate memory for the RenderEngine struct
    app->engine = (Engine*)malloc(sizeof(Engine));
    if (app->engine == NULL) {
        fprintf(stderr, "Failed to allocate memory for RenderEngine.\n");
        application_clean_up(app); // Call cleanup on failure
        return 1;
    }
    // Initialize the allocated RenderEngine to a known state
    app->engine->renderer = NULL; // Initialize sub-members if necessary

    printf("Initializing engine...\n");
    // Initialize the render engine
    if (engine_init(app->engine, app->window, canvas) != 0) {
        fprintf(stderr, "Failed to initialize render engine.\n");
        application_clean_up(app); // Call cleanup on failure
        return 1;
    }

    printf("Allocating scene...\n");
    // Allocate memory for scene
    app->scene = (Scene*)malloc(sizeof(Scene));
    if (app->scene == NULL) {
        fprintf(stderr, "Failed to allocate memory for Scene.\n");
        application_clean_up(app); // Call cleanup on failure
        return 1;
    }

    printf("Initializing scene...\n");
    // Initialize scene
    if (scene_init(app->scene) != 0) {
        fprintf(stderr, "Failed to initialize scene.\n");
        application_clean_up(app); // Call cleanup on failure
        return 1;
    }

    printf("Allocating camera...\n");
    // Allocate memory for camera
    app->camera = (Camera*)malloc(sizeof(Camera));
    if (app->camera == NULL) {
        fprintf(stderr, "Failed to allocate memory for camera.\n");
        application_clean_up(app); // Call cleanup on failure
        return 1;
    }

    printf("Creating camera...\n");
    // Initialize camera struct itself
    *app->camera = camera_new(vector3_new(0, 0, 0), 1.0f, canvas);

    // Set initial application state
    app->is_running = 1; // Application is running

    printf("Application initialized successfully.\n");
    return 0; // Indicate success
}

void application_loop(Application* app) {
    // Ensure app is not NULL
    if (app == NULL) {
        fprintf(stderr, "Error: application_loop received NULL pointer for app.\n");
        return;
    }

    SDL_Event e;

    // --- Optimization: Frame Rate Control ---
    const int FPS = 30; // Desired frames per second
    const int frameDelay = 1000 / FPS; // Milliseconds per frame

    Uint32 frameStart;
    int frameTime;

    // Main application loop
    while (app->is_running) {
        frameStart = SDL_GetTicks(); // Get the time at the start of the frame

        // Event polling loop
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                app->is_running = 0; // Set flag to exit main loop
            }
            // Keyboard event handling
            else if (e.type == SDL_KEYDOWN) {
                // Check if the pressed key is ESC
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    app->is_running = 0; // Set flag to exit main loop
                }
            }
        }

        // Call the render function from the render engine
        engine_render(app->engine, app->camera, app->scene, app->canvas); // Correct call, app->render_engine is already a pointer

        // --- Frame Rate Capping Logic ---
        frameTime = SDL_GetTicks() - frameStart; // Calculate time taken for this frame

        if (frameDelay > frameTime) {
            // If the frame took less time than desired, delay to cap FPS
            SDL_Delay(frameDelay - frameTime);
        }
        // Optional: If you want to log frame drops (frameTime > frameDelay), you can add it here.
    }
}

void application_exit(Application* app) {
    (void)app;
    // Quit SDL subsystems (must be called after all SDL objects are destroyed)
    // No need to check for app == NULL here, as SDL_Quit is global
    SDL_Quit();
}

/**
 * @brief Deallocates all resources held by the Application struct.
 * This function can be safely called even if some resources are NULL.
 * It's designed to be idempotent (calling it multiple times doesn't cause issues).
 * @param app A pointer to the Application struct.
 */
void application_clean_up(Application* app) {
    if (app == NULL) {
        return; // Nothing to clean up if app is NULL
    }

    printf("Starting application cleanup...\n");

    // Clean up camera (if dynamically allocated)
    if (app->camera != NULL) {
        // If camera_new or a similar function allocated internal memory,
        // you would call a camera_clean_up(app->camera) function here.
        // As per your example, it seems *app->camera is assigned a value,
        // so only the pointer itself needs to be freed.
        free(app->camera);
        app->camera = NULL;
        printf("Camera freed.\n");
    }

    // Clean up scene (if dynamically allocated)
    if (app->scene != NULL) {
        scene_clean_up(app->scene); // Call specific scene cleanup
        free(app->scene);
        app->scene = NULL;
        printf("Scene freed and cleaned up.\n");
    }

    // Clean up engine (if dynamically allocated)
    if (app->engine != NULL) {
        engine_clean_up(app->engine); // Call specific engine cleanup (e.g., destroys renderer)
        free(app->engine);
        app->engine = NULL;
        printf("Engine freed and cleaned up.\n");
    }

    // Destroy the SDL Window
    if (app->window != NULL) {
        SDL_DestroyWindow(app->window);
        app->window = NULL;
        printf("SDL Window destroyed.\n");
    }

    // Quit SDL subsystems
    // SDL_Quit is safe to call even if SDL_Init failed or was never called,
    // and multiple calls are harmless.
    SDL_Quit();
    printf("SDL subsystems quit.\n");

    printf("Application cleanup complete.\n");
}
