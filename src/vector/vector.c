#include "vector.h"
#include <math.h>

// ============================================================================
// VECTOR FUNCTIONS
// ============================================================================

/**
 * @brief Creates and returns a new Vector3.
 * @param x The x-component.
 * @param y The y-component.
 * @param z The z-component.
 * @return A new Vector3 initialized with the given components.
 */
Vector3 Vector3_New(float x, float y, float z) {
    Vector3 vec = {x, y, z};
    return vec;
}

/**
 * @brief Adds two Vector3 vectors.
 * @param a The first vector.
 * @param b The second vector.
 * @return A new Vector3 representing the sum (a + b).
 */
Vector3 Vector3_Add(Vector3 a, Vector3 b) {
    Vector3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

/**
 * @brief Subtracts one Vector3 vector from another.
 * @param a The first vector.
 * @param b The second vector.
 * @return A new Vector3 representing the difference (a - b).
 */
Vector3 Vector3_Subtract(Vector3 a, Vector3 b) {
    Vector3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

/**
 * @brief Multiplies a Vector3 by a scalar value.
 * @param vec The vector to scale.
 * @param scalar The scalar value.
 * @return A new Vector3 representing the scaled vector (vec * scalar).
 */
Vector3 Vector3_Scale(Vector3 vec, float scalar) {
    Vector3 result;
    result.x = vec.x * scalar;
    result.y = vec.y * scalar;
    result.z = vec.z * scalar;
    return result;
}

/**
 * @brief Calculates the dot product of two Vector3 vectors.
 * @param a The first vector.
 * @param b The second vector.
 * @return The scalar dot product of a and b.
 */
float Vector3_Dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/**
 * @brief Calculates the cross product of two Vector3 vectors.
 * @param a The first vector.
 * @param b The second vector.
 * @return A new Vector3 representing the cross product (a x b).
 */
Vector3 Vector3_Cross(Vector3 a, Vector3 b) {
    Vector3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

/**
 * @brief Calculates the squared magnitude (length) of a Vector3.
 * This is often preferred over magnitude to avoid sqrt for comparisons.
 * @param vec The vector.
 * @return The squared magnitude of the vector.
 */
float Vector3_MagnitudeSq(Vector3 vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

/**
 * @brief Calculates the magnitude (length) of a Vector3.
 * @param vec The vector.
 * @return The magnitude of the vector.
 */
float Vector3_Magnitude(Vector3 vec) {
    return sqrt(Vector3_MagnitudeSq(vec));
}

/**
 * @brief Normalizes a Vector3 (makes its magnitude 1).
 * @param vec The vector to normalize.
 * @return A new Vector3 representing the normalized vector.
 * Returns a zero vector if the input vector has zero magnitude.
 */
Vector3 Vector3_Normalize(Vector3 vec) {
    float magnitude = Vector3_Magnitude(vec);
    if (magnitude > 0) {
        return Vector3_Scale(vec, 1.0f / magnitude);
    } else {
        return Vector3_New(0.0f, 0.0f, 0.0f);
    }
}

/**
 * @brief Prints the components of a Vector3 to the console.
 * @param label A string label to print before the vector components.
 * @param vec The vector to print.
 */
void Vector3_Print(const char* label, Vector3 vec) {
    printf("%s: (%.2f, %.2f, %.2f)\n", label, vec.x, vec.y, vec.z);
}

// ============================================================================
// COLOR FUNCTIONS
// ============================================================================

/**
 * @brief Creates and returns a new Color.
 * @param r The red component (0-255).
 * @param g The green component (0-255).
 * @param b The blue component (0-255).
 * @param a The alpha component (0-255).
 * @return A new Color initialized with the given components.
 */
Color Color_New(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    Color color = {r, g, b, a};
    return color;
}