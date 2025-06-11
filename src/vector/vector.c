#include "vector.h"
#include <math.h>

Vector3 Vector3Sum(Vector3 vec1, Vector3 vec2) {
    return (Vector3) {
        .x = vec1.x + vec2.x,
        .y = vec1.y + vec2.y,
        .z = vec1.z + vec2.z,
    };
}

Vector3 Vector3Scalar(Vector3 vec1, float scalar) {
    return (Vector3) {
        .x = vec1.x * scalar,
        .y = vec1.y * scalar,
        .z = vec1.z * scalar,
    };
}

Vector3 Vector3Difference(Vector3 vec1, Vector3 vec2) {
    return Vector3Sum(vec1, Vector3Scalar(vec2, -1.0));
}

Vector3 Vector3HadamardProduct(Vector3 vec1, Vector3 vec2) {
    return (Vector3) {
        .x = vec1.x * vec2.x,
        .y = vec1.y * vec2.y,
        .z = vec1.z * vec2.z,
    };
} 

float Vector3GrandSum(Vector3 vec) {
    return vec.x + vec.y + vec.z;
}

float Vector3Dot(Vector3 vec1, Vector3 vec2) {
    return Vector3GrandSum(Vector3HadamardProduct(vec1, vec2));
}

float Vector3Length(Vector3 v) {
    return sqrtf(Vector3Dot(v, v));
}

Vector3 Vector3Normalize(Vector3 v) {
    float len = Vector3Length(v);
    if (len > 1e-6f) {
        return Vector3Scalar(v, 1.0f / len);
    }
    return (Vector3){0.0f, 0.0f, 0.0f};
}