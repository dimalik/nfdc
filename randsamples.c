/* randsamples.c *****************************************************************
**
** Draw random samples (uniformly) without replacement from a range
** 
** @param: array (uint32_t**) the array that the results are going to be stored in
** @param: x (uint32_t) the number of random samples to draw
** @param: min (uint32_t) lower bound of the range
** @param: max (uint32_t) upper bound of the range
** @return: (int) if error
**
** @author: dimitris alikaniotis (da352@cam.ac.uk)
** @@
*********************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "randsamples.h"

int *randsamp(uint32_t **array, uint32_t x, uint32_t min, uint32_t max)
{
	uint32_t r, i=x;
	if (!x || x > MAX_SAMPLES || x > (max-min+1))
		return NULL;

	while (i--) {
		r = (max-min+1-i);
		(*array)[i] = min += (r ? rand() % r: 0);
		min--;
	}

	while (x>1) {
		r = (*array)[i=rand()%x--];
		(*array)[i]=(*array)[x];
		(*array)[x]=r;
	}
  return 0;
}

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
void shuffle(uint64_t *array, uint64_t n)
{
    if (n > 1) 
    {
        uint64_t i;
        for (i = 0; i < n - 1; i++) 
        {
          uint64_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}