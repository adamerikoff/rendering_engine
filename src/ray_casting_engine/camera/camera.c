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
    
    // Initial orientation: looking down negative Z-axis.
    // For the `camera_update_vectors` formula below, a yaw of -M_PI_2 (or 270 degrees)
    // results in a forward vector of (0, 0, -1) when pitch is 0.
    camera.yaw = M_PI_2; // Start looking down -Z
    camera.pitch = 0.0f;  // No initial vertical tilt

    // Initialize vectors. These will be updated by camera_update_vectors.
    // Call camera_update_vectors to set the initial forward, up, and right vectors.
    camera_update_vectors(&camera); 
    
    return camera;
}

void camera_update_vectors(Camera* camera) {
    // Calculate the new Forward vector based on Yaw and Pitch
    // The formulas are derived from spherical coordinates or combined rotation matrices.
    // Yaw rotates around the world Y-axis. Pitch rotates around the camera's local X-axis (right).
    camera->forward.x = cosf(camera->yaw) * cosf(camera->pitch);
    camera->forward.y = sinf(camera->pitch);
    camera->forward.z = sinf(camera->yaw) * cosf(camera->pitch);
    camera->forward = vector3_normalize(camera->forward); // Ensure it's a unit vector

    // Calculate Right vector as cross product of Forward and World Up
    // Using a fixed world_up (0,1,0) helps prevent "gimbal lock" for simple FPS cameras.
    Vector3 world_up = vector3_new(0.0f, 1.0f, 0.0f);
    camera->right = vector3_normalize(vector3_cross(camera->forward, world_up));

    // Calculate true Up vector as cross product of Right and Forward
    // This ensures Up is perpendicular to Forward and Right, forming an orthonormal basis.
    camera->up = vector3_normalize(vector3_cross(camera->right, camera->forward));
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
