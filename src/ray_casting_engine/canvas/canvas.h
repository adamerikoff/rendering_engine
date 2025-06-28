#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>

#ifndef _CANVAS_H_
#define _CANVAS_H_

// Define the Application struct
typedef struct Canvas {
    int width;
    int height;
} Canvas;

Canvas canvas_new(int width, int height);

#endif
