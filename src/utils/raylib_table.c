#include <stdarg.h>
#include "raylib.h"

#include "raylib_table.h"

void create_raylib_table(RaylibTable *t) {
    t->num_rows = 0;
}

void add_raylib_table_row(RaylibTable *t, ...) {
    if (t->num_rows >= MAX_TABLE_ROWS) return;

    va_list args;
    va_start(args, t);

    for (int c = 0; c < t->num_cols; c++)
        t->cells[t->num_rows][c] = va_arg(args, const char *);

    va_end(args);
    t->num_rows++;
}

void draw_raylib_table(RaylibTable *t, Vector2 pos, int base_font_size, float pixel_scale) {
    Font font = GetFontDefault();

    int font_size   = base_font_size * pixel_scale;
    int line_height = (base_font_size + t->row_pad) * pixel_scale;
    int padding     = t->box_pad * pixel_scale;
    int border_px   = t->border_thickness * pixel_scale;
    float spacing   = pixel_scale;

    int box_width = padding * 2;
    for (int c = 0; c < t->num_cols; c++)
        box_width += (int)(t->col_width[c] * pixel_scale) + (int)(t->col_pad * pixel_scale);
    box_width -= (int)(t->col_pad * pixel_scale);

    int box_height = padding * 2 + line_height * t->num_rows;

    DrawRectangle(pos.x, pos.y, box_width, box_height, t->bg_color);

    // Draw border
    for (int i = 0; i < border_px; i++) {
        DrawRectangleLines(pos.x + i, pos.y + i, box_width - 2*i, box_height - 2*i, t->border_color);
    }

    float row_y = pos.y + padding;

    for (int r = 0; r < t->num_rows; r++) {
        float col_x = pos.x + padding;

        for (int c = 0; c < t->num_cols; c++) {
            const char *text = t->cells[r][c];
            int text_width = MeasureTextEx(font, text, font_size, spacing).x;

            float draw_x = col_x;
            if (t->col_align[c] == ALIGN_RIGHT)
                draw_x = col_x + t->col_width[c] * pixel_scale - text_width;

            DrawTextEx(font, text, (Vector2){draw_x, row_y}, font_size, spacing, t->col_color[c]);

            col_x += t->col_width[c] * pixel_scale + t->col_pad* pixel_scale;
        }

        row_y += line_height;
    }
}
