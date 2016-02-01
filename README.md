Normalised Frequency Difference Estimator
======================================================================================================================

This implements the NFD measure as described in Bentz, C., Alikaniotis, D., Samardžić, T. & Buttery, P. (submitted). *Variation in word frequency distributions: Definitions, measures and implications for a corpus-based language typology.*

For a more accessible `R` interface please check the [R package](https://github.com/dimalik/nfd).


Installation and usage
------------------------

Simple run the `makefile`

```
make
./main -input /path/to/corpus/ -output /path/to/nfdoutput -sample <int> -alpha <float> -random <int> -simple <int>
```

for more details on the `sample`, `alpha`, `random` and `simple` parameters you can consult the R package page.
