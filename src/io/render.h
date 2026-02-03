#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include "types.h"
#include "raylib.h"

#define DEFAULT_WINDOW_NAME "Demo"

typedef struct Window {
    int width, height;
    int min_dim;
    float per_row, per_col;
    float Lx, Ly, invLx, invLy;
    float xmin, xmax;
    float ymin, ymax;
    int flip_y;
    Color bg_color;
    char* name;
} Window;

extern Window main_window;

/*************************/
int render_init(void);
int render_end(void);
int render_escape(void);
int render_frame(ProgramState* state);
/*************************/


/****************************************************/
int xval_to_col(float x);
int yval_to_row(float y);
float col_to_xval(int c);
float row_to_yval(int r);
void xy_to_rowcol(float x, float y, int* r, int* c);
void rowcol_to_xy(int r, int c, float* x, float* y);
/****************************************************/


#endif