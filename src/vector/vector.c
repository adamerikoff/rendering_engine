#include "vector.h"

// Constructor
Vector3 vector3_new(float x, float y, float z) {
    Vector3 v = {x, y, z};
    return v;
}

// Vector Addition
Vector3 vector3_add(Vector3 a, Vector3 b) {
    Vector3 result = {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
    return result;
}

// Vector Subtraction
Vector3 vector3_subtract(Vector3 a, Vector3 b) {
    Vector3 result = {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
    return result;
}

// Vector Scaling
Vector3 vector3_scale(Vector3 vec, float scalar) {
    Vector3 result = {
        vec.x * scalar,
        vec.y * scalar,
        vec.z * scalar
    };
    return result;
}

// Cross Product
Vector3 vector3_cross(Vector3 a, Vector3 b) {
    Vector3 result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return result;
}

// Normalize Vector
Vector3 vector3_normalize(Vector3 vec) {
    float magnitude = vector3_magnitude(vec);
    if (magnitude > 0.0f) { // Avoid division by zero for zero vectors
        Vector3 result = {
            vec.x / magnitude,
            vec.y / magnitude,
            vec.z / magnitude
        };
        return result;
    } else {
        // For a zero vector, return a zero vector
        return vector3_new(0.0f, 0.0f, 0.0f);
    }
}

// Dot Product
float vector3_dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Squared Magnitude (Length Squared)
float vector3_magnitude_sq(Vector3 vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

// Magnitude (Length)
float vector3_magnitude(Vector3 vec) {
    return sqrtf(vector3_magnitude_sq(vec)); // Using sqrtf for float precision
}
