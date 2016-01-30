/* sortfreqs.c **************************************************************
**
** Extract frequency distributions from arrays and sort in decreasing order
** using hash-tables
** 
** @param: array (uint32_t) the array to be sorted by frequency
** @param: length (uint32_t) the length of the above array
** @return: (uint32_t) the new length of the array
**
** @author: dimitris alikaniotis (da352@cam.ac.uk)
** @@
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "sortfreqs.h"

#define MAX_SIZE 50000  // max_size of the hash
                        // should be larger than
                        // the total number of words

struct node {
    int count;
    int arrindex;
};

int compare(const void* a, const void* b)
{
    int num1 = ((struct node*)a)->count;
    int num2 = ((struct node*)b)->count;
    int arrindex1 = ((struct node*)a)->arrindex;
    int arrindex2 = ((struct node*)b)->arrindex;
    if (num1 - num2)
        return (num2 - num1);
    else
        return (arrindex1 - arrindex2);
}

uint32_t sortfreqs(uint32_t** array, uint32_t length)
{
  uint32_t words_found = 0;
      
  uint64_t i;
  struct node hash[MAX_SIZE]; // for really large MAX_SIZE consider
                              // using the heap by malloc'ing

  for (i = 0; i < MAX_SIZE; i++) {
      hash[i].count = 0;
      hash[i].arrindex = -1;
  }

  for (i = 0; i < length; i++) {
      hash[(*array)[i]].count++;
      if (hash[(*array)[i]].arrindex == -1) {
        hash[(*array)[i]].arrindex = i;
        words_found++; // if new word keep track
      }
  }
  
  qsort(hash, MAX_SIZE, sizeof(hash[0]), compare);
  for (i = 0; i < words_found; i++) {
    (*array)[i] = hash[i].count;
  }
  return words_found;
}
