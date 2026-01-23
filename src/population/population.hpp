#pragma once

#include <memory>
#include <limits>
#include <vector>

#include "../individual/individual.hpp"
#include "../resultCache/resultCache.hpp"

typedef struct Abc_Frame_t_ Abc_Frame_t;
typedef struct Abc_Ntk_t_   Abc_Ntk_t;

enum Stage {
    EXPANSION,
    COMPETITION,
    REFINEMENT,
    REDUCTION,
    COMPLETION,
};

class Population {
private:
    const int size;
    int generation = 0;
    std::vector<Individual> indivduals;
    std::shared_ptr<ResultCache> resultCache;

    Stage currentStage = EXPANSION;
    int gensOnCurrentStage = 0;
    std::string lastBestCommand;
    int last25AvgLevels = std::numeric_limits<int>::max();
    int last25AvgGates = std::numeric_limits<int>::max();
    int last75AvgLevels = std::numeric_limits<int>::max();
    int last75AvgGates = std::numeric_limits<int>::max();

    const Individual & getRandomIndividual(const int sampleSize);
    Stage getStage();
    void createNewGeneration();
    void createClones(std::vector<Individual> &newGen, const int num);
    void createChildren(std::vector<Individual> &newGen, const int num, const float portion);
    void createMutants(std::vector<Individual> &newGen, const int num, const float portion);
    
public:
    Population(const int size, const int startingChromosoneLength);
    Population(std::vector<Individual> &seedExamples, const int size, const int startingChromosoneLength);
    Population(const int size, const int length, Abc_Ntk_t *base);

    // TODO should have it's own frames and just take the network
    Stage runGeneration(Abc_Frame_t **pAbc, Abc_Ntk_t **pNtks, const int nThreads);
    Stage runGeneration(Abc_Frame_t **pAbc, const int nThreads);

    const Individual & getFittest();
};