#include "camera.h"

Camera Camera_New(Vector3 position, float projection_plane_z, float viewport_size) {
    Camera camera;

    camera.position = position;
    camera.projection_plane_z = projection_plane_z;
    camera.viewport_size = viewport_size;

    return camera;
}

void Camera_Cleanup(Camera* camera) {
    // Currently, there's no dynamically allocated memory within the Camera struct
    // (e.g., no pointers to malloc'd data).
    // If you were to add such resources (e.g., a dynamically sized array for camera path),
    // you would free them here.
    (void)camera; // Suppress unused parameter warning
}

// Change parameter names to reflect they are full dimensions
Vector3 Camera_CanvasToViewport(Camera camera, int x, int y, int canvas_width, int canvas_height) {
    return Vector3_New(
        x * camera.viewport_size / canvas_width,
        y * camera.viewport_size / canvas_height,
        camera.projection_plane_z
    );
}