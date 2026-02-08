#ifndef CONSTANTS_H
#define CONSTANTS_H

// Comment this out to compile locally
// Otherwise make with make PLATFORM=web
// #define PLATFORM_WEB


#define DEFAULT_WINDOW_WIDTH  1280
#define DEFAULT_WINDOW_HEIGHT  720
#define GRID_SCREEN_FRAC 0.9

#define TARGET_FPS 60

#ifndef RES
    #define RES 256
#endif

#define DEFAULT_SCALE       64.0f
#define DEFAULT_DT          0.1f
#define DEFAULT_EPSILON     1.0f
#define DEFAULT_GAMMA       1.0f
#define DEFAULT_INIT_STDEV  0.1f


#endif
