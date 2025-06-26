#include "color.h"

Color color_new(unsigned char r, unsigned char g, unsigned char b) {
    Color c;
    c.r = r;
    c.g = g;
    c.b = b;
    return c;
}
