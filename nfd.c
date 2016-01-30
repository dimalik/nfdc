/* nfd.c ***************************************************************************
**
** Normalized Frequency Difference calculator for two discrete ranked distributions
** 
** @param: v1 (uint32_t) the first distribution
** @param: v2 (uint32_t) the second distribution
** @param: v1s (uint32_t) length of the first distribution
** @param: v2s (uint32_t) length of the second distribution
** @return: (float) the normalized frequency difference
**
** @author: dimitris alikaniotis (da352@cam.ac.uk)
** @@
************************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "nfd.h"

float nfd(uint32_t *v1, uint32_t *v2, uint32_t v1s, uint32_t v2s) {
  uint32_t a, sum1=0, sum2=0, sumvec=0;
  uint32_t t = MAX(v1s, v2s);
  
  uint32_t *vec;
  
  vec = (uint32_t *) malloc (t * sizeof(uint32_t));

  // calculate the difference for every rank until you reach
  // the end of the SHORTER distribution, and then write
  // the values of the longer distribution.
  
  if (v1s >= v2s) {
    for (a=0; a<v2s; a++) {
      vec[a] = abs(v1[a] - v2[a]);
      sum1 += v1[a];
      sum2 += v2[a];
    }
    if (v1s > v2s) {
      for (; a<v1s; a++) {
        vec[a] = v1[a];
        sum1 += v1[a];
      }
    }
  } else {
    for (a=0; a<v1s; a++) {
      vec[a] = abs(v1[a] - v2[a]);
      sum1 += v1[a];
      sum2 += v2[a];
    }
    for (; a<v2s; a++) {
      vec[a] = v2[a];
      sum2 += v2[a];
    }
  }
  
  for (a=0; a<t; a++) {
    sumvec += vec[a];
  }
  free(vec);
  return (float) sumvec / (sum1 + sum2);
}