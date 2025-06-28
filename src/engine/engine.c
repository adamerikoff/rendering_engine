#include "./engine.h"

#define EPSILON 0.05f

// Assuming canvas_to_viewport, vector3_add, vector3_scale, vector3_subtract,
// vector3_normalize, vector3_dot, color_new are defined in their respective headers.

/**
 * @brief Initializes the rendering engine.
 * @param engine Pointer to the Engine struct to initialize.
 * @param window Pointer to the SDL_Window.
 * @param canvas Pointer to the Canvas struct.
 * @return 0 on success, 1 on failure.
 */
int engine_init(Engine* engine, SDL_Window* window, Canvas* canvas) {
    if (!engine || !window || !canvas) {
        fprintf(stderr, "Error: NULL pointer passed to engine_init.\n");
        return 1;
    }

    engine->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!engine->renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        // No need to destroy window/quit SDL here, it should be handled by the caller
        return 1;
    }

    return 0;
}

/**
 * @brief Renders the entire scene from the camera's perspective onto the canvas.
 * @param engine Pointer to the Engine struct.
 * @param camera Pointer to the Camera struct.
 * @param scene Pointer to the Scene struct containing objects and lights.
 * @param canvas Pointer to the Canvas struct.
 */
void engine_render(Engine* engine, const Camera* camera, const Scene* scene, const Canvas* canvas) {
    if (!engine || !camera || !scene || !canvas) {
        fprintf(stderr, "Error: NULL pointer passed to engine_render.\n");
        return;
    }

    // Set background color for clearing
    SDL_SetRenderDrawColor(engine->renderer, scene->background_color.r, scene->background_color.g, scene->background_color.b, 255);
    SDL_RenderClear(engine->renderer);

    // Calculate half dimensions for clearer loop bounds
    const int canvas_half_width = canvas->width / 2;
    const int canvas_half_height = canvas->height / 2;

    // Iterate over each pixel in the canvas (viewport coordinates)
    for (int pixel_x = -canvas_half_width; pixel_x < canvas_half_width; ++pixel_x) {
        for (int pixel_y = -canvas_half_height; pixel_y < canvas_half_height; ++pixel_y) {
            // Calculate ray direction from camera through the current pixel on the viewport
            Vector3 ray_direction = canvas_to_viewport(camera, canvas, pixel_x, pixel_y);

            // Trace the ray to find the color of the pixel
            Color pixel_color = engine_trace_ray(camera->position, scene, ray_direction, 3, EPSILON, FLT_MAX);

            // Draw the computed color at the pixel location
            engine_draw_pixel(engine, canvas, &pixel_color, pixel_x, pixel_y);
        }
    }

    // Display rendered frame
    SDL_RenderPresent(engine->renderer);
}


Vector3 engine_reflect_ray(Vector3 ray_direction, Vector3 normal) {
    // R = I - 2(N·I)N
    return vector3_subtract(
        vector3_scale(normal, 2.0f * vector3_dot(ray_direction, normal)),
        ray_direction
    );
}

/**
 * @brief Traces a ray into the scene to determine the color of the intersected object.
 * @param camera Pointer to the Camera from which the ray originates.
 * @param scene Pointer to the Scene containing objects and lights.
 * @param ray_direction Direction vector of the ray.
 * @param background_color The color to return if no object is hit.
 * @return The computed color of the pixel.
 */
Color engine_trace_ray(Vector3 origin, const Scene* scene, Vector3 ray_direction, int recursion_depth, float t_min, float t_max) {
    if (!scene) {
        fprintf(stderr, "Error: NULL scene passed to engine_trace_ray.\n");
        return scene->background_color; // Return background color on error
    }

    ClosestIntersection closest_intersection = engine_calculate_closest_intersection(scene->objects, origin, ray_direction, t_min, t_max);

    if (closest_intersection.closest_object == NULL) {
        return scene->background_color;
    }

    // Calculate the exact 3D point where the ray hit the object
    Vector3 intersection_point = vector3_add(origin, vector3_scale(ray_direction, closest_intersection.closest_t));

    // Calculate the surface normal at the intersection point.
    // For a sphere, the normal is simply (intersection_point - sphere_center) normalized.
    Vector3 surface_normal =  vector3_normalize(vector3_subtract(intersection_point, closest_intersection.closest_object->position));

    // The view direction is the inverse of the ray direction from the camera
    Vector3 view_direction = vector3_scale(ray_direction, -1.0f);

    // Compute the total light intensity at the intersection point
    float light_intensity = engine_compute_light(scene, intersection_point, surface_normal, closest_intersection.closest_object->specularity, view_direction);

    // Return the object's color multiplied by the calculated light intensity
    Color local_color = color_new(
        (unsigned char)(closest_intersection.closest_object->color.r * light_intensity),
        (unsigned char)(closest_intersection.closest_object->color.g * light_intensity),
        (unsigned char)(closest_intersection.closest_object->color.b * light_intensity)
    );

    if (recursion_depth <= 0 || closest_intersection.closest_object->reflectivity <= 0) {
        return local_color;
    }

    Vector3 reflected_ray = engine_reflect_ray(view_direction, surface_normal);

    Color reflected_color = engine_trace_ray(intersection_point, scene, reflected_ray, recursion_depth - 1, EPSILON, FLT_MAX);

    return color_new(
        (local_color.r * (1 - closest_intersection.closest_object->reflectivity)) + (reflected_color.r * closest_intersection.closest_object->reflectivity),
        (local_color.g * (1 - closest_intersection.closest_object->reflectivity)) + (reflected_color.g * closest_intersection.closest_object->reflectivity),
        (local_color.b * (1 - closest_intersection.closest_object->reflectivity)) + (reflected_color.b * closest_intersection.closest_object->reflectivity)
    );
}

/**
 * @brief Computes the total light intensity at a given surface point.
 * @param lights_list Pointer to the list of lights in the scene.
 * @param surface_point The 3D point on the object's surface.
 * @param surface_normal The normal vector at the surface_point.
 * @param specular_exponent The specular exponent of the material.
 * @param view_direction Direction vector from the surface point to the camera.
 * @return The total light intensity.
 */
float engine_compute_light(const Scene* scene, Vector3 surface_point, Vector3 surface_normal, int specular_exponent, Vector3 view_direction) {
    if (!scene->lights) {
        fprintf(stderr, "Error: NULL lights_list passed to engine_compute_light.\n");
        return 0.0f;
    }

    float total_intensity = 0.0f;
    float t_max = 0.0;

    Vector3 normalized_view_direction = vector3_normalize(view_direction);

    for (size_t i = 0; i < scene->lights->count; ++i) {
        const Light* current_light = &(scene->lights->lights[i]); 

        Vector3 light_direction;

        switch (current_light->type) {
            case LIGHT_TYPE_AMBIENT:
                total_intensity += current_light->intensity;
                continue;
            case LIGHT_TYPE_POINT:
                light_direction = vector3_subtract(current_light->data.pointData.position, surface_point);
                t_max = vector3_magnitude(light_direction);
                break;
            case LIGHT_TYPE_DIRECTIONAL:
                light_direction = vector3_scale(current_light->data.directionalData.direction, 1.0f);
                t_max = FLT_MAX;
                break;
            default:
                fprintf(stderr, "Warning: Unknown Light Type encountered in engine_compute_light.\n");
                continue;;
        }

        // Normalize light direction
        Vector3 normalized_light_direction = vector3_normalize(light_direction);

        ClosestIntersection closest_intersection = engine_calculate_closest_intersection(scene->objects, surface_point, normalized_light_direction, EPSILON, t_max);

        if (closest_intersection.closest_object != NULL) {
            continue;
        }

        // --- DIFFUSE LIGHT (Lambertian Reflection) ---
        // N · L (Normal dot Light direction)
        float diffuse_dot_product = vector3_dot(surface_normal, normalized_light_direction);

        if (diffuse_dot_product > 0) {
            total_intensity += current_light->intensity * diffuse_dot_product;
        }

        // --- SPECULAR LIGHT (Phong Reflection Model) ---
        if (specular_exponent > 0) {
            // Calculate reflection vector R = 2 * (N · L) * N - L
            Vector3 reflection_vector = vector3_subtract(
                vector3_scale(surface_normal, 2.0f * diffuse_dot_product), normalized_light_direction
            );
            reflection_vector = vector3_normalize(reflection_vector);
            // Compute R · V (Reflection dot View)
            float reflection_dot_view = vector3_dot(reflection_vector, normalized_view_direction);
            if (reflection_dot_view > 0) {
                total_intensity += current_light->intensity * powf(reflection_dot_view, specular_exponent);
            }
        }
    }

    return fmin(total_intensity, 1.0f); 
}

ClosestIntersection engine_calculate_closest_intersection(ObjectList* objects, Vector3 ray_origin, Vector3 ray_direction, float t_min, float t_max) {
    ClosestIntersection closest_intersection = { NULL, FLT_MAX};

    for (int i = 0; i < objects->count; ++i) {
        Object* current_object = &(objects->objects[i]);
        IntersectionRoots roots = engine_ray_sphere_intersection(ray_origin, ray_direction, *current_object);

        // Check if root1 is a valid intersection and closer than previous
        if (roots.root1 > t_min && roots.root1 < t_max && roots.root1 < closest_intersection.closest_t) {
            closest_intersection.closest_t = roots.root1;
            closest_intersection.closest_object = current_object;
        }
        // Check if root2 is a valid intersection and closer than previous
        if (roots.root2 > t_min && roots.root2 < t_max && roots.root2 < closest_intersection.closest_t) {
            closest_intersection.closest_t = roots.root2;
            closest_intersection.closest_object = current_object;
        }
    }

    return closest_intersection;
}

/**
 * @brief Calculates the intersection points of a ray with a sphere.
 * @param ray_origin The origin of the ray.
 * @param ray_direction The direction of the ray (should be normalized).
 * @param sphere_object The sphere object to intersect with.
 * @return An IntersectionRoots struct containing the two intersection 't' values.
 * If no intersection, roots will be FLT_MAX.
 */
IntersectionRoots engine_ray_sphere_intersection(Vector3 ray_origin, Vector3 ray_direction, const Object sphere_object) {
    IntersectionRoots intersection_t_values = {FLT_MAX, FLT_MAX};

    // Vector from sphere center to ray origin: L = O - C
    Vector3 origin_to_sphere_center = vector3_subtract(ray_origin, sphere_object.position);

    // Quadratic equation coefficients: At^2 + Bt + C = 0
    // A = D · D (ray_direction dot ray_direction). If ray_direction is normalized, A = 1.
    // B = 2 * (L · D)
    // C = (L · L) - R^2
    const float a_coeff = vector3_dot(ray_direction, ray_direction);
    const float b_coeff = 2.0f * vector3_dot(origin_to_sphere_center, ray_direction);
    const float c_coeff = vector3_dot(origin_to_sphere_center, origin_to_sphere_center) - (sphere_object.data.sphereData.radius * sphere_object.data.sphereData.radius);

    const float discriminant = (b_coeff * b_coeff) - (4.0f * a_coeff * c_coeff);

    if (discriminant < 0) {
        // No real solutions, ray misses the sphere
        return intersection_t_values;
    }

    // Calculate the two possible 't' values
    // Using const for calculated values
    const float sqrt_discriminant = sqrtf(discriminant);
    const float inv_2a = 1.0f / (2.0f * a_coeff);

    intersection_t_values.root1 = (-b_coeff - sqrt_discriminant) * inv_2a;
    intersection_t_values.root2 = (-b_coeff + sqrt_discriminant) * inv_2a;

    // Ensure root1 is always the smaller value
    if (intersection_t_values.root1 > intersection_t_values.root2) {
        float temp = intersection_t_values.root1;
        intersection_t_values.root1 = intersection_t_values.root2;
        intersection_t_values.root2 = temp;
    }

    return intersection_t_values;
}

/**
 * @brief Draws a single pixel on the canvas using the engine's renderer.
 * Handles coordinate system conversion from viewport to SDL.
 * @param engine Pointer to the Engine struct.
 * @param canvas Pointer to the Canvas struct.
 * @param color Pointer to the Color of the pixel.
 * @param x X-coordinate in viewport space.
 * @param y Y-coordinate in viewport space.
 */
void engine_draw_pixel(Engine* engine, const Canvas* canvas, const Color* color, int x, int y) {
    if (!engine || !canvas || !color) {
        fprintf(stderr, "Error: NULL pointer passed to engine_draw_pixel.\n");
        return;
    }

    // Set the drawing color
    SDL_SetRenderDrawColor(engine->renderer, color->r, color->g, color->b, 255);

    // Convert viewport coordinates (centered) to SDL screen coordinates (top-left origin)
    int sdl_x = (canvas->width / 2) + x;
    // SDL y-coordinates increase downwards, so we invert the viewport y and adjust for 0-indexing
    int sdl_y = (canvas->height / 2) - y - 1;

    // Perform boundary checks before drawing to prevent out-of-bounds access
    if (sdl_x >= 0 && sdl_x < canvas->width &&
        sdl_y >= 0 && sdl_y < canvas->height) {
        SDL_RenderDrawPoint(engine->renderer, sdl_x, sdl_y);
    } else {
        // This might indicate an issue with coordinate calculation or an oversized viewport
        // For debugging, you might want to print this, but for release, it might be too noisy.
        fprintf(stderr, "Warning: Pixel coordinates out of canvas bounds: (%d, %d)\n", sdl_x, sdl_y);
    }
}

/**
 * @brief Cleans up resources used by the engine.
 * @param engine Pointer to the Engine struct.
 */
void engine_clean_up(Engine* engine) {
    if (engine && engine->renderer) {
        SDL_DestroyRenderer(engine->renderer);
        engine->renderer = NULL; // Prevent double-free
    }
    // SDL_Quit() and SDL_DestroyWindow() should be handled outside,
    // where SDL was initialized and the window created.
}
