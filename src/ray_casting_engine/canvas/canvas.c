#include "./canvas.h"

Canvas canvas_new(int width, int height) {
    Canvas c;
    c.width = width;
    c.height = height;
    return c;
}
