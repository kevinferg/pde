#ifndef COLOR_H
#define COLOR_H

#include "raylib.h"

typedef struct ColorMapEntry {
    float value;
    Color color;
} ColorMapEntry;

typedef struct ColorMap {
    ColorMapEntry* data;
    int size;
} ColorMap;

#define rgb(r,g,b) (Color){r,g,b,255}
#define rgba(r,g,b,a) (Color){r,g,b,a}

Color map_color(float val, ColorMap* cm);

#define CMAP2(vmin, vmax, cmin, cmax) \
    (ColorMap){ \
        .data = (ColorMapEntry[]){ \
            { (float)(vmin), (cmin) }, \
            { (float)(vmax), (cmax) }, \
        }, \
        .size = 2 \
    }


#define CMAP3(vmin, vmax, cmin, cmid, cmax) \
    (ColorMap){ \
        .data = (ColorMapEntry[]){ \
            { (float)(vmin),                     (cmin) }, \
            { (float)((vmin) + (vmax)) * 0.5f,   (cmid) }, \
            { (float)(vmax),                     (cmax) }, \
        }, \
        .size = 3 \
    }


#endif