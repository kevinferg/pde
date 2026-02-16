#ifndef RAYLIB_TABLE_H
#define RAYLIB_TABLE_H

#include "raylib.h"

#define MAX_TABLE_ROWS 16
#define MAX_TABLE_COLS 4

typedef enum TableAlign {
    ALIGN_LEFT,
    ALIGN_RIGHT
} TableAlign;

typedef struct RaylibTable {
    int num_cols;
    int num_rows;

    int col_width[MAX_TABLE_COLS];
    Color col_color[MAX_TABLE_COLS];
    TableAlign col_align[MAX_TABLE_COLS];

    const char *cells[MAX_TABLE_ROWS][MAX_TABLE_COLS];

    Color bg_color;
    Color border_color;
    int border_thickness;
    
    int row_pad;
    int box_pad;
    int col_pad;
    
} RaylibTable;


void create_raylib_table(RaylibTable *t);
void add_raylib_table_row(RaylibTable *t, ...);
void draw_raylib_table(RaylibTable *t, Vector2 pos, int base_font_size, float pixel_scale);

#endif 