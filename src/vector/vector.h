#pragma once

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

typedef struct IntersectionRoots {
    float root1;
    float root2;
} IntersectionRoots;

typedef struct Color {
    int r;
    int g;
    int b;
    int a;
} Color;

Vector3 Vector3Sum(Vector3 vec1, Vector3 vec2);
Vector3 Vector3Scalar(Vector3 vec1, float scalar);
Vector3 Vector3Difference(Vector3 vec1, Vector3 vec2);
Vector3 Vector3HadamardProduct(Vector3 vec1, Vector3 vec2);
Vector3 Vector3Normalize(Vector3 v);
float Vector3Length(Vector3 v);
float Vector3GrandSum(Vector3 vec);
float Vector3Dot(Vector3 vec1, Vector3 vec2);
