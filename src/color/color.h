#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>

#ifndef _COLOR_H_
#define _COLOR_H_

typedef struct Color {
    float r;
    float g;
    float b;
} Color;

Color color_new(unsigned char r, unsigned char g, unsigned char b);

#endif
