#include "object.h"
// ============================================================================
// OBJECT FUNCTIONS
// ============================================================================

/**
 * @brief Creates a new Cube Object.
 * @param center The center position of the cube.
 * @param color The color of the cube.
 * @param size The size (side length) of the cube.
 * @return A new Object representing a cube.
 */
Object Object_NewCube(Vector3 center, Color color, float size) {
    Object obj;
    obj.type = OBJECT_TYPE_CUBE;
    obj.position = center;
    obj.color = color;
    obj.data.cubeData.size = size;
    return obj;
}

/**
 * @brief Creates a new Sphere Object.
 * @param center The center position of the sphere.
 * @param color The color of the sphere.
 * @param radius The radius of the sphere.
 * @return A new Object representing a sphere.
 */
Object Object_NewSphere(Vector3 center, Color color, float radius) {
    Object obj;
    obj.type = OBJECT_TYPE_SPHERE;
    obj.position = center;
    obj.color = color;
    obj.data.sphereData.radius = radius;
    return obj;
}

/**
 * @brief Creates a new Cuboid Object.
 * @param center The center position of the cuboid.
 * @param color The color of the cuboid.
 * @param width The width of the cuboid.
 * @param height The height of the cuboid.
 * @param length The length of the cuboid.
 * @return A new Object representing a cuboid.
 */
Object Object_NewCuboid(Vector3 center, Color color, float width, float height, float length) {
    Object obj;
    obj.type = OBJECT_TYPE_CUBOID;
    obj.position = center;
    obj.color = color;
    obj.data.cuboidData.width = width;
    obj.data.cuboidData.height = height;
    obj.data.cuboidData.length = length;
    return obj;
}

// --- Functions for Object Printing ---

/**
 * @brief Prints the details of a CubeObjectData.
 * @param cubeData The CubeObjectData to print.
 */
void Object_PrintCubeData(CubeObjectData cubeData) {
    printf("  Type: Cube\n");
    printf("  Size: %.2f\n", cubeData.size);
}

/**
 * @brief Prints the details of a SphereObjectData.
 * @param sphereData The SphereObjectData to print.
 */
void Object_PrintSphereData(SphereObjectData sphereData) {
    printf("  Type: Sphere\n");
    printf("  Radius: %.2f\n", sphereData.radius);
}

/**
 * @brief Prints the details of a CuboidObjectData.
 * @param cuboidData The CuboidObjectData to print.
 */
void Object_PrintCuboidData(CuboidObjectData cuboidData) {
    printf("  Type: Cuboid\n");
    printf("  Width: %.2f\n", cuboidData.width);
    printf("  Height: %.2f\n", cuboidData.height);
    printf("  Length: %.2f\n", cuboidData.length);
}

/**
 * @brief Prints the details of a generic Object, dispatching based on its type.
 * @param label A string label for the object.
 * @param obj The Object to print.
 */
void Object_Print(const char* label, Object obj) {
    printf("%s:\n", label);
    Vector3_Print("  Center", obj.position);
    printf("  Color: (R:%d, G:%d, B:%d, A:%d)\n", obj.color.r, obj.color.g, obj.color.b, obj.color.a);

    switch (obj.type) {
        case OBJECT_TYPE_CUBE:
            Object_PrintCubeData(obj.data.cubeData);
            break;
        case OBJECT_TYPE_SPHERE:
            Object_PrintSphereData(obj.data.sphereData);
            break;
        case OBJECT_TYPE_CUBOID:
            Object_PrintCuboidData(obj.data.cuboidData);
            break;
        default:
            printf("  Unknown Object Type\n");
            break;
    }
}

#define INITIAL_OBJECT_CAPACITY 8 // Starting capacity for object list

ObjectList ObjectList_New(void) {
    ObjectList list;
    list.objects = NULL;
    list.capacity = 0;
    list.count = 0;
    return list;
}

int ObjectList_Add(ObjectList* list, Object obj) {
    if (list == NULL) {
        return -1; // Invalid list
    }

    // If capacity is full, reallocate
    if (list->count == list->capacity) {
        size_t new_capacity = (list->capacity == 0) ? INITIAL_OBJECT_CAPACITY : list->capacity * 2;
        Object* new_objects = (Object*)realloc(list->objects, new_capacity * sizeof(Object));

        if (new_objects == NULL) {
            // Reallocation failed
            return -1;
        }
        list->objects = new_objects;
        list->capacity = new_capacity;
    }

    // Add the new object
    list->objects[list->count] = obj;
    list->count++;

    return 0; // Success
}

void ObjectList_Free(ObjectList* list) {
    if (list != NULL && list->objects != NULL) {
        free(list->objects);
        list->objects = NULL;
        list->capacity = 0;
        list->count = 0;
    }
}

Object* ObjectList_Get(ObjectList* list, size_t index) {
    if (list != NULL && index < list->count) {
        return &list->objects[index];
    }
    return NULL;
}