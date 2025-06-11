#include "camera.h"

void Camera_Cleanup(Camera* camera) {
    // Currently, there's no dynamically allocated memory within the Camera struct
    // (e.g., no pointers to malloc'd data).
    // If you were to add such resources (e.g., a dynamically sized array for camera path),
    // you would free them here.
    (void)camera; // Suppress unused parameter warning
}

// Change parameter names to reflect they are full dimensions
Vector3 Camera_GetViewportPointForPixel(const Camera* camera, int x_pixel, int y_pixel, int full_canvas_width, int full_canvas_height) {
    Vector3 viewport_point;

    // Calculate the scale factor for one pixel relative to the *full* canvas width/height
    // This maps the centered pixel (x_pixel + 0.5f) to the viewport space.
    float scale_x_per_pixel = camera->viewportWidth / (float)full_canvas_width;
    float scale_y_per_pixel = camera->viewportHeight / (float)full_canvas_height;

    // Map the centered pixel coordinates (e.g., -320 to 319) to viewport coordinates
    // (e.g., -Vw/2 to Vw/2)
    // For x_pixel = -full_canvas_width/2, viewport_point.x should be -camera->viewportWidth/2
    // For x_pixel = full_canvas_width/2 - 1, viewport_point.x should be close to camera->viewportWidth/2
    viewport_point.x = (x_pixel + 0.5f) * scale_x_per_pixel;
    viewport_point.y = (y_pixel + 0.5f) * scale_y_per_pixel;

    viewport_point.z = camera->d;

    return viewport_point;
}