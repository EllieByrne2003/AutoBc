#include "population.hpp"

#include <algorithm>
#include <iostream>
#include <ostream>
#include <vector>
#include <random>
#include <thread>
#include <atomic>

#include "../abc.hpp"
#include "../utils/random.hpp"

#define CHANGE_CHANCE (1.0 / 3.0)
#define REMOVE_CHANCE (1.0 / 3.0)
#define ADD_CHANCE    (1.0 / 3.0)

Population::Population(const int size, const int chromosoneLength) : size(size) {
    for(int i = 0; i < size; i++) {
        indivduals.push_back(Individual(chromosoneLength));
    }
}

const Individual & Population::getRandomIndividual(const int sampleSize) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<size_t> dist(0, sampleSize - 1);
    return indivduals[dist(gen)];
}

// void runIndividuals(std::vector<Individual> &indivduals, Abc_Frame_t *pAbc, Abc_Ntk_t *pNtk, const int start, const int end) {
//     for(int i = start; i < end; i++) {
//         // Create a copy of the network given
//         Abc_FrameSetCurrentNetwork(pAbc, Abc_NtkDup(pNtk));

//         indivduals[i].calculateFitness(pAbc);

//         // Delete the network and free the memory
//         Abc_FrameDeleteAllNetworks(pAbc);
//     }
// }

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

    // for(int i = start; i < end; i++) {
    //     // Create a copy of the network given
    //     Abc_FrameSetCurrentNetwork(pAbc, Abc_NtkDup(pNtk));

    //     indivduals[i].calculateFitness(pAbc);

    //     // Delete the network and free the memory
    //     Abc_FrameDeleteAllNetworks(pAbc);
    // }
}

void Population::runGeneration(Abc_Frame_t **pAbc, Abc_Ntk_t **pNtks, const int nThreads) {
    std::vector<std::thread> threads;
    std::atomic_int nextIndividual = 0;

    int n = indivduals.size() / nThreads;
    for(int i = 0; i < nThreads - 1; i++) {
        // threads.emplace_back(runIndividuals, std::ref(indivduals), pAbc[i], pNtks[i], i * n, (i + 1) * n);
        threads.emplace_back(runIndividuals, std::ref(indivduals), std::ref(nextIndividual), pAbc[i], pNtks[i]);
    }

    runIndividuals(indivduals, nextIndividual, pAbc[nThreads - 1], pNtks[nThreads - 1]);

    // // TODO fix this, have threads pull from the vector until the end instead of predefined splits
    // for(int i = n * (nThreads - 1); i < indivduals.size(); i++) {
    //     Abc_FrameSetCurrentNetwork(pAbc[nThreads - 1], Abc_NtkDup(pNtks[nThreads - 1]));
        
    //     indivduals[i].calculateFitness(pAbc[nThreads - 1]);

    //     // Duplicate network and set it
    //     Abc_FrameDeleteAllNetworks(pAbc[nThreads - 1]);
    // }

    for(std::thread &thread : threads) {
        thread.join();
    }

    // Show fittest
    std::cout << "Generation: " << generation << std::endl;
    std::cout << getFittest() << std::endl;

    // Sort population
    std::sort(indivduals.begin(), indivduals.end());
    std::reverse(indivduals.begin(), indivduals.end());

    // Create new population
    int s;
    std::vector<Individual> newIndividuals;

    // Take fittest 10% from old population
    s = (10 * size) / 100;
    for(int i = 0; i < s; i++) {
        newIndividuals.push_back(indivduals[i]);
    }

    // From top x % of population, mutate, add or remove genes to make child
    s = (30 * size) / 100;
    for(int i = 0; i < s; i++) {
        const float choice = randomFloat(0.0, 1.0);

        if(choice < CHANGE_CHANCE) {
            newIndividuals.push_back(indivduals[i].mutateGenes());
        } else if(choice < (CHANGE_CHANCE + REMOVE_CHANCE)) {
            newIndividuals.push_back(indivduals[i].removeGenes());
        } else {
            newIndividuals.push_back(indivduals[i].addGenes());
        }
    }

    // // Mutate fittest 20% from old population
    // s = (20 * size) / 100;
    // for(int i = 0; i < s; i++) {
    //     newIndividuals.push_back((Individual(indivduals[i], 0.1)));
    // }

    // Mate fittest 50% from old population
    s = (60 * size) / 100;
    for(int i = 0; i < s; i++) {
        const Individual &parent1 = getRandomIndividual(size / 2);
        const Individual &parent2 = getRandomIndividual(size / 2);

        newIndividuals.push_back(Individual(parent1, parent2));
    }

    indivduals = newIndividuals;
    generation++;
}

const Individual & Population::getFittest()
{
    return *std::max_element(indivduals.begin(), indivduals.end());
}