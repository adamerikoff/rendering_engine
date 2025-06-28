#pragma once

#include "math.h"

#ifndef _VECTOR_H_
#define _VECTOR_H_

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

Vector3 vector3_new(float x, float y, float z);
Vector3 vector3_add(Vector3 a, Vector3 b);
Vector3 vector3_subtract(Vector3 a, Vector3 b);
Vector3 vector3_scale(Vector3 vec, float scalar);
Vector3 vector3_cross(Vector3 a, Vector3 b);
Vector3 vector3_normalize(Vector3 vec);
Vector3 vector3_multiply(Vector3 a, Vector3 b);
float vector3_dot(Vector3 a, Vector3 b);
float vector3_magnitude_sq(Vector3 vec);
float vector3_magnitude(Vector3 vec);

#endif
