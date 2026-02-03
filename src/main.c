#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <complex.h>

#include "constants.h"
#include "types.h"
#include "raylib.h"
#include "io/render.h"
#include "io/input.h"
#include "utils/rng.h"
#include "utils/fft.h"


int step_pde(ProgramState* state);
int update_grid_with_cursor(ProgramState* state);
void update_control_settings(ProgramState* state);

int run_demo(void);
int update_frame(ProgramState* state);

/**********************************************************************/

int main(int argc, char** argv) {
    run_demo();
    return 0;
}

/**********************************************************************/

int run_demo(void) {
    const int rows = RES;
    const int cols = RES;
    float data[RES*RES] = {0};
    random_normal_array(data, RES*RES, 0, DEFAULT_INIT_STDEV);
    FloatGrid grid = {.rows = rows, .cols = cols, .data = data};

    ProgramState state = {
        .frame = 0,
        .grid = grid,
        .completed = 0,
        .paused = 0,
        .pde = 0,
        .scale      = DEFAULT_SCALE,
        .dt         = DEFAULT_DT,
        .epsilon    = DEFAULT_EPSILON,
        .gamma      = DEFAULT_GAMMA,
        .init_stdev = DEFAULT_INIT_STDEV,
    };

    render_init();
    while (!render_escape()) {
        update_controls();
        update_frame(&state);
        render_frame(&state);
    }
    state.completed = 1;
    step_pde(&state); // Free buffer used for solving PDE
    render_end();
    return 0;
}

int update_frame(ProgramState* state) {
    (state->frame)++;
    update_control_settings(state);
    if (!state->paused) {
        step_pde(state);
    }
    update_grid_with_cursor(state);
    return 0;
}


void draw_at_grid_indices(FloatGrid* grid, float value, int r, int c) {
    int rad = controls.size * grid->rows / main_window.min_dim;
    int x0 = c - rad;
    int x1 = c + rad;
    int y0 = r - rad;
    int y1 = r + rad;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 >= grid->cols) x1 = grid->cols - 1;
    if (y1 >= grid->rows) y1 = grid->rows - 1;

    int r2 = rad*rad;

    for (int y = y0; y <= y1; y++) {
        if (y < 0 || y >= grid->rows) continue;
        int dy = y - r;
        int dy2 = dy * dy;
        for (int x = x0; x <= x1; x++) {
            if (x < 0 || x >= grid->cols) continue;
            int dx = x - c;
            int dist = dx*dx + dy2;
            if (dist > r2) continue;
            grid->data[y * grid->rows + x] = value;
        }
    }
}

void draw_between_grid_indices(FloatGrid* grid, float value, int prev_r, int prev_c, int r, int c) {
    if (abs(prev_c - c) > 1 || abs(prev_r - r) > 1) {
        int mid_c = (prev_c + c) / 2;
        int mid_r = (prev_r + r) / 2;
        draw_between_grid_indices(grid, value, prev_r, prev_c, mid_r, mid_c);
        draw_between_grid_indices(grid, value, mid_r, mid_c, r, c);
    } else {
        draw_at_grid_indices(grid, value, r, c);
    }
}

int update_grid_with_cursor(ProgramState* state) {
    if (!controls.left && !controls.right) {
        return 0;
    }

    int min_dim = main_window.min_dim;
    int x0 = (main_window.width  - min_dim)/2;
    int y0 = (main_window.height - min_dim)/2;
    int gx = (controls.c - x0) * RES / min_dim;
    int gy = (controls.r - y0) * RES / min_dim;
    int prev_gx = (controls.prev_mouse.c - x0) * RES / min_dim;
    int prev_gy = (controls.prev_mouse.r - y0) * RES / min_dim;

    FloatGrid grid = state->grid;
    float value = -1.0f*(controls.left) + 1.0f*(controls.right);
    
    draw_between_grid_indices(&grid, value, prev_gy, prev_gx, gy, gx);

    return 0;
}

void update_control_settings(ProgramState* state) {
    if (controls.tab && !controls.prev_keys.tab) {
        state->pde = !state->pde;
    }

    if (controls.space && !controls.prev_keys.space) {
        state->paused = !state->paused;
    }
}



/**********************************************************************/

int step_pde(ProgramState* state) {
    float* u = state->grid.data;
    int res = RES;
    float epsilon = state->epsilon;
    float scale = state->scale;
    float dt = state->dt;
    float gamma = state->gamma;


    int N = res*res;
    int i, x, y;
    static int init = 0;
    static size_t total_size;
    static void* mem;
    static complex32_t *uhat, *uhat_buf, *tmp;
    static float *Lap, *real_vals, *K2;

    if (state->completed) {
        free(mem);
        return 0;
    }
    if (init == 0) {
        total_size =
            3*res*res*sizeof(complex32_t) +   /* uhat, uhat_buf, tmp */
            (2*res*res + res)*sizeof(float);  /* Lap, real_vals, K2 */

        mem = malloc(total_size);
        if (!mem) return -1;

        /***********   Initialize buffers   *********/
        uhat     = (complex32_t *)mem;
        uhat_buf = uhat + res*res;
        tmp      = uhat_buf + res*res;

        Lap       = (float *)(tmp + res*res);
        real_vals = Lap + res*res;
        K2        = real_vals + res*res;

        float lin_op, dk = 2.0f * PI / scale;
        for (i = 0; i <= RES/2; i++) {
            K2[i] = i * dk;
            K2[i] *= K2[i];
            K2[RES - i] = K2[i];
        }
        for (y = 0; y < RES; y++) {
            for (x = 0; x < RES; x++) {
                Lap[y*RES + x] = K2[x] + K2[y];
            }
        }
        /**********************************************/
        init = 1;
    }
    
    if (state->pde == 0) {
        for (y = 0; y < N; y++)
            u[y] = u[y] + dt * ( -u[y]*u[y]*u[y] );

        fft2_real(u, uhat, uhat_buf, RES);

        for (y = 0; y < N; y++) {
            float lin_op = (gamma*gamma - Lap[y]);
            lin_op = epsilon - lin_op*lin_op;
            lin_op = (1.0f - dt*lin_op);
            uhat[y] /= lin_op;
        }

        ifft2_complex(uhat, u, uhat_buf, res);

    } else {    
        for (i = 0; i < N; i++)
            real_vals[i] = u[i]*u[i]*u[i] - u[i];

        fft2_real(real_vals, uhat_buf, tmp, res);
        fft2_real(u, uhat, tmp, res);

        for (i = 0; i < N; i++) {
            float k2 = Lap[i];
            float denom = 1.0f + dt * epsilon * gamma * k2 * k2;
            uhat[i] = (uhat[i] - dt * epsilon * k2 * uhat_buf[i]) / denom;
        }

        ifft2_complex(uhat, u, tmp, res);
    }

    return 0;
}