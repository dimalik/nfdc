#ifndef MAIN_H
#define MAIN_H
#include <math.h>
#include "randsamples.c"
#include "nfd.c"
#include "sortfreqs.c"

int GetHash(const char* token);
uint64_t AddVocab(const char* token);
uint64_t SearchVocab(const char* token);
uint64_t AddLemmaVocab(const char* token);
uint64_t SearchLemmaVocab(const char* token);
void LearnVocab(const char* filename);
void SaveVec();
void MakeSimulation();
int ArgPos(char* str, int argc, char** argv);
double double_exponential(double x, double a);

#endif