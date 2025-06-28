#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>

#include "../colors.h"
#include "../geometry/geometry.h"


#ifndef _APP_H_
#define _APP_H_

// Define the Application struct
typedef struct Application {
    SDL_Window* window;
    SDL_Renderer* renderer; // Add an SDL renderer
    SDL_Texture* canvas_texture; // This will represent our software canvas
    unsigned int* pixel_buffer; // Raw pixel data for our software canvas
    int canvas_width;
    int canvas_height;
    int is_running;
} Application;

// --- Application lifecycle functions ---
int application_init(Application* app, int width, int height);
void application_loop(Application* app);
void application_clean_up(Application* app);
void application_exit(Application* app); // SDL_Quit call

// --- Canvas operations (our PutPixel) ---
// Note: This put_pixel works with *SDL pixel coordinates* (top-left 0,0, Y-down)
void put_pixel(Application* app, int x, int y, unsigned int color);
void clear_canvas(Application* app, unsigned int color);
void update_canvas_texture(Application* app); // Transfers pixel_buffer to texture

// --- Drawing primitives based on the article ---
// Interpolate function (returns a dynamically allocated float array)
// It's the caller's responsibility to free the returned array.
float* interpolate(float i0, float d0, float i1, float d1, int* out_count);

// draw_line function from the article (uses our put_pixel)
// P0 and P1 are in *true geometrical coordinates* (centered, Y-up)
void draw_line(Application* app, Point2D P0, Point2D P1, unsigned int color);

// Helper function for swapping points
void swap_points(Point2D* p1, Point2D* p2);

#endif
