#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h> // For bool type
#include <float.h> // For FLT_MAX
#include <math.h>  // For powf and sqrtf

// Assuming these headers exist and define necessary structs/functions:
#include "../canvas/canvas.h"
#include "../scene/scene.h"
#include "../camera/camera.h"
#include "../color/color.h"
#include "../object/object.h"
#include "../vector/vector.h"

#ifndef ENGINE_H
#define ENGINE_H

/**
 * @brief Represents the core rendering engine.
 */
typedef struct Engine {
    SDL_Renderer* renderer;        ///< SDL Renderer for drawing operations.
    Color background_color;        ///< Background color of the scene.
} Engine;

/**
 * @brief Structure to hold the two roots of a quadratic equation,
 * typically used for ray-object intersections.
 */
typedef struct IntersectionRoots {
    float root1;
    float root2;
} IntersectionRoots;

/**
 * @brief Initializes the rendering engine.
 * @param engine Pointer to the Engine struct to initialize.
 * @param window Pointer to the SDL_Window.
 * @param canvas Pointer to the Canvas struct.
 * @return 0 on success, 1 on failure.
 */
int engine_init(Engine* engine, SDL_Window* window, Canvas* canvas);

/**
 * @brief Renders the entire scene from the camera's perspective onto the canvas.
 * @param engine Pointer to the Engine struct.
 * @param camera Pointer to the Camera struct.
 * @param scene Pointer to the Scene struct containing objects and lights.
 * @param canvas Pointer to the Canvas struct.
 */
void engine_render(Engine* engine, const Camera* camera, const Scene* scene, const Canvas* canvas);

/**
 * @brief Draws a single pixel on the canvas using the engine's renderer.
 * Handles coordinate system conversion from viewport to SDL.
 * @param engine Pointer to the Engine struct.
 * @param canvas Pointer to the Canvas struct.
 * @param color Pointer to the Color of the pixel.
 * @param x X-coordinate in viewport space.
 * @param y Y-coordinate in viewport space.
 */
void engine_draw_pixel(Engine* engine, const Canvas* canvas, const Color* color, int x, int y);

/**
 * @brief Cleans up resources used by the engine.
 * @param engine Pointer to the Engine struct.
 */
void engine_clean_up(Engine* engine);

/**
 * @brief Traces a ray into the scene to determine the color of the intersected object.
 * @param camera Pointer to the Camera from which the ray originates.
 * @param scene Pointer to the Scene containing objects and lights.
 * @param ray_direction Direction vector of the ray.
 * @param background_color The color to return if no object is hit.
 * @return The computed color of the pixel.
 */
Color engine_trace_ray(const Camera* camera, const Scene* scene, Vector3 ray_direction, Color background_color);

/**
 * @brief Computes the total light intensity at a given surface point.
 * @param lights_list Pointer to the list of lights in the scene.
 * @param surface_point The 3D point on the object's surface.
 * @param surface_normal The normal vector at the surface_point.
 * @param specular_exponent The specular exponent of the material.
 * @param view_direction Direction vector from the surface point to the camera.
 * @return The total light intensity.
 */
float render_compute_light(const LightList* lights_list, Vector3 surface_point, Vector3 surface_normal, int specular_exponent, Vector3 view_direction);

/**
 * @brief Calculates the intersection points of a ray with a sphere.
 * @param ray_origin The origin of the ray.
 * @param ray_direction The direction of the ray (should be normalized).
 * @param sphere_object The sphere object to intersect with.
 * @return An IntersectionRoots struct containing the two intersection 't' values.
 * If no intersection, roots will be FLT_MAX.
 */
IntersectionRoots render_ray_sphere_intersection(Vector3 ray_origin, Vector3 ray_direction, const Object sphere_object);

#endif // ENGINE_H
