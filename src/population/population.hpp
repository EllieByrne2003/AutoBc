#pragma once

#include <vector>

#include "../individual/individual.hpp"

typedef struct Abc_Frame_t_ Abc_Frame_t;
typedef struct Abc_Ntk_t_   Abc_Ntk_t;

class Population {
private:
    const int size;
    int generation = 0;
    std::vector<Individual> indivduals;

    const Individual & getRandomIndividual(const int sampleSize);

    // void runIndividuals(Abc_Frame_t *pAbc, Abc_Ntk_t *pNtk, const int start, const int end);
public:
    Population(const int size, const int startingChromosoneLength);

    // TODO should have it's own frames and just take the network
    void runGeneration(Abc_Frame_t **pAbc, Abc_Ntk_t **pNtks, const int nThreads);
    const Individual & getFittest();
};