#include "./app.h" // Include the updated header
#include <stdlib.h> // For malloc, free
#include <math.h>   // For roundf, fabs, floorf
#include <string.h> // For memcpy

// Helper function to extract individual color components
unsigned char get_red(unsigned int color) {
    return (color >> 16) & 0xFF;
}

unsigned char get_green(unsigned int color) {
    return (color >> 8) & 0xFF;
}

unsigned char get_blue(unsigned int color) {
    return color & 0xFF;
}

// Helper function to combine color components back into an unsigned int
unsigned int make_color(unsigned char r, unsigned char g, unsigned char b) {
    return (0xFF000000 | ((unsigned int)r << 16) | ((unsigned int)g << 8) | (unsigned int)b);
}

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
        "Rasterizer Demo: Shaded Triangles", // Updated window title
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
    clear_canvas(app, COLOR_BLACK);

    app->is_running = 1;
    printf("Application initialized successfully.\n");
    return 0;
}

// --- Canvas Operations ---

// Sets a pixel on our software canvas (pixel_buffer)
// x, y are SDL screen coordinates (0,0 top-left, Y-down)
void put_pixel(Application* app, int x, int y, unsigned int color) {
    if (app == NULL || app->pixel_buffer == NULL) return;

    // Check bounds to prevent out-of-bounds access
    if (x >= 0 && x < app->canvas_width && y >= 0 && y < app->canvas_height) {
        app->pixel_buffer[y * app->canvas_width + x] = color;
    }
}

// Clears the entire canvas to a given color
void clear_canvas(Application* app, unsigned int color) {
    if (app == NULL || app->pixel_buffer == NULL) return;

    for (int i = 0; i < app->canvas_width * app->canvas_height; ++i) {
        app->pixel_buffer[i] = color;
    }
}

// Transfers the pixel_buffer data to the SDL texture
void update_canvas_texture(Application* app) {
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

// Helper function for swapping Point2D (now includes h)
void swap_points(Point2D* p1, Point2D* p2) {
    Point2D temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

// Interpolate function (returns a dynamically allocated float array)
// It's the caller's responsibility to free the returned array.
// i0, i1 are independent variable (e.g., y values), d0, d1 are dependent (e.g., x or h values)
float* interpolate(float i0, float d0, float i1, float d1, int* out_count) {
    // If start and end independent variables are the same, return a single point
    if (fabs(i0 - i1) < 0.001f) { // Use epsilon for float comparison
        *out_count = 1;
        float* values = (float*)malloc(sizeof(float));
        if (values) {
            values[0] = d0;
        } else {
            fprintf(stderr, "Memory allocation failed for interpolate (single point).\n");
        }
        return values;
    }

    // Number of integer steps from floor(i0) to floor(i1) inclusive
    // The book's definition leads to (int)round(i1) - (int)round(i0) + 1 steps.
    // However, the common approach for pixel-perfect interpolation is to consider
    // the integer range covered. Let's use the ceiling of the absolute difference
    // to determine array size, and then iterate through the pixel range.
    // The book's pseudocode implies array indexing based on `y - y0`.
    // So the count should be `(int)roundf(i1) - (int)roundf(i0) + 1`.
    // Let's use `floorf` for consistency with `y - y0` indexing when iterating.
    int start_i = (int)floorf(i0);
    int end_i = (int)floorf(i1);

    // The number of *elements* in the array should cover all integer steps between i0 and i1.
    // If i0=0.1, i1=0.9, (int)floorf(i0)=0, (int)floorf(i1)=0. Count is 1 (for index 0).
    // If i0=0.1, i1=1.9, (int)floorf(i0)=0, (int)floorf(i1)=1. Count is 2 (for indices 0, 1).
    // The length of the array needed is `floor(i_max) - floor(i_min) + 1`.
    *out_count = (int)roundf(i1) - (int)roundf(i0) + 1; // Following book's implicit count logic

    // If out_count is less than 1, it means the range is invalid or points are too close
    if (*out_count <= 0) {
        *out_count = 0;
        return NULL;
    }
    
    float* values = (float*)malloc((*out_count) * sizeof(float));
    if (values == NULL) {
        fprintf(stderr, "Memory allocation failed for interpolate.\n");
        return NULL;
    }

    float a = (d1 - d0) / (i1 - i0); // Slope
    // The starting value of d for the first integer step 'roundf(i0)'
    float d = d0 + a * ((float)roundf(i0) - i0);

    for (int idx = 0; idx < *out_count; ++idx) {
        values[idx] = d;
        d += a;
    }
    return values;
}

// draw_line function from the article (Listing 6-2)
// P0 and P1 are in *true geometrical coordinates* (centered, Y-up)
void draw_line(Application* app, Point2D P0, Point2D P1, unsigned int color) {
    // --- Coordinate Transformation from Geometrical to SDL Pixel Space ---
    // So, we convert P0 and P1 from geo-space to pixel-space BEFORE calling the
    // core line drawing logic.
    // Origin: (0,0) center of screen in geo-space.
    // Y-axis: Positive up in geo-space.
    // SDL: (0,0) top-left, Y-down.

    // Transform P0
    int x0_pixel = (int)roundf(P0.x + app->canvas_width / 2.0f);
    int y0_pixel = (int)roundf(app->canvas_height / 2.0f - P0.y); // Y-axis flip

    // Transform P1
    int x1_pixel = (int)roundf(P1.x + app->canvas_width / 2.0f);
    int y1_pixel = (int)roundf(app->canvas_height / 2.0f - P1.y); // Y-axis flip
    
    // Create new points for the transformed pixel coordinates (h not used for wireframe)
    Point2D p0_screen = { (float)x0_pixel, (float)y0_pixel, 0.0f };
    Point2D p1_screen = { (float)x1_pixel, (float)y1_pixel, 0.0f };
    
    float dx = p1_screen.x - p0_screen.x;
    float dy = p1_screen.y - p0_screen.y;

    if (fabs(dx) > fabs(dy)) {
        // Line is horizontal-ish (more horizontal than vertical)
        // Ensure p0_screen.x is the smaller x for iteration
        if (p0_screen.x > p1_screen.x) {
            swap_points(&p0_screen, &p1_screen);
        }

        int ys_count;
        // Interpolate y values based on x as independent variable
        float* ys = interpolate(p0_screen.x, p0_screen.y, p1_screen.x, p1_screen.y, &ys_count);
        if (ys == NULL) {
             fprintf(stderr, "Error: interpolate returned NULL for horizontal-ish line.\n");
             return; // Handle allocation failure
        }

        // Iterate through x coordinates and plot pixels
        for (int x = 0; x < ys_count; ++x) {
            put_pixel(app, (int)roundf(p0_screen.x + x), (int)roundf(ys[x]), color);
        }
        free(ys); // Free allocated memory
    } else {
        // Line is vertical-ish (more vertical than horizontal)
        // Ensure p0_screen.y is the smaller y for iteration
        if (p0_screen.y > p1_screen.y) {
            swap_points(&p0_screen, &p1_screen);
        }

        int xs_count;
        // Interpolate x values based on y as independent variable
        float* xs = interpolate(p0_screen.y, p0_screen.x, p1_screen.y, p1_screen.x, &xs_count);
        if (xs == NULL) {
            fprintf(stderr, "Error: interpolate returned NULL for vertical-ish line.\n");
            return; // Handle allocation failure
        }

        // Iterate through y coordinates and plot pixels
        for (int y = 0; y < xs_count; ++y) {
            put_pixel(app, (int)roundf(xs[y]), (int)roundf(p0_screen.y + y), color);
        }
        free(xs); // Free allocated memory
    }
}

// Draw a wireframe triangle by drawing its three edges.
// P0, P1, P2 are in *true geometrical coordinates* (centered, Y-up)
void draw_wireframe_triangle(Application* app, Point2D p0, Point2D p1, Point2D p2, unsigned int color) {
    draw_line(app, p0, p1, color);
    draw_line(app, p1, p2, color);
    draw_line(app, p2, p0, color);
}


// New: Draw a shaded triangle (replaces draw_filled_triangle)
// P0, P1, P2 are in *true geometrical coordinates* (centered, Y-up)
void draw_shaded_triangle(Application* app, Point2D p0, Point2D p1, Point2D p2, unsigned int base_color) {
    // 1. Create a local array of points and sort them by Y-coordinate: p[0].y <= p[1].y <= p[2].y
    Point2D p[3] = {p0, p1, p2};

    // Sort the points by Y-coordinate (Bubble sort like logic), swapping all attributes (x, y, h)
    if (p[1].y < p[0].y) { swap_points(&p[1], &p[0]); }
    if (p[2].y < p[0].y) { swap_points(&p[2], &p[0]); }
    if (p[2].y < p[1].y) { swap_points(&p[2], &p[1]); }

    // If the triangle is degenerate (e.g., all points on a line, or a single point)
    // where y0 == y2, there's nothing to fill.
    if (fabs(p[0].y - p[2].y) < 0.001f) {
        return;
    }

    // 2. Compute the x coordinates AND h values of the triangle edges using interpolate
    int x01_count, h01_count;
    float* x01 = interpolate(p[0].y, p[0].x, p[1].y, p[1].x, &x01_count);
    float* h01 = interpolate(p[0].y, p[0].h, p[1].y, p[1].h, &h01_count);

    int x12_count, h12_count;
    float* x12 = interpolate(p[1].y, p[1].x, p[2].y, p[2].x, &x12_count);
    float* h12 = interpolate(p[1].y, p[1].h, p[2].y, p[2].h, &h12_count);
    
    int x02_count, h02_count;
    float* x02 = interpolate(p[0].y, p[0].x, p[2].y, p[2].x, &x02_count);
    float* h02 = interpolate(p[0].y, p[0].h, p[2].y, p[2].h, &h02_count);

    // Handle allocation failures - free any successfully allocated memory
    if (!x01 || !h01 || !x12 || !h12 || !x02 || !h02) {
        fprintf(stderr, "Error: Memory allocation failed during triangle interpolation.\n");
        free(x01); free(h01); free(x12); free(h12); free(x02); free(h02);
        return;
    }

    // 3. Concatenate the short sides (x01 and x12, AND h01 and h12)
    // Remove the last element of x01/h01 to avoid duplication at y1
    if (x01_count > 0) { // Only if x01 has elements, remove one
        x01_count--;
        h01_count--; // h01_count should match x01_count
    }
    
    int x012_count = x01_count + x12_count;
    float* x012 = (float*)malloc(x012_count * sizeof(float));
    if (!x012) {
        fprintf(stderr, "Error: Memory allocation failed for x012.\n");
        free(x01); free(h01); free(x12); free(h12); free(x02); free(h02);
        return;
    }
    memcpy(x012, x01, x01_count * sizeof(float));
    memcpy(x012 + x01_count, x12, x12_count * sizeof(float));

    int h012_count = h01_count + h12_count; // Should be same as x012_count
    float* h012 = (float*)malloc(h012_count * sizeof(float));
    if (!h012) {
        fprintf(stderr, "Error: Memory allocation failed for h012.\n");
        free(x01); free(h01); free(x12); free(h12); free(x02); free(h02);
        free(x012); // Free x012 if h012 fails
        return;
    }
    memcpy(h012, h01, h01_count * sizeof(float));
    memcpy(h012 + h01_count, h12, h12_count * sizeof(float));

    // Free the intermediate arrays (x01, h01, x12, h12) as they are no longer needed
    free(x01); free(h01);
    free(x12); free(h12);

    // 4. Determine which is left and which is right (x_left_arr, x_right_arr, h_left_arr, h_right_arr)
    float* x_left_arr;
    float* x_right_arr;
    float* h_left_arr;
    float* h_right_arr;

    // Use a middle point to compare; ensure m is within bounds for both arrays
    int m = (int)floorf(x012_count / 2.0f); // Index for the middle Y-value

    // Clamp m to be within x02_count as well
    if (m >= x02_count) m = x02_count - 1;
    if (m < 0) m = 0; // Handle very small triangles

    if (x02[m] < x012[m]) {
        x_left_arr = x02;
        h_left_arr = h02;
        x_right_arr = x012;
        h_right_arr = h012;
    } else {
        x_left_arr = x012;
        h_left_arr = h012;
        x_right_arr = x02;
        h_right_arr = h02;
    }

    // 5. Draw the horizontal segments
    int y_start = (int)roundf(p[0].y); // Lowest Y (geometric)
    int y_end = (int)roundf(p[2].y);   // Highest Y (geometric)

    for (int y_geo = y_start; y_geo <= y_end; ++y_geo) {
        // Calculate the index for current y_geo in the interpolated arrays
        int array_idx = y_geo - y_start;

        // Ensure array_idx is within bounds. This check is crucial because of
        // potential rounding differences and the way interpolate calculates counts.
        // It's possible for x02_count/h02_count to be slightly different from x012_count/h012_count
        // especially near the middle point or for very thin triangles.
        // We take the minimum count as a safe upper bound for array_idx.
        int max_idx_left = x_left_arr == x02 ? x02_count : x012_count;
        int max_idx_right = x_right_arr == x02 ? x02_count : x012_count;

        if (array_idx >= max_idx_left || array_idx >= max_idx_right) {
             // This can happen if the floating point arithmetic for y_geo ends up
             // outside the calculated range of the interpolated arrays.
             // For robustness, skip this y_geo.
             continue;
        }

        // Get the geometric x-coordinates and h values for the current horizontal line
        float x_left_geo = x_left_arr[array_idx];
        float h_left_this_y = h_left_arr[array_idx];
        float x_right_geo = x_right_arr[array_idx];
        float h_right_this_y = h_right_arr[array_idx];

        // Ensure x_left_geo is actually less than or equal to x_right_geo after interpolation
        // If they swap, swap their corresponding h values too.
        if (x_left_geo > x_right_geo) {
            float temp_x = x_left_geo;
            x_left_geo = x_right_geo;
            x_right_geo = temp_x;

            float temp_h = h_left_this_y;
            h_left_this_y = h_right_this_y;
            h_right_this_y = temp_h;
        }

        // Interpolate h values across the horizontal segment
        int h_segment_count;
        float* h_segment = interpolate(x_left_geo, h_left_this_y,
                                       x_right_geo, h_right_this_y, &h_segment_count);
        
        if (!h_segment) {
            fprintf(stderr, "Error: Memory allocation failed for h_segment at y_geo=%d.\n", y_geo);
            continue; // Skip this scanline if interpolation fails
        }


        // Transform geometric (x_geo, y_geo) to SDL pixel (x_pixel, y_pixel)
        int y_pixel = (int)roundf(app->canvas_height / 2.0f - y_geo); // Y-axis flip for SDL

        int start_x_pixel = (int)roundf(x_left_geo + app->canvas_width / 2.0f);
        int end_x_pixel = (int)roundf(x_right_geo + app->canvas_width / 2.0f);

        // Iterate through x coordinates for the current horizontal segment
        for (int x_pixel_iter = 0; x_pixel_iter < h_segment_count; ++x_pixel_iter) {
            int current_x_pixel = start_x_pixel + x_pixel_iter;

            // Clamp h to [0.0, 1.0] just in case interpolation goes slightly out of range
            float current_h = fmaxf(0.0f, fminf(1.0f, h_segment[x_pixel_iter]));

            // Compute the shaded color
            unsigned char r = get_red(base_color) * current_h;
            unsigned char g = get_green(base_color) * current_h;
            unsigned char b = get_blue(base_color) * current_h;
            unsigned int shaded_color = make_color(r, g, b);

            put_pixel(app, current_x_pixel, y_pixel, shaded_color);
        }
        free(h_segment); // Free h_segment for the current scanline
    }

    // 6. Free all dynamically allocated memory for edges
    free(x02);
    free(h02);
    free(x012);
    free(h012);
}


void application_loop(Application* app) {
    if (app == NULL) {
        fprintf(stderr, "Error: application_loop received NULL pointer for app.\n");
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
        clear_canvas(app, COLOR_BLACK);

        // Example 1: A blue shaded triangle
        Point2D tri1_p0 = {-200.0f, -250.0f, 0.2f}; // Low intensity
        Point2D tri1_p1 = {200.0f, 50.0f, 1.0f};    // Full intensity
        Point2D tri1_p2 = {20.0f, 250.0f, 0.5f};    // Medium intensity
        draw_shaded_triangle(app, tri1_p0, tri1_p1, tri1_p2, COLOR_BLUE);
        
        // Example 2: A green shaded triangle, offset
        Point2D tri2_p0 = {-100.0f, -100.0f, 0.1f};
        Point2D tri2_p1 = {150.0f, 100.0f, 0.9f};
        Point2D tri2_p2 = {-50.0f, 200.0f, 0.4f};
        // Offset this triangle to the right
        float offset_x = 200.0f;
        float offset_y = -150.0f;
        tri2_p0.x += offset_x; tri2_p0.y += offset_y;
        tri2_p1.x += offset_x; tri2_p1.y += offset_y;
        tri2_p2.x += offset_x; tri2_p2.y += offset_y;
        draw_shaded_triangle(app, tri2_p0, tri2_p1, tri2_p2, COLOR_GREEN);

        // Example 3: A red shaded triangle, demonstrating `h = 1.0` for solid color
        Point2D tri3_p0 = {-150.0f, 0.0f, 1.0f};
        Point2D tri3_p1 = {150.0f, 0.0f, 1.0f};
        Point2D tri3_p2 = {0.0f, 200.0f, 1.0f};
        float offset3_x = -200.0f;
        float offset3_y = -150.0f;
        tri3_p0.x += offset3_x; tri3_p0.y += offset3_y;
        tri3_p1.x += offset3_x; tri3_p1.y += offset3_y;
        tri3_p2.x += offset3_x; tri3_p2.y += offset3_y;
        draw_shaded_triangle(app, tri3_p0, tri3_p1, tri3_p2, COLOR_RED);

        update_canvas_texture(app); // Push changes to the screen

        // --- Frame Rate Capping Logic ---
        frameTime = SDL_GetTicks() - frameStart;

        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void application_exit(Application* app) {
    (void)app; // Suppress unused parameter warning
    SDL_Quit(); // Global SDL cleanup
}

void application_clean_up(Application* app) {
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

    printf("Application cleanup complete.\n");
}
