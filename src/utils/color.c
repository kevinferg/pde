#include <stdint.h>
#include "raylib.h"
#include "color.h"


Color map_color(float val, ColorMap* cm) {
    if(val <= cm->data[0].value)          return cm->data[0].color;
    if(val >= cm->data[cm->size-1].value) return cm->data[cm->size-1].color;

    for(int i=0; i < cm->size-1; i++) {
        if(val >= cm->data[i].value && val <= cm->data[i+1].value) {
            float t = (val - cm->data[i].value) / (cm->data[i+1].value - cm->data[i].value);
            Color c0 = cm->data[i].color;
            Color c1 = cm->data[i+1].color;
            return (Color){
                (uint8_t)(c0.r + t*(c1.r - c0.r)),
                (uint8_t)(c0.g + t*(c1.g - c0.g)),
                (uint8_t)(c0.b + t*(c1.b - c0.b)),
                (uint8_t)(c0.a + t*(c1.a - c0.a))
            };
        }
    }
    return cm->data[cm->size-1].color;
}