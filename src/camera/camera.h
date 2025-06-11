#pragma once

#include "../vector/vector.h"

/**
 * @brief Represents the virtual camera in the 3D scene.
 */
typedef struct Camera {
    Vector3 position;      // Camera's position in world space (e.g., {0,0,0})
    float viewportWidth;  // Viewport width in world units (Vw)
    float viewportHeight; // Viewport height in world units (Vh)
    float d;            // Distance from camera to viewport (Vz = d)
} Camera;

/**
 * @brief Cleans up camera-specific resources. (Currently empty, but good for future)
 * @param camera A pointer to the Camera struct to clean up.
 */
void Camera_Cleanup(Camera* camera);

/**
 * @brief Calculates the 3D point on the viewport corresponding to a 2D pixel on the canvas.
 * Assumes camera is at origin (0,0,0) looking along +Z.
 *
 * @param camera A pointer to the Camera struct with viewport definitions.
 * @param x_pixel The X-coordinate of the pixel on the canvas (from top-left).
 * @param y_pixel The Y-coordinate of the pixel on the canvas (from top-left).
 * @param canvas_width The total width of the canvas (window) in pixels.
 * @param canvas_height The total height of the canvas (window) in pixels.
 * @return A Vec3 representing the 3D point on the viewport.
 */
Vector3 Camera_GetViewportPointForPixel(const Camera* camera, int x_pixel, int y_pixel, int full_canvas_width, int full_canvas_height);
