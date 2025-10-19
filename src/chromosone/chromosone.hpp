#pragma once

#include <vector>

#include "../gene/gene.hpp"

class Chromosone {
private:
    std::vector<const Gene *> genes;

    // Fitness information
    bool equivalent; // Whether the output circuit is equivalent
    int nGates;
    int nLevels;

public:
    Chromosone(const int initialLength);

    // Have a check if fitness already found (clones will have already)
    void calculateFitness();

    friend bool operator<(const Chromosone &left, const Chromosone &right);
    friend bool operator>(const Chromosone &left, const Chromosone &right);
};