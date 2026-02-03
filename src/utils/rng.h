#ifndef RNG_H
#define RNG_H

#include <stdint.h>

static inline uint16_t urand15(void);                      // Generate random unsigned integer from [0, RNG_MAX] (15 bits)
static inline uint32_t urandi(int max_plus_one);           // Generate random unsigned integer from [0, max_plus_one)
static inline int32_t rand_int(int min, int max_plus_one); // Sample random signed integer from [min, max_plus_one)
static inline float frand(void);                           // Sample random float uniformly from interval (0,1)
static inline float frand_uniform(float min, float max);   // Sample random float uniformly from custom interval (min, max)
static inline float frandn(void);                          // Sample random float from standard normal distribution: mean 0, std 1
static inline float frand_normal(float mean, float std);   // Sample random float from custom normal distribution

void random_normal_array(float* arr, int N, float mean, float stdev);

/*******************************************************************************/
// RandomBag: 
// - Put items (integers) into a bag and sample from among them
// - Includes with and without replacement

#define INITIAL_BAG_SIZE 32

typedef struct RandomBag {
    int* data;
    int count;
    int capacity;
} RandomBag;

int init_bag(RandomBag* bag);
int free_bag(RandomBag* bag);

int append_to_bag(RandomBag* bag, int value);                 // Put a single integer into the bag
int add_to_bag(RandomBag* bag, int value, int count);         // Put multiple of the same integer into the bag
int sample_from_bag(RandomBag* bag, int replace, int errval); // Sample from the bag, specify replacement
                                                              // ...and value to return upon error

/*******************************************************************************/

// Get/set RNG seed
static uint32_t rng15_seed;
static inline uint32_t get_rng15_seed(void) {
    return rng15_seed;
}
static inline void set_rng15_seed(uint32_t seed) {
    rng15_seed = seed;
    return;
}

// Sample a random 16-bit integer, URAND15_MAX or lower
#define URAND15_MAX 0x7fff
static inline uint16_t urand15(void) {
    rng15_seed = rng15_seed * 747796405u + 2891336453u;
    return rng15_seed >> 16;  // return 15-bit result
}

// Integer functions
static inline uint32_t urandi(int max_plus_one) {
    return (((uint64_t) urand15()*max_plus_one) >> 15);
}
static inline int32_t rand_int(int min, int max_plus_one) {
    uint32_t numvals = max_plus_one - min;
    uint32_t r = (((uint32_t) urand15()*numvals) >> 15);
    return min + r;
}

// Float functions with standard parameters:
static inline float frand(void) {
    return (float) urand15() / (float) ((uint16_t) URAND15_MAX+1);
}
static inline float frandn(void) {
    static const int16_t samples[5][8] = {
        {-18666, 1465,  9124, -2456,  7113,   2465,  2440,  -4190},
        { -1690, 1656, -6392,  3616, -7014,   8614,  5799,  -1058},
        { -3021, 4302, -3768, -7831, 11343,  13204, -1676,  -5054},
        {  1012, 6001,  5497, 14892,   487,    967,   -39, -13798},
        { -7858, 3286,  1206, -5579, 14569, -11125, -9127,  -8716}
    }; // ^ Fine-tuned samples from ~N(mu=0, sig=2^14/sqrt(5))
    uint16_t r = urand15();           // 15 random bits
    int32_t x;                      // Add 1 entry from each row
    x  = samples[0][r&7];  r >>= 3;
    x += samples[1][r&7];  r >>= 3;
    x += samples[2][r&7];  r >>= 3;
    x += samples[3][r&7];  r >>= 3;
    x += samples[4][r&7];           // Now, x~N(mu=0, sig=2^14)
    return (float) x * (1.0f / 16384.0f);
}

// Float functions with custom parameters:
static inline float frand_uniform(float min, float max) {
    return min + frand() * (max - min);
}
float frand_normal(float mean, float std) {
    return mean + std * frandn();
}

#endif