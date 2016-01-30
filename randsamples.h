#ifndef RANDSAMPLES_H
#define RANDSAMPLES_H

#define MAX_ALLOC ((uint32_t)0x40000000)
#define MAX_SAMPLES (MAX_ALLOC/sizeof(uint32_t))

int *randsamp(uint32_t **array, uint32_t x, uint32_t min, uint32_t max);
void shuffle(uint64_t *array, uint64_t n);

#endif