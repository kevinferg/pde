#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "constants.h"
#include "types.h"
#include "raylib.h"
#include "input.h"
#include "utils/color.h"
#include "utils/raylib_table.h"

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
    .grid_panel = {.x = DEFAULT_WINDOW_HEIGHT*(1-GRID_SCREEN_FRAC)/2.0f,
                   .y = DEFAULT_WINDOW_HEIGHT*(1-GRID_SCREEN_FRAC)/2.0f,
                   .height = DEFAULT_WINDOW_HEIGHT * GRID_SCREEN_FRAC,
                   .width = DEFAULT_WINDOW_HEIGHT * GRID_SCREEN_FRAC},
    .table_location = {DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_HEIGHT*(1-GRID_SCREEN_FRAC)/2.0f},
    .table_below = 0,
    .table_scale = 1.0f,
};

static RaylibTable controls_table = {
    .num_cols = 3,
    .col_width = {RES*2/5, RES*4/5, RES*3/5},
    .col_color = {rgb(48, 0, 192), rgb(0,0,0), rgb(166, 0, 192)},
    .col_align = {ALIGN_RIGHT, ALIGN_LEFT, ALIGN_LEFT},

    .bg_color = rgb(255, 255, 255),
    .border_color = rgb(0, 0, 0),
    .border_thickness = 5,
    
    .box_pad = 12,
    .col_pad = 10,
    .row_pad = 4,
};



/*******************************************************/

int render_controls_table(ProgramState* state);
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
        render_controls_table(state);
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
    float margin = (min_dim - main_window.min_dim)/2.0f;
    main_window.grid_panel = (Rectangle) {
        .x = margin,
        .y = margin,
        .height = main_window.min_dim,
        .width = main_window.min_dim
    };
    if (main_window.height > main_window.width) {
        main_window.table_location = (Vector2) {margin, min_dim};
        main_window.table_below = 1;
    } else {
        main_window.table_location = (Vector2) {min_dim, margin};
        main_window.table_below = 0;
    }
    
    return 0;
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
        main_window.grid_panel,
        (Vector2){0, 0},
        0.0f,
        WHITE
    );
    return 0;
}

int render_controls_table(ProgramState* state) {
    RaylibTable* table = &controls_table;
    char* current_pde = state->pde? "Cahn-Hilliard": "Swift-Hohenberg";
    char* sim_state = state->paused? "Paused": "Playing";
    char brush_str[20];
    sprintf(brush_str, "%g", controls.size);
    create_raylib_table(table);

    #define TABLE_FONT_SIZE 20

    add_raylib_table_row(table, "[TAB]",   "Change current PDE:",    current_pde);
    add_raylib_table_row(table, "[SPACE]", "Pause/Play simulation:", sim_state);
    add_raylib_table_row(table, "[-/+]",   "Change brush size",      brush_str);
    add_raylib_table_row(table, "[Scroll]","(same as above)",    "-");
    add_raylib_table_row(table, "[Left click]", "Paint value -1", "(Blue)");
    add_raylib_table_row(table, "[Right click]", "Paint value +1", "(Red)");
    add_raylib_table_row(table, "[L+R click]", "Paint value 0", "(White)");
    draw_raylib_table(table, main_window.table_location, TABLE_FONT_SIZE, main_window.table_scale);
    return 0;
}