#pragma once

#include <stdlib.h>
#include <stdio.h>

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

Vector3 Vector3_New(float x, float y, float z);
Vector3 Vector3_Add(Vector3 a, Vector3 b);
Vector3 Vector3_Subtract(Vector3 a, Vector3 b);
Vector3 Vector3_Scale(Vector3 vec, float scalar);
Vector3 Vector3_Cross(Vector3 a, Vector3 b);
Vector3 Vector3_Normalize(Vector3 vec);
float Vector3_Dot(Vector3 a, Vector3 b);
float Vector3_MagnitudeSq(Vector3 vec);
float Vector3_Magnitude(Vector3 vec);
void Vector3_Print(const char* label, Vector3 vec);

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color;

Color Color_New(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void Color_Print(const char* label, Color color);