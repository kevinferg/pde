#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include "raylib.h"
#include "render.h"

#include <stdint.h>

/**************************************/

#define MOUSE_MEMBERS \
    float x, y;       \
    int r, c;         \
    uint8_t left, right;

typedef struct {
    MOUSE_MEMBERS
} MouseState;


#include <stdint.h>

#define KEYBOARD_MEMBERS \
    uint32_t tab    : 1;  \
    uint32_t space  : 1;  \


typedef struct {
    KEYBOARD_MEMBERS
} KeyboardState;


typedef struct Controls {
    union { // Mouse
        struct { MOUSE_MEMBERS };
        MouseState mouse;
    };
    MouseState prev_mouse;

    union { // Keyboard
        struct { KEYBOARD_MEMBERS };
        KeyboardState keys;
    };
    KeyboardState prev_keys;
    
    // Other
    float size, min_size, max_size;
    Color color, left_color, right_color;
    int allow_resize;
} Controls;

extern Controls controls;

void update_controls(void);

#endif