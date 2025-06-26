#pragma once

#include <stdio.h>

#include "../vector/vector.h"
#include "../canvas/canvas.h"


#ifndef _CAMERA_H_
#define _CAMERA_H_

typedef struct ViewPort {
    float viewport_width;      // Viewport width in world units (Vw)
    float viewport_height;     // Viewport height in world units (Vh)
    float projection_plane_z;  // Distance from camera to viewport (Vz = d)
    float viewport_size;
} ViewPort;

typedef struct Camera {
    Vector3 position;         // Camera's position in world space (e.g., {0,0,0})
    ViewPort viewport;
} Camera;

Camera camera_new(Vector3 position, float projection_plane_z, float viewport_size);
void camera_cleanup(Camera* camera);

// Converts 2D canvas coordinates to 3D viewport coordinates.
Vector3 canvas_to_viewport(const Camera* camera, const Canvas* canvas, int x, int y);

#endif
