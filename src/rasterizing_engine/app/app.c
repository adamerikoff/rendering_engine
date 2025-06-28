#include "./app.h" // Include the updated header

// Initializes the application, SDL, window, renderer, and canvas texture.
int application_init(Application* app, int width, int height) {
    if (app == NULL) {
        fprintf(stderr, "Error: application_init received NULL pointer for app.\n");
        return 1;
    }

    // CRUCIAL: Initialize all resource pointers to NULL.
    app->window = NULL;
    app->renderer = NULL;
    app->canvas_texture = NULL;
    app->pixel_buffer = NULL;
    app->is_running = 0;

    app->canvas_width = width;
    app->canvas_height = height;

    printf("Initializing SDL...\n");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        application_clean_up(app); // Call cleanup on failure (still using old name here for safe cleanup)
        return 1;
    }

    printf("Creating window...\n");
    app->window = SDL_CreateWindow(
        "Raycaster to Rasterizer Demo",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        app->canvas_width,
        app->canvas_height,
        SDL_WINDOW_SHOWN
    );

    if (app->window == NULL) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        application_clean_up(app); // Call cleanup on failure
        return 1;
    }

    printf("Creating renderer...\n");
    app->renderer = SDL_CreateRenderer(
        app->window,
        -1, // Best rendering driver
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC // Use hardware acceleration and VSync
    );

    if (app->renderer == NULL) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        application_clean_up(app); // Call cleanup on failure
        return 1;
    }

    // Create a texture to act as our drawing canvas
    // SDL_TEXTUREACCESS_STREAMING allows us to update pixel data directly
    app->canvas_texture = SDL_CreateTexture(
        app->renderer,
        SDL_PIXELFORMAT_ARGB8888, // Matches our pixel_buffer format
        SDL_TEXTUREACCESS_STREAMING,
        app->canvas_width,
        app->canvas_height
    );

    if (app->canvas_texture == NULL) {
        fprintf(stderr, "Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        application_clean_up(app); // Call cleanup on failure
        return 1;
    }

    // Allocate memory for our pixel buffer (one unsigned int per pixel for ARGB8888)
    app->pixel_buffer = (unsigned int*)malloc(app->canvas_width * app->canvas_height * sizeof(unsigned int));
    if (app->pixel_buffer == NULL) {
        fprintf(stderr, "Pixel buffer allocation failed!\n");
        application_clean_up(app); // Call cleanup on failure
        return 1;
    }

    // Initialize pixel buffer to black
    clear_canvas(app, COLOR_BLACK); // Renamed call

    app->is_running = 1;
    printf("Application initialized successfully.\n");
    return 0;
}

// --- Canvas Operations ---

// Sets a pixel on our software canvas (pixel_buffer)
// x, y are SDL screen coordinates (0,0 top-left, Y-down)
void put_pixel(Application* app, int x, int y, unsigned int color) { // Renamed definition
    if (app == NULL || app->pixel_buffer == NULL) return;

    // Check bounds to prevent out-of-bounds access
    if (x >= 0 && x < app->canvas_width && y >= 0 && y < app->canvas_height) {
        app->pixel_buffer[y * app->canvas_width + x] = color;
    }
}

// Clears the entire canvas to a given color
void clear_canvas(Application* app, unsigned int color) { // Renamed definition
    if (app == NULL || app->pixel_buffer == NULL) return;

    for (int i = 0; i < app->canvas_width * app->canvas_height; ++i) {
        app->pixel_buffer[i] = color;
    }
}

// Transfers the pixel_buffer data to the SDL texture
void update_canvas_texture(Application* app) { // Renamed definition
    if (app == NULL || app->renderer == NULL || app->canvas_texture == NULL || app->pixel_buffer == NULL) return;

    // Update the texture with the pixel data from our buffer
    SDL_UpdateTexture(
        app->canvas_texture,
        NULL, // Null rect means update the entire texture
        app->pixel_buffer,
        app->canvas_width * sizeof(unsigned int) // Pitch: bytes per row
    );

    // Clear the renderer (optional, but good practice before copying texture)
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255); // Black
    SDL_RenderClear(app->renderer);

    // Copy the texture to the renderer
    SDL_RenderCopy(app->renderer, app->canvas_texture, NULL, NULL);

    // Present the renderer
    SDL_RenderPresent(app->renderer);
}

// --- Drawing Primitives based on Article ---

// Helper function for swapping Point2D
void swap_points(Point2D* p1, Point2D* p2) { // Already lowercase with underscore
    Point2D temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

// Interpolate function (returns a dynamically allocated float array)
float* interpolate(float i0, float d0, float i1, float d1, int* out_count) { // Renamed definition
    if (i0 == i1) {
        *out_count = 1;
        float* values = (float*)malloc(sizeof(float));
        if (values) {
            values[0] = d0;
        } else {
            fprintf(stderr, "Memory allocation failed for interpolate (single point).\n"); // Renamed call
        }
        return values;
    }

    int count = (int)round(i1) - (int)round(i0) + 1; // Number of integer steps from i0 to i1
    *out_count = count;

    float* values = (float*)malloc(count * sizeof(float));
    if (values == NULL) {
        fprintf(stderr, "Memory allocation failed for interpolate.\n"); // Renamed call
        return NULL;
    }

    float a = (d1 - d0) / (i1 - i0);
    float d = d0;

    for (int idx = 0; idx < count; ++idx) {
        values[idx] = d;
        d += a;
    }
    return values;
}

// draw_line function from the article (Listing 6-2)
// P0 and P1 are in *true geometrical coordinates* (centered, Y-up)
void draw_line(Application* app, Point2D P0, Point2D P1, unsigned int color) { // Renamed definition
    // --- Coordinate Transformation from Geometrical to SDL Pixel Space ---
    // So, we convert P0 and P1 from geo-space to pixel-space BEFORE calling the
    // core line drawing logic.
    // Origin: (0,0) center of screen in geo-space.
    // Y-axis: Positive up in geo-space.
    // SDL: (0,0) top-left, Y-down.

    // Transform P0 - now using app->canvas_width/height directly
    int x0_pixel = (int)round(P0.x + app->canvas_width / 2.0f);
    int y0_pixel = (int)round(app->canvas_height / 2.0f - P0.y); // Y-axis flip

    // Transform P1 - now using app->canvas_width/height directly
    int x1_pixel = (int)round(P1.x + app->canvas_width / 2.0f);
    int y1_pixel = (int)round(app->canvas_height / 2.0f - P1.y); // Y-axis flip
    
    // Create new points for the transformed pixel coordinates
    Point2D p0_screen = { (float)x0_pixel, (float)y0_pixel };
    Point2D p1_screen = { (float)x1_pixel, (float)y1_pixel };
    
    float dx = p1_screen.x - p0_screen.x;
    float dy = p1_screen.y - p0_screen.y;

    if (fabs(dx) > fabs(dy)) {
        // Line is horizontal-ish
        // Make sure x0 < x1 for iteration
        if (p0_screen.x > p1_screen.x) {
            swap_points(&p0_screen, &p1_screen);
        }

        int ys_count;
        float* ys = interpolate(p0_screen.x, p0_screen.y, p1_screen.x, p1_screen.y, &ys_count); // Renamed call
        if (ys == NULL) return; // Handle allocation failure

        for (int x = (int)round(p0_screen.x); x <= (int)round(p1_screen.x); ++x) {
            put_pixel(app, x, (int)round(ys[x - (int)round(p0_screen.x)]), color); // Renamed call
        }
        free(ys); // Free allocated memory
    } else {
        // Line is vertical-ish
        // Make sure y0 < y1 for iteration
        if (p0_screen.y > p1_screen.y) {
            swap_points(&p0_screen, &p1_screen);
        }

        int xs_count;
        float* xs = interpolate(p0_screen.y, p0_screen.x, p1_screen.y, p1_screen.x, &xs_count); // Renamed call
        if (xs == NULL) return; // Handle allocation failure

        for (int y = (int)round(p0_screen.y); y <= (int)round(p1_screen.y); ++y) {
            put_pixel(app, (int)round(xs[y - (int)round(p0_screen.y)]), y, color); // Renamed call
        }
        free(xs); // Free allocated memory
    }
}


void application_loop(Application* app) { // Renamed definition
    if (app == NULL) {
        fprintf(stderr, "Error: application_loop received NULL pointer for app.\n"); // Renamed call
        return;
    }

    SDL_Event e;

    const int FPS = 30; // Increased FPS for smoother drawing
    const int frameDelay = 1000 / FPS;

    Uint32 frameStart;
    int frameTime;

    // Main application loop
    while (app->is_running) {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                app->is_running = 0;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    app->is_running = 0;
                }
            }
        }

        // --- Drawing happens here ---
        clear_canvas(app, COLOR_BLACK); // Renamed call

        // Example drawing: Draw a few lines
        // These points are in our "true geometrical coordinates" (centered, Y-up)
        // Center of screen (0,0) in geo-coords
        Point2D p_center = {0.0f, 0.0f};

        // Draw a horizontal line
        Point2D p_h1 = {-200.0f, 50.0f};
        Point2D p_h2 = {200.0f, 50.0f};
        draw_line(app, p_h1, p_h2, COLOR_RED); // Renamed call

        // Draw a vertical line
        Point2D p_v1 = {-100.0f, -150.0f};
        Point2D p_v2 = {-100.0f, 150.0f};
        draw_line(app, p_v1, p_v2, COLOR_GREEN); // Renamed call

        // Draw a diagonal line (similar to Figure 6-1, but adjusted for scale)
        Point2D p_diag1 = {-200.0f, -100.0f};
        Point2D p_diag2 = {240.0f, 120.0f};
        draw_line(app, p_diag1, p_diag2, COLOR_BLUE); // Renamed call
        
        // Draw another diagonal line (similar to Figure 6-3, steep)
        Point2D p_steep1 = {-50.0f, -200.0f};
        Point2D p_steep2 = {60.0f, 240.0f};
        draw_line(app, p_steep1, p_steep2, COLOR_WHITE); // Renamed call

        // Draw a line connecting two corners of the screen (in geo-coords)
        // Screen corners (in geo-coords, relative to center):
        // Top-Left: (-width/2, height/2)
        // Bottom-Right: (width/2, -height/2)
        Point2D p_corner1 = {-(float)app->canvas_width / 2.0f, (float)app->canvas_height / 2.0f};
        Point2D p_corner2 = {(float)app->canvas_width / 2.0f, -(float)app->canvas_height / 2.0f};
        draw_line(app, p_corner1, p_corner2, COLOR_RED); // Renamed call


        update_canvas_texture(app); // Renamed call - Push changes to the screen

        // --- Frame Rate Capping Logic ---
        frameTime = SDL_GetTicks() - frameStart;

        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void application_exit(Application* app) { // Renamed definition
    (void)app; // Suppress unused parameter warning
    SDL_Quit(); // Global SDL cleanup
}

void application_clean_up(Application* app) { // Renamed definition
    if (app == NULL) {
        return;
    }

    printf("Starting application cleanup...\n");

    if (app->pixel_buffer != NULL) {
        free(app->pixel_buffer);
        app->pixel_buffer = NULL;
        printf("Pixel buffer freed.\n");
    }

    if (app->canvas_texture != NULL) {
        SDL_DestroyTexture(app->canvas_texture);
        app->canvas_texture = NULL;
        printf("SDL Texture destroyed.\n");
    }

    if (app->renderer != NULL) {
        SDL_DestroyRenderer(app->renderer);
        app->renderer = NULL;
        printf("SDL Renderer destroyed.\n");
    }

    if (app->window != NULL) {
        SDL_DestroyWindow(app->window);
        app->window = NULL;
        printf("SDL Window destroyed.\n");
    }

    // SDL_Quit will be called by application_exit, which is called after this.
    // If application_exit is not guaranteed, then SDL_Quit() should be here too.
    // For this structure, it's fine.
    printf("Application cleanup complete.\n");
}
