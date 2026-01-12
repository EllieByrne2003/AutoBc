#pragma once

#include <vector>
#include <chrono>

#include <boost/json/object.hpp>
namespace json = boost::json;

// #include "../chromosone/chromosone.hpp"
#include "../gene/gene.hpp"

typedef struct Abc_Frame_t_ Abc_Frame_t;

class Individual
{
private:
    std::vector<Gene> chromosone;

    // Fitness information
    bool calculated = false;
    bool error = false;
    bool equivalent; // Whether the output circuit is equivalent
    int nGates;
    int nLevels;
    std::chrono::duration<double> timeElapsed;

public:
    // Initiaisation, create random chromosone of this length
    Individual();
    Individual(const std::vector<Gene> &chromosone);
    Individual(const int chromosoneLength);

    // Cloning, mutating and mating for working with individuals
    Individual(const Individual &individual, const float mutationRate);
    Individual(const Individual &parent1, const Individual &parent2);

    Individual mutateGenes();
    Individual removeGenes();
    Individual addGenes();
    
    // ~Individual();

    void calculateFitness(Abc_Frame_t *pAbc);

    // Comparison operators for sorting by fitness
    friend bool operator<(const Individual &left, const Individual &right);
    friend bool operator>(const Individual &left, const Individual &right);

    // 
    friend std::ostream& operator<<(std::ostream& out, const Individual& indivdual);

    json::object to_json() const;
};