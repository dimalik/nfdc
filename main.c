#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>

#include "main.h"

#define MAX_STRING 100
#define BURNIN 10 // number of trials to ignore in the beginning
#define SIMPLE_ITERS 19450 // if you change the for loop remember to change that


const int hash_size = 30000000;

typedef struct {
    char* word;
} Word, Lemma;

typedef float real;

uint64_t words_found = 0, lemmas_found = 0;
int64_t* word_hash, *lemma_hash;
Word* word_dict, *lemma_dict;

uint64_t* words, *lemmas;
uint64_t words_max_size = 10000000, lemmas_max_size = 10000000;
uint64_t totwords;

real* nfd_vec;
char target_file[MAX_STRING], out_file[MAX_STRING];

// parameters
int sample = 1, rnd = 0, simple = 1;
float alpha = 0.3;

double double_exponential(double x, double a)
{
    // if too slow calculate the inverse root by hand
    return x <= 0.5 ? (pow((2 * x), a) / 2) : 1 - (pow((2 * (1 - x)), a) / 2);
}

int GetHash(const char* token)
{
    uint64_t a, hash = 0;
    for (a = 0; a < strlen(token); a++)
        hash = hash * 257 + token[a];
    hash = hash % hash_size;
    return hash;
}

uint64_t AddVocab(const char* token)
{
    unsigned int hash, length = strlen(token) + 1;
    word_dict[words_found].word = (char*)calloc(length, sizeof(char));
    if (word_dict[words_found].word == NULL) {
        printf("Problem in allocating memory for new word...\n");
        exit(1);
    }
    strcpy(word_dict[words_found].word, token);
    words_found++;

    hash = GetHash(token);
    while (word_hash[hash] != -1)
        hash = (hash + 1) % hash_size;
    word_hash[hash] = words_found - 1;
    return words_found - 1;
}

uint64_t SearchVocab(const char* token)
{
    uint64_t hash = GetHash(token);
    while (1) {
        if (word_hash[hash] == -1)
            return -1;
        if (!strcmp(token, word_dict[word_hash[hash]].word))
            return word_hash[hash];
        hash = (hash + 1) % hash_size;
    }
    return -1;
}

uint64_t AddLemmaVocab(const char* token)
{
    unsigned int hash, length = strlen(token) + 1;
    lemma_dict[lemmas_found].word = (char*)calloc(length, sizeof(char));
    if (lemma_dict[lemmas_found].word == NULL) {
        printf("Problem in allocating memory for new word...\n");
        exit(1);
    }
    strcpy(lemma_dict[lemmas_found].word, token);
    lemmas_found++;

    hash = GetHash(token);
    while (lemma_hash[hash] != -1)
        hash = (hash + 1) % hash_size;
    lemma_hash[hash] = lemmas_found - 1;
    return lemmas_found - 1;
}

uint64_t SearchLemmaVocab(const char* token)
{
    uint64_t hash = GetHash(token);
    while (1) {
        if (lemma_hash[hash] == -1)
            return -1;
        if (!strcmp(token, lemma_dict[lemma_hash[hash]].word))
            return lemma_hash[hash];
        hash = (hash + 1) % hash_size;
    }
    return -1;
}

void LearnVocab(const char* filename)
{
    // temporary arrays to keep track of the words seen
    // they are going to be free'd in the end, where we'll
    // end up only having the arrays of the word indices

    uint64_t i, k = 0;
    char temp_token[MAX_STRING], temp_tag[MAX_STRING], temp_lemma[MAX_STRING];
    FILE* fin;

    if ((word_dict == NULL) || (lemma_dict == NULL)) {
        printf("Could not allocate enough memory for either word_dict or lemma_dict...\n");
        exit(1);
    }

    fin = fopen(filename, "rb");
    if (fin == NULL) {
        printf("Could not locate file\n");
        exit(1);
    }

    for (i = 0; i < hash_size; i++) {
        word_hash[i] = -1;
        lemma_hash[i] = -1;
    }

    while (!feof(fin)) {
        fscanf(fin, "%s %s %s", temp_token, temp_tag, temp_lemma);
        i = SearchVocab(temp_token);
        if (i == -1) {
            i = AddVocab(temp_token);
        }

        words[k] = i;

        if (words_found + 2 >= words_max_size) {
            words_max_size += 1000;
            words = (uint64_t*)realloc(words, words_max_size * sizeof(uint64_t));
        }

        i = SearchLemmaVocab(temp_lemma);
        if (i == -1) {
            i = AddLemmaVocab(temp_lemma);
        }

        lemmas[k] = i;
        k++;
        if (lemmas_found + 2 >= lemmas_max_size) {
            lemmas_max_size += 1000;
            lemmas = (uint64_t*)realloc(lemmas, lemmas_max_size * sizeof(uint64_t));
        }
    }
    totwords = --k;
    fclose(fin);
    free(word_dict);
    free(lemma_dict);
    free(word_hash);
    free(lemma_hash);
}

void SaveVec()
{
    FILE* fout;
    fout = fopen(out_file, "wb");
    uint32_t i, j;
    j = simple ? (SIMPLE_ITERS - BURNIN) : (totwords - BURNIN);
    printf("%d\n", j);

    for (i = 0; i < j; i++)
        fprintf(fout, "%f ", nfd_vec[i]);
    fclose(fout);
    free(nfd_vec);
}

void MakeSimulation()
{
    uint16_t* random_samples;
    uint32_t a, i, nws, nls, *rand_samples = NULL, *words_temp, *lemmas_temp, skipped = 0;
    uint32_t* simple_vec;
    double* rand_vec1, *rand_vec2;

    words_temp = (uint32_t*)malloc(totwords * sizeof(uint32_t));
    lemmas_temp = (uint32_t*)malloc(totwords * sizeof(uint32_t));
    nfd_vec = (real*)malloc((totwords - BURNIN) * sizeof(real));

    if (simple) {
        simple_vec = (uint32_t*)malloc(SIMPLE_ITERS * sizeof(uint32_t));
        a = 0;
        i = 0;

        for (a = 0; a < 10000; a++)
            simple_vec[i++] = a;
        for (a = 10000; a < 50000; a += 10)
            simple_vec[i++] = a;
        for (a = 50000; a < 500000; a += 100)
            simple_vec[i++] = a;
        for (a = 500000; a < 1000000; a += 1000)
            simple_vec[i++] = a;
        for (a = 1000000; a < 5000000; a += 10000)
            simple_vec[i++] = a;
        for (a = 5000000; a < 10000000; a += 100000)
            simple_vec[i++] = a;
    }

    // randomize one of the vectors so the words picked are not correlated
    // needs to be done only once
    shuffle(lemmas, totwords);
    shuffle(words, totwords);

    if (sample) {
        rand_vec1 = (double*)malloc(totwords * sizeof(double));
        rand_vec2 = (double*)malloc(totwords * sizeof(double));
        random_samples = (uint16_t*)malloc(totwords * sizeof(uint16_t));
        for (a = 0; a < totwords; a++)
            rand_vec1[a] = double_exponential((a / 1E7), alpha);
        for (a = 0; a < totwords; a++)
            rand_vec2[a] = ((double)rand() / (RAND_MAX));
        for (a = 0; a < totwords; a++)
            random_samples[a] = (rand_vec1[a] >= rand_vec2[a] ? 1 : 0);
        free(rand_vec1);
        free(rand_vec2);
    }

    if (!simple) {

        if (rnd) {
            //taking random samples
            rand_samples = (uint32_t*)malloc(totwords * sizeof(uint32_t));

            for (a = BURNIN; a < totwords; a++) {
                // discard high samples
                if (sample && random_samples[a]) {
                    nfd_vec[a - BURNIN] = NAN;
                    skipped++;
                    continue;
                }

                randsamp(&rand_samples, a, 0, totwords); // positions of the words we want
                for (i = 0; i < a; i++) {
                    words_temp[i] = words[rand_samples[i]];
                    lemmas_temp[i] = lemmas[rand_samples[i]];
                }

                nws = sortfreqs(&words_temp, a);
                nls = sortfreqs(&lemmas_temp, a);

                nfd_vec[a - BURNIN] = nfd(words_temp, lemmas_temp, nws, nls);
                printf("%c%15u%15.3f%17u", 13, a, nfd_vec[a - BURNIN], skipped);
                setvbuf(stdout, NULL, _IONBF, 0);
            }
        } else {
            // sample sequentially
            for (a = BURNIN; a < totwords; a++) {
                if (sample && random_samples[a]) {
                    nfd_vec[a - BURNIN] = NAN;
                    skipped++;
                    continue;
                }

                randsamp(&rand_samples, a, 0, totwords); // positions of the words we want
                for (i = 0; i < a; i++) {
                    words_temp[i] = words[rand_samples[i]];
                    lemmas_temp[i] = lemmas[rand_samples[i]];
                }

                nws = sortfreqs(&words_temp, a);
                nls = sortfreqs(&lemmas_temp, a);
                nfd_vec[a - BURNIN] = nfd(words_temp, lemmas_temp, nws, nls);
                printf("%c%15u%15.3f%17u", 13, a, nfd_vec[a - BURNIN], skipped);
                setvbuf(stdout, NULL, _IONBF, 0);
            }
        }
    } else {
        if (rnd)
            rand_samples = (uint32_t*)malloc(totwords * sizeof(uint32_t));

        for (a = 10; a < SIMPLE_ITERS; a++) {

            if (rnd) {
                randsamp(&rand_samples, a, 0, totwords); // positions of the words we want
                for (i = 0; i < a; i++) {
                    words_temp[i] = words[rand_samples[i]];
                    lemmas_temp[i] = lemmas[rand_samples[i]];
                }
            } else {

                for (i = 0; i < simple_vec[a]; i++) {
                    words_temp[i] = words[i];
                    lemmas_temp[i] = lemmas[i];
                }
            }

            nws = sortfreqs(&words_temp, a);
            nls = sortfreqs(&lemmas_temp, a);
            nfd_vec[a - BURNIN] = nfd(words_temp, lemmas_temp, nws, nls);
            printf("%c%15u%15.3f%17u", 13, simple_vec[a], nfd_vec[a - BURNIN], skipped);
            setvbuf(stdout, NULL, _IONBF, 0);
        }
    }

    SaveVec();

    free(words_temp);
    free(lemmas_temp);
    free(rand_samples);
}

int ArgPos(char* str, int argc, char** argv)
{
    int a;
    for (a = 1; a < argc; a++)
        if (!strcmp(str, argv[a])) {
            if (a == argc - 1) {
                printf("Argument missing for %s\n", str);
                exit(1);
            }
            return a;
        }
    return -1;
}

int main(int argc, char** argv)
{
    int i;
    if (argc == 1) {
        printf("Normalized Frequency Difference estimation from TreeTagger file\n\n");
        printf("Options:\n");
        printf("\t-input <file>\n");
        printf("\t\tUse text data from <file> to estimate the differences\n");
        printf("\t-output <file>\n");
        printf("\t\tUse <file> to save to resulting vector\n");
        printf("\t-sample <int>\n");
        printf("\t\tSkip iterations (default 1) based on a double exponential function. Otherwise do all iterations (memory intensive)\n");
        printf("\t-alpha <float>\n");
        printf("\t\tSet the alpha parameter of the double exponential. Values closer to zero\n\t\tproduce linear-like functions\n");
        printf("\t-random <int>\n");
        printf("\t\tTake random samples from the final distribution at each step (default 0; sequential sampling).\n");
        printf("\t\tKeep in mind that one of the vectors is randomized by default in the beginning.\n");
        printf("\t-simple <int>\n");
        printf("\t\tImplements a much simpler sampling technique described in the code. Overrides sample!\n\n");
        printf("Example: ./main -input eng_lemmatized_10mio.txt -output results -sample 1 -alpha 0.01 -random 0 -simple 0\n\n");

        return 0;
    }
    if ((i = ArgPos((char*)"-input", argc, argv)) > 0)
        strcpy(target_file, argv[i + 1]);
    if ((i = ArgPos((char*)"-output", argc, argv)) > 0)
        strcpy(out_file, argv[i + 1]);
    if ((i = ArgPos((char*)"-sample", argc, argv)) > 0)
        sample = atoi(argv[i + 1]);
    if ((i = ArgPos((char*)"-alpha", argc, argv)) > 0)
        alpha = atof(argv[i + 1]);
    if ((i = ArgPos((char*)"-random", argc, argv)) > 0)
        rnd = atoi(argv[i + 1]);
    if ((i = ArgPos((char*)"-simple", argc, argv)) > 0)
        simple = atoi(argv[i + 1]);

    if (simple)
        sample = 0;

    srand(time(NULL));

    word_dict = (Word*)calloc(10000000, sizeof(Word));
    lemma_dict = (Word*)calloc(10000000, sizeof(Word));
    word_hash = (int64_t*)calloc(hash_size, sizeof(int64_t));
    lemma_hash = (int64_t*)calloc(hash_size, sizeof(int64_t));
    // these are the only ones we need
    // they are going to hold the entire corpus as
    // number tokens
    words = (uint64_t*)malloc(words_max_size * sizeof(uint64_t));
    lemmas = (uint64_t*)malloc(lemmas_max_size * sizeof(uint64_t));

    printf("Gathering word statistics\n");
    LearnVocab(target_file);
    printf("Found\t%" PRIu64 " words\n\t%" PRIu64 " lemmas\n\t%" PRIu64 " total words\n", words_found, lemmas_found, totwords);
    printf("Starting simulations\n");
    printf("%15s%15s%18s", (char*)"Corpus size", (char*)"NFD", (char*)"Iters skipped\n");
    MakeSimulation();

    free(words);
    free(lemmas);

    return 0;
}
