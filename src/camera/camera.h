#pragma once

#include <stdio.h>
#include "../vector/vector.h"
#include "../canvas/canvas.h"

#ifndef _CAMERA_H_
#define _CAMERA_H_

typedef struct ViewPort {
    float width;               // Viewport width in world units
    float height;              // Viewport height in world units
    float projection_plane_z;  // Distance from camera to projection plane
    float aspect_ratio;        // Width/height ratio (calculated automatically)
} ViewPort;

typedef struct Camera {
    Vector3 position;          // Camera's position in world space
    ViewPort viewport;         // Camera's viewport properties
} Camera;

// Creates a new Camera instance
// position: 3D position of the camera
// projection_plane_z: Distance to projection plane (recommended: 1.0)
// viewport_size: Base size of viewport (height) in world units (recommended: 1.0)
Camera camera_new(Vector3 position, float projection_plane_z, Canvas* canvas);

// Cleans up camera resources (currently placeholder)
void camera_cleanup(Camera* camera);

// Converts 2D canvas coordinates to 3D viewport coordinates
Vector3 canvas_to_viewport(const Camera* camera, const Canvas* canvas, int x, int y);

#endif
