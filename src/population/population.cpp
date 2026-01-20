#include "population.hpp"

#include <algorithm>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <random>
#include <thread>
#include <atomic>

#include "../abc.hpp"
#include "../utils/random.hpp"

#define CHANGE_CHANCE (1.0 / 3.0)
#define REMOVE_CHANCE (1.0 / 3.0)
#define ADD_CHANCE    (1.0 / 3.0)

Population::Population(const int size, const int startingChromosoneLength) : size(size) {
    for(int i = 0; i < size; i++) {
        indivduals.push_back(Individual(startingChromosoneLength));
    }
}

Population::Population(std::vector<Individual> &seedExamples, const int size, const int startingChromosoneLength) :
    size(size) {
    for(const Individual &seedExample : seedExamples) {
        indivduals.push_back(seedExample);
    }

    while(indivduals.size() < size) {
        indivduals.push_back(Individual(startingChromosoneLength));
    }
}

const Individual & Population::getRandomIndividual(const int sampleSize) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<size_t> dist(0, sampleSize - 1);
    return indivduals[dist(gen)];
}


Stage Population::getStage() {
    // Analyze stage, check best command, check average scores of entire pop and top quartile
    const std::string bestCommand = getFittest().getCommand();
    int total25Levels = 0, total75Levels = 0;
    int total25Gates  = 0, total75Gates  = 0;
    for(int i = 0; i < size * 0.25; i++) {
        const Individual &ind = indivduals[i];

        total25Levels += ind.getLevels();
        total25Gates  += ind.getGates();
    }
    for(int i = 0; i < size * 0.75; i++) {
        const Individual &ind = indivduals[i];

        total75Levels += ind.getLevels();
        total75Gates  += ind.getGates();
    }

    const int curr25AvgLevels = total25Levels / (size * 0.25);
    const int curr25AvgGates  = total25Gates  / (size * 0.25);
    const int curr75AvgLevels = total75Levels / (size * 0.75);
    const int curr75AvgGates  = total75Gates  / (size * 0.75);

    // If significant changes ongoing, EXPANSION
    if(bestCommand != lastBestCommand || last25AvgLevels - curr25AvgLevels >= 5 || last25AvgGates - curr25AvgGates >= 25) {
        last25AvgLevels = curr25AvgLevels;
        last25AvgGates  = curr25AvgGates;
        last75AvgLevels = curr75AvgLevels;
        last75AvgGates  = curr75AvgGates;
        lastBestCommand = bestCommand;
        return EXPANSION;
    }

    // If some changes, COMPETITION
    if(last25AvgLevels - curr25AvgLevels >= 3 || last75AvgLevels - curr75AvgLevels >= 5 || last25AvgGates - curr25AvgGates >= 15) {
        last25AvgLevels = curr25AvgLevels;
        last25AvgGates  = curr25AvgGates;
        last75AvgLevels = curr75AvgLevels;
        last75AvgGates  = curr75AvgGates;

        if(gensOnCurrentStage < 1 && currentStage == EXPANSION) {
            return currentStage;
        }

        gensOnCurrentStage = 0;
        return COMPETITION;
    }

    // If few any changes, REFINEMENT
    if(last25AvgLevels - curr25AvgLevels >= 1 || last75AvgLevels - curr75AvgLevels >= 3 || last25AvgGates - curr25AvgGates >= 5) {
        last25AvgLevels = curr25AvgLevels;
        last25AvgGates  = curr25AvgGates;
        last75AvgLevels = curr75AvgLevels;
        last75AvgGates  = curr75AvgGates;

        if(gensOnCurrentStage < 2 && (currentStage == EXPANSION || currentStage == COMPETITION)) {
            return currentStage;
        }

        if(currentStage == EXPANSION) {
            gensOnCurrentStage = 0;
            return COMPETITION;
        }

        gensOnCurrentStage = 0;
        return REFINEMENT;
    }

    // If barely any changes, REDUCTION

    if(last75AvgGates - curr75AvgGates >= 1) {
        last25AvgLevels = curr25AvgLevels;
        last25AvgGates  = curr25AvgGates;
        last75AvgLevels = curr75AvgLevels;
        last75AvgGates  = curr75AvgGates;

        if(gensOnCurrentStage < 3 && (currentStage == EXPANSION || currentStage == COMPETITION || currentStage == REFINEMENT)) {
            return currentStage;
        }

        if(currentStage == EXPANSION) {
            gensOnCurrentStage = 0;
            return COMPETITION;
        } 

        if(currentStage == COMPETITION) {
            gensOnCurrentStage = 0;
            return REFINEMENT;
        }

        return REDUCTION;
    }

    if(gensOnCurrentStage < 4 && (currentStage == EXPANSION || currentStage == COMPETITION || currentStage == REFINEMENT || currentStage == REDUCTION)) {
        return currentStage;
    }

    if(currentStage == EXPANSION) {
        gensOnCurrentStage = 0;
        return COMPETITION;
    } 

    if(currentStage == COMPETITION) {
        gensOnCurrentStage = 0;
        return REFINEMENT;
    }

    if(currentStage == REFINEMENT) {
        gensOnCurrentStage = 0;
        return REDUCTION;
    }

    // If all equal or max gens reached, COMPLETION
    gensOnCurrentStage = 0;
    return COMPLETION;
}

void Population::createNewGeneration() {
    std::vector<Individual> newGen;

    switch(currentStage) {
        case EXPANSION:
            // High addition chance/opportunities, high crossover, low rest
            createClones(newGen, size * 0.1);
            createMutants(newGen, size * 0.6, 0.5);
            createChildren(newGen, size * 0.3, 0.5);

            break;
        case COMPETITION:
            // High crossover, high coarse mutation, low rest
            createClones(newGen, size * 0.1);
            createMutants(newGen, size * 0.45, 0.5);
            createChildren(newGen, size * 0.45, 0.5);

            break;
        case REFINEMENT:
            // High cloning, high coarse mutation, high fine mutation, low rest
            createClones(newGen, size * 0.4);
            createMutants(newGen, size * 0.4, 0.5);
            createChildren(newGen, size * 0.2, 0.5);

            break;
        case REDUCTION:
            // High cloning 2 per, high removal, low rest
            createClones(newGen, size * 0.25);
            createClones(newGen, size * 0.25);
            createMutants(newGen, size * 0.4, 0.5);
            createChildren(newGen, size * 0.1, 0.5);

            break;
        default:
            // TODO put some error message here

            // Clone whole gen and nothing more
            createClones(newGen, size);
            break;
    }

    // Fill remaining space with clones
    createClones(newGen, size - newGen.size());

    indivduals = newGen;
}


void Population::createClones(std::vector<Individual> &newGen, const int num) {
    for(int i = 0; i < num; i++) {
        newGen.push_back(indivduals[i]);
    }
}

void Population::createChildren(std::vector<Individual> &newGen, const int num, const float portion) {
    assert(portion >= 0.0 && portion <= 1.0);

    for(int i = 0; i < num ; i++) {
        const Individual &parent1 = getRandomIndividual(size * portion);
        const Individual &parent2 = getRandomIndividual(size * portion);

        newGen.push_back(Individual(parent1, parent2));
    }
}

void Population::createMutants(std::vector<Individual> &newGen, const int num, const float portion) {
    MutationParams params;

    switch(currentStage) {
        case EXPANSION:
            // High addtion, low rest
            params.removalAttempts = 1;
            params.removalChance = 0.1;

            params.coarseMutationAttempts = 1;
            params.coarseMutationChance = 0.1;

            params.fineMutationAttempts = 1;
            params.fineMutationChance = 0.1;

            params.additionAttempts = 4;
            params.additionChance = 0.5;

            break;
        case COMPETITION:
            // High coarse mutation, low rest
            params.removalAttempts = 1;
            params.removalChance = 0.1;

            params.coarseMutationAttempts = 4;
            params.coarseMutationChance = 0.5;

            params.fineMutationAttempts = 1;
            params.fineMutationChance = 0.1;
            
            params.additionAttempts = 1;
            params.additionChance = 0.1;
            break;
        case REFINEMENT:
            // High coarse mutation, high fine mutation, low rest
            params.removalAttempts = 1;
            params.removalChance = 0.1;

            params.coarseMutationAttempts = 4;
            params.coarseMutationChance = 0.5;

            params.fineMutationAttempts = 4;
            params.fineMutationChance = 0.5;
            
            params.additionAttempts = 1;
            params.additionChance = 0.1;
            break;
        case REDUCTION:
            // High removal, low rest
            params.removalAttempts = 2;
            params.removalChance = 0.5;

            params.coarseMutationAttempts = 1;
            params.coarseMutationChance = 0.1;

            params.fineMutationAttempts = 1;
            params.fineMutationChance = 0.1;
            
            params.additionAttempts = 1;
            params.additionChance = 0.1;
            break;
        default:
            // TODO put some error here
            break;
    }

    for(int i = 0; i < num; i++) {
        newGen.push_back(Individual(getRandomIndividual(size * portion), params));
    }
}

void runIndividuals(std::vector<Individual> &indivduals, std::atomic_int &nextIndex, Abc_Frame_t *pAbc, Abc_Ntk_t *pNtk) {
    while(true) {
        const int currentIndex = nextIndex.fetch_add(1, std::memory_order_relaxed);

        if(currentIndex >= indivduals.size()) {
            break;
        }

        // Create a copy of the network given
        Abc_FrameSetCurrentNetwork(pAbc, Abc_NtkDup(pNtk));

        // Calculate the fitness
        indivduals[currentIndex].calculateFitness(pAbc);

        // Delete the network to free its memory
        Abc_FrameDeleteAllNetworks(pAbc);
    }
}

Stage Population::runGeneration(Abc_Frame_t **pAbc, Abc_Ntk_t **pNtks, const int nThreads) {
    std::vector<std::thread> threads;
    std::atomic_int nextIndividual = 0;

    // int n = indivduals.size() / nThreads;
    for(int i = 0; i < nThreads - 1; i++) {
        // threads.emplace_back(runIndividuals, std::ref(indivduals), pAbc[i], pNtks[i], i * n, (i + 1) * n);
        threads.emplace_back(runIndividuals, std::ref(indivduals), std::ref(nextIndividual), pAbc[i], pNtks[i]);
    }

    runIndividuals(indivduals, nextIndividual, pAbc[nThreads - 1], pNtks[nThreads - 1]);

    for(std::thread &thread : threads) {
        thread.join();
    }

    // Show fittest
    std::cout << "Generation: " << generation << std::endl;
    std::cout << getFittest() << std::endl;

    // Sort population
    // TODO fix this so reversing isn't required
    std::sort(indivduals.begin(), indivduals.end());
    std::reverse(indivduals.begin(), indivduals.end());

    // Find new stage
    currentStage = getStage();

    // Create next generation to test
    createNewGeneration();

    // // Create new population
    // int s;
    // std::vector<Individual> newIndividuals;

    // // Take fittest 10% from old population
    // s = (10 * size) / 100;
    // for(int i = 0; i < s; i++) {
    //     newIndividuals.push_back(indivduals[i]);
    // }

    // // From top x % of population, mutate, add or remove genes to make child
    // s = (30 * size) / 100;
    // for(int i = 0; i < s; i++) {
    //     const float choice = randomFloat(0.0, 1.0);

    //     // TODO let genes be mixed and matched
    //     if(choice < CHANGE_CHANCE) {
    //         newIndividuals.push_back(indivduals[i].mutateGenes());
    //     } else if(choice < (CHANGE_CHANCE + REMOVE_CHANCE)) {
    //         newIndividuals.push_back(indivduals[i].removeGenes());
    //     } else {
    //         newIndividuals.push_back(indivduals[i].addGenes());
    //     }
    // }

    // // Mate fittest 50% from old population
    // s = (60 * size) / 100;
    // for(int i = 0; i < s; i++) {
    //     const Individual &parent1 = getRandomIndividual(size / 2);
    //     const Individual &parent2 = getRandomIndividual(size / 2);

    //     newIndividuals.push_back(Individual(parent1, parent2));
    // }

    // indivduals = newIndividuals;
    generation++;

    gensOnCurrentStage++;
    return currentStage;
}

const Individual & Population::getFittest()
{
    return *std::max_element(indivduals.begin(), indivduals.end());
}