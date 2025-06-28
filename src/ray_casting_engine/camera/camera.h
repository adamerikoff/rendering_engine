#pragma once

#include <stdio.h>
#include <math.h>
#include "../vector/vector.h"
#include "../canvas/canvas.h"

#ifndef _CAMERA_H_
#define _CAMERA_H_

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679f // Pi / 2
#endif

typedef struct ViewPort {
    float width;               // Viewport width in world units
    float height;              // Viewport height in world units
    float projection_plane_z;  // Distance from camera to projection plane
    float aspect_ratio;        // Width/height ratio (calculated automatically)
} ViewPort;

typedef struct Camera {
    Vector3 position;          // Camera's position in world space
    
    Vector3 forward;           // Camera's forward direction vector (normalized)
    Vector3 up;                // Camera's up direction vector (normalized)
    Vector3 right;             // Camera's right direction vector (normalized)

    float yaw;                 // Rotation around the world Y-axis (radians)
    float pitch;               // Rotation around the camera's local X-axis (radians)

    ViewPort viewport;         // Camera's viewport properties
} Camera;

// Creates a new Camera instance
// position: 3D position of the camera
// projection_plane_z: Distance to projection plane (recommended: 1.0)
// canvas: Pointer to the Canvas struct for aspect ratio
Camera camera_new(Vector3 position, float projection_plane_z, Canvas* canvas);

// Updates the camera's forward, up, and right vectors based on its yaw and pitch angles.
void camera_update_vectors(Camera* camera);

// Cleans up camera resources (currently placeholder)
void camera_cleanup(Camera* camera);

// Converts 2D canvas coordinates to 3D viewport coordinates in camera's LOCAL space.
Vector3 canvas_to_viewport(const Camera* camera, const Canvas* canvas, int x, int y);


#endif
