#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "rng.h"

/***********************************************************************/

void random_normal_array(float* arr, int N, float mean, float stdev) {
    uint32_t i;
    for (i = 0; i < N; i++) {
        arr[i] = frandn() * stdev + mean;
    }
}

/***********************************************************************/

int init_bag(RandomBag* bag) {
    bag->count = 0;
    bag->data = malloc(INITIAL_BAG_SIZE*sizeof(int));
    if (bag->data == NULL) {
        bag->capacity = 0;
        return -1;
    }
    bag->capacity = INITIAL_BAG_SIZE;
    return 0;
}
int free_bag(RandomBag* bag) {
    bag->capacity = 0;
    bag->count = 0;
    free(bag->data);
    return 0;
}

int append_to_bag(RandomBag* bag, int value) {
    if (bag->count >= bag->capacity) {
        int new_capacity = bag->capacity ? bag->capacity * 2 : 1;
        int* new_data = realloc(bag->data, new_capacity * sizeof(int));
        if (!new_data) return -1;
        bag->data = new_data;
        bag->capacity = new_capacity;
    }
    int i = urandi(bag->count);
    bag->data[(bag->count)++] = bag->data[i];
    bag->data[i] = value;
    return 0;
}

int add_to_bag(RandomBag* bag, int value, int count) {
    int i;
    for (i = 0; i < count; i++) {
        if (append_to_bag(bag, value)) return -1;
    }
    return 0;
}

int sample_from_bag(RandomBag* bag, int replace, int errval) {
    if (bag->count <= 0) return errval;
    int index = urandi(bag->count);
    int value = bag->data[index];
    if (!replace) bag->data[index] = bag->data[--(bag->count)];
    return value;
}