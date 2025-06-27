#include "camera.h"

// Creates a new Camera instance that adapts to any aspect ratio
Camera camera_new(Vector3 position, float projection_plane_z, Canvas* canvas) {
    Camera camera;
    camera.position = position;
    camera.viewport.projection_plane_z = projection_plane_z;
    
    // Calculate actual aspect ratio from screen dimensions
    float aspect_ratio = (float)canvas->width / (float)canvas->height;
    
    // Set viewport properties
    camera.viewport.height = 1.0f;
    camera.viewport.width = 1.0f * aspect_ratio;
    camera.viewport.aspect_ratio = aspect_ratio;
    
    return camera;
}

void camera_cleanup(Camera* camera) {
    (void)camera;
}

Vector3 canvas_to_viewport(const Camera* camera, const Canvas* canvas, int x, int y) {
    if (camera == NULL || canvas->width <= 0 || canvas->height <= 0) {
        return vector3_new(0.0f, 0.0f, 0.0f);
    }

    float vp_x = x * camera->viewport.width / canvas->width;
    float vp_y = y * camera->viewport.height / canvas->height;

    return vector3_new(vp_x, vp_y, camera->viewport.projection_plane_z);
}
