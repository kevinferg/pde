#include <stdio.h>

#include "constants.h"
#include "types.h"

#include "raylib.h"
#include "render.h"
#include "input.h"

Controls controls = {
    .min_size = 5, .max_size = 500, .size = 45,
    .allow_resize = 1,
    .left = 0, .right = 0,
};

void update_cursor(void) {
    controls.prev_mouse = controls.mouse;
    controls.left = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    controls.right = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

    controls.r = GetMouseY();
    controls.c = GetMouseX();

    if (controls.allow_resize) {
        float resize =   GetMouseWheelMove() 
                       + IsKeyDown(KEY_EQUAL) 
                       - IsKeyDown(KEY_MINUS);
        if (resize == 0) return;
        float size = controls.size + 5*resize;
        if (size < controls.min_size) size = controls.min_size;
        if (size > controls.max_size) size = controls.max_size;
        controls.size = size;
    }
    
    return;
}



void update_keyboard(void) {
    controls.prev_keys = controls.keys;
    controls.tab = IsKeyDown(KEY_TAB);
    controls.space = IsKeyDown(KEY_SPACE);
}

void update_controls(void) {
    update_cursor();
    update_keyboard();
}
