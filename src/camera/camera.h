#pragma once

#include "../vector/vector.h"

typedef struct Camera {
    Vector3 position;         // Camera's position in world space (e.g., {0,0,0})
    float viewport_width;      // Viewport width in world units (Vw)
    float viewport_height;     // Viewport height in world units (Vh)
    float projection_plane_z;  // Distance from camera to viewport (Vz = d)
    float viewport_size;
} Camera;

void Camera_Cleanup(Camera* camera);
Camera Camera_New(Vector3 position, float projection_plane_z, float viewport_size);

// Converts 2D canvas coordinates to 3D viewport coordinates.
Vector3 Camera_CanvasToViewport(Camera camera, int x, int y, int canvas_width, int canvas_height);
