#ifndef TYPES_H
#define TYPES_H

typedef struct FloatGrid {
    int rows;
    int cols;
    float* data;
} FloatGrid;



typedef struct ProgramState {
    int frame;
    FloatGrid grid;

    int completed;
    int paused;
    int pde;

    float scale;       // Number of cycles across domain
    float dt;          // Time step
    float epsilon;     // Linear growth coefficient       /    diffusion coefficient
    float gamma;       // Wave number                     /    transition length
    float init_stdev;  // Stdev of initial noise field
} ProgramState;

#endif