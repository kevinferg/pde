#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "constants.h"
#include "types.h"
#include "raylib.h"
#include "input.h"
#include "utils/color.h"

#include "render.h"



char main_window_name[] = DEFAULT_WINDOW_NAME;

Window main_window = {
    .width = DEFAULT_WINDOW_WIDTH,
    .height = DEFAULT_WINDOW_HEIGHT,
    .min_dim = DEFAULT_WINDOW_HEIGHT * GRID_SCREEN_FRAC,
    .per_col = 1./DEFAULT_WINDOW_WIDTH,
    .per_row = 1./DEFAULT_WINDOW_HEIGHT,
    .xmin = -1, .xmax = 1, .Lx = 2, .invLx = .5,
    .ymin = -1, .ymax = 1, .Ly = 2, .invLy = .5,
    .flip_y = 1,
    .bg_color = rgb(65, 58, 41),
    .name = main_window_name,
};


/*******************************************************/

int render_cursor(void);
int render_grid(FloatGrid* grid);

static int window_was_resized(void);
static int update_window_size(void);

/*******************************************************/

int render_init(void) {
    SetTraceLogLevel(LOG_ERROR);
    SetTargetFPS(TARGET_FPS);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(main_window.width, main_window.height, main_window.name);
    HideCursor();
    return 0;
}

int render_end(void) {
    ShowCursor();
    CloseWindow(); 
    return 0;
}

int render_escape(void) {
    return WindowShouldClose();
}

int render_frame(ProgramState* state) {
    if (window_was_resized()) update_window_size();

    BeginDrawing();
        ClearBackground(main_window.bg_color);
        
        // Render everything here
        render_grid(&(state->grid));
        render_cursor();

    EndDrawing();
    return 0;
}

/************************************************************************/

static int window_was_resized(void) {
    return (main_window.width != GetScreenWidth()) ||
           (main_window.height != GetScreenHeight());
}

static int update_window_size(void) {
    main_window.width = GetScreenWidth();
    main_window.height = GetScreenHeight();
    main_window.per_row = 1./main_window.height;
    main_window.per_col = 1./main_window.width;
    int min_dim = main_window.height < main_window.width? 
                  main_window.height: 
                  main_window.width;
    main_window.min_dim = (int) ((float) min_dim*GRID_SCREEN_FRAC);
    return 0;
}

/************************************************************************/

int xval_to_col(float x) {
    return (main_window.invLx * (x - main_window.xmin)) * main_window.width;
}
int yval_to_row(float y) {
    if (main_window.flip_y) {
        return (main_window.invLy * (main_window.ymax - y)) * main_window.width;
    } else {
        return (main_window.invLy * (y - main_window.xmin)) * main_window.width;
    }
}
float col_to_xval(int c) {
    return main_window.xmin + c * main_window.per_col;
}
float row_to_yval(int r) {
    if (main_window.flip_y) {
        return main_window.ymax - r * main_window.per_row;
    } else {
        return main_window.ymin + r * main_window.per_row;
    }
}
void xy_to_rowcol(float x, float y, int* r, int* c) {
    *r = yval_to_row(y);
    *c = xval_to_col(x);
    return;
}
void rowcol_to_xy(int r, int c, float* x, float* y) {
    *x = col_to_xval(c);
    *y = row_to_yval(r);
    return;
}

/************************************************************************/

int render_cursor(void) {
    DrawRing((Vector2){controls.c, controls.r}, controls.size - 2, controls.size + 5, 0.0, 360.0, 24, BLACK);
    return 0;
}

/************************************************************************/

static ColorMap cmap_grays= CMAP2(-1.0, 1.0, 
          rgb(255, 255, 255), rgb(0, 0, 0));

static ColorMap cmap_gkv = CMAP3(-1.0, 1.0, 
          rgb(38, 255, 0), rgb(58, 58, 58), rgb(153, 0, 255));

static ColorMap cmap_bwr= CMAP3(-1.0, 1.0, 
          rgb(44, 110, 224), rgb(255, 255, 255), rgb(187, 37, 37));


int render_grid(FloatGrid* grid) {
    static Color pixels[RES*RES];
    static Image img = {0};
    static Texture2D tex = {0};
    static int cur_w = -7777, cur_h = -7777;

    if (grid->cols != cur_w || grid->rows != cur_h) {
        if (tex.id) UnloadTexture(tex);
        img = (Image) {
            .data = pixels, .mipmaps = 1,
            .width = grid->cols, .height = grid->rows,
            .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
        };
        tex = LoadTextureFromImage(img);
        SetTextureFilter(tex, TEXTURE_FILTER_POINT);
        cur_w = grid->cols;
        cur_h = grid->rows;
    }

    int num_pixels = grid->cols*grid->rows;
    for (int i = 0; i < num_pixels; i++) {
        pixels[i] = map_color(grid->data[i], &cmap_bwr);
    }

    int mindim = main_window.min_dim;
    UpdateTexture(tex, pixels);
    DrawTexturePro(
        tex,
        (Rectangle){0, 0, grid->cols, grid->rows},
        (Rectangle){(main_window.width - mindim)/2, 
                    (main_window.height - mindim)/2, 
                     mindim, mindim},
        (Vector2){0, 0},
        0.0f,
        WHITE
    );
    return 0;
}
