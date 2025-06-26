#include "camera.h"

// Creates a new Camera instance
Camera camera_new(Vector3 position, float projection_plane_z, float viewport_size) {
    Camera camera;
    camera.position = position;
    camera.viewport.projection_plane_z = projection_plane_z;
    camera.viewport.viewport_size = viewport_size;

    camera.viewport.viewport_width = 1.0f;
    camera.viewport.viewport_height = 1.0f;
    return camera;
}

void camera_cleanup(Camera* camera) {
    (void)camera;
}

// Converts 2D canvas coordinates to 3D viewport coordinates.
Vector3 canvas_to_viewport(const Camera* camera, const Canvas* canvas, int x, int y) {
    // Ensure camera is not NULL and canvas dimensions are valid
    if (camera == NULL || canvas->width <= 0 || canvas->height <= 0) {
        return vector3_new(0.0f, 0.0f, 0.0f); // Return a zero vector or handle error
    }

    // Convert canvas (x, y) from screen space to normalized device coordinates, then to viewport space
    // Assuming (0,0) is top-left for canvas, and origin is center for viewport
    float vp_x = (float)(x - canvas->width / 2) * camera->viewport.viewport_size / canvas->width;
    float vp_y = (float)(canvas->height / 2 - y) * camera->viewport.viewport_size / canvas->height; // Invert Y for typical Cartesian
    float vp_z = camera->viewport.projection_plane_z;

    return vector3_new(vp_x, vp_y, vp_z);
}
