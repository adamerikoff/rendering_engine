#include "./app.h"

// Initializes the application, SDL, window, and render engine.
int application_init(Application* app, Canvas* canvas) {
    // Ensure app and canvas are not NULL
    if (app == NULL || canvas == NULL) {
        fprintf(stderr, "Error: application_init received NULL pointer for app or canvas.\n");
        return 1;
    }

    // Assign the canvas to the application struct
    app->canvas = canvas;

    // Initialize members to NULL or default values
    app->window = NULL;
    app->engine = NULL;
    app->scene = NULL;  // Initialize scene to NULL
    app->camera = NULL; // Initialize camera to NULL
    app->is_running = 0; // Not running yet

    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1; // Indicate failure
    }

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
        SDL_Quit(); // Clean up SDL if window creation fails
        return 1;   // Indicate failure
    }

    // Allocate memory for the RenderEngine struct
    app->engine = (Engine*)malloc(sizeof(Engine));
    if (app->engine == NULL) {
        fprintf(stderr, "Failed to allocate memory for RenderEngine.\n");
        SDL_DestroyWindow(app->window); // Destroy window if allocation fails
        SDL_Quit();
        return 1;
    }
    // Initialize the allocated RenderEngine to a known state
    app->engine->renderer = NULL;

    // Initialize the render engine, passing the pointer to the allocated RenderEngine and the window
    // Assumes render_init is defined elsewhere (e.g., in engine.c)
    if (engine_init(app->engine, app->window, canvas) != 0) {
        fprintf(stderr, "Failed to initialize render engine.\n");
        // Clean up already allocated memory and SDL resources
        free(app->engine);
        app->engine = NULL; // Clear pointer
        SDL_DestroyWindow(app->window);
        app->window = NULL; // Clear pointer
        SDL_Quit();
        return 1; // Indicate failure
    }

    // Allocate and initialize scene
    app->scene = (Scene*)malloc(sizeof(Scene));
    if(scene_init(app->scene) != 0) {
        fprintf(stderr, "Failed to initialize scene.\n");
        // Clean up already initialized SDL and engine resources
        engine_clean_up(app->engine);
        free(app->engine);
        app->engine = NULL;
        SDL_DestroyWindow(app->window);
        app->window = NULL;
        SDL_Quit();
        return 1;
    }

    // Allocate and initialize camera
    app->camera = (Camera*)malloc(sizeof(Camera));
    if (app->camera == NULL) {
        fprintf(stderr, "Failed to allocate memory for camera.\n");
        // Clean up scene and other resources before exiting
        scene_clean_up(app->scene);
        free(app->scene);
        app->scene = NULL;
        engine_clean_up(app->engine);
        free(app->engine);
        app->engine = NULL;
        SDL_DestroyWindow(app->window);
        app->window = NULL;
        SDL_Quit();
        return 1;
    }
    
    *app->camera = camera_new(vector3_new(0, 0, 0), 1.0f, 1.0f);

    // Set initial application state
    app->is_running = 1; // Application is running

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

void application_clean_up(Application* app) {
    // Ensure app is not NULL
    if (app == NULL) {
        return;
    }

    // Clean up the render engine first (if it was allocated)
    if (app->engine != NULL) {
        engine_clean_up(app->engine); // Correct call
        free(app->engine); // Free the memory allocated for the RenderEngine struct
        app->engine = NULL; // Clear pointer after freeing
    }

    // Destroy the window
    if (app->window != NULL) {
        SDL_DestroyWindow(app->window);
        app->window = NULL; // Clear pointer after destruction
    }
}

void application_exit(Application* app) {
    (void)app;
    // Quit SDL subsystems (must be called after all SDL objects are destroyed)
    // No need to check for app == NULL here, as SDL_Quit is global
    SDL_Quit();
}
