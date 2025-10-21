#include "individual.hpp"

#include <iomanip>
#include <iostream>

#include "../abc.hpp"

#include "../genome/genome.hpp"
#include "../utils/random.hpp"

#define CHANGE_RATE 0.1
#define REMOVE_RATE 0.1
#define ADD_RATE 0.1

#define REMOVAL_NUM 1
#define ADDITION_NUM 1

Individual::Individual() {}


Individual::Individual(const std::vector<Gene> chromosone) : chromosone(chromosone) {}

Individual::Individual(const int chromosoneLength) {
    for(int i = 0; i < chromosoneLength; i++) {
        chromosone.push_back(Genome::getInstance().getRandomGene());
    }
}

Individual::Individual(const Individual &individual, const float mutationRate) {
    // for(int i = 0; i < individual.chromosone.size(); i++) {
    //     if(mutationRate < randomFloat(0.0, 1.0)) {
    //         this->chromosone.push_back(individual.chromosone[i]);
    //     } else {
    //         this->chromosone.push_back(Genome::getInstance().getRandomGene());
    //     }
    // }

    const float mutationType = randomFloat(0.0, 1.0);
    if(mutationType < 1.0 / 3.0) {
        // Mutate random genes
        for(int i = 0; i < individual.chromosone.size(); i++) {
            if(mutationRate > randomFloat(0.0, 1.0)) {
                this->chromosone.push_back(individual.chromosone[i]);
            } else {
                this->chromosone.push_back(Genome::getInstance().getRandomGene());
            }
        }
    } else if(mutationType < 2.0 / 3.0) {
        // Add random genes

        // Add gene before
        if(mutationRate < randomFloat(0.0, 1.0)) {
            chromosone.push_back(Genome::getInstance().getRandomGene());
        }

        for(int i = 0; i < individual.chromosone.size(); i++) {
            this->chromosone.push_back(individual.chromosone[i]);
            
            // Add gene after gene copy
            if(mutationRate > randomFloat(0.0, 1.0)) {
                this->chromosone.push_back(Genome::getInstance().getRandomGene());
            }
        }
    } else {
        // Remove random genes
        for(int i = 0; i < individual.chromosone.size(); i++) {
            if(mutationRate > randomFloat(0.0, 1.0)) {
                this->chromosone.push_back(individual.chromosone[i]);
            }
        }
    }
}

Individual::Individual(const Individual &parent1, const Individual &parent2) {
    // // Simple random chance
    // for(int i = 0; i < parent1.chromosone.size(); i++) {
    //     if(randomBool()) {
    //         this->chromosone.push_back(parent1.chromosone[i]);
    //     } else {
    //         this->chromosone.push_back(parent2.chromosone[i]);
    //     }
    // }

    // // Single point crossover, 0 <= length <= combined length of parents
    // int cross1 = randomInt(0, parent1.chromosone.size() - 1);
    // int cross2 = randomInt(0, parent2.chromosone.size() - 1);
    // for(int i = 0; i < cross1; i++) {
    //     chromosone.push_back(parent1.chromosone[i]);
    // }

    // for(int i = cross2; i < parent2.chromosone.size(); i++) {
    //     chromosone.push_back(parent2.chromosone[i]);
    // }

    // Single point crossover, avg length of parents
    std::pair<int, int> crosses = randomIntPair({0, parent1.chromosone.size() - 1}, {0, parent2.chromosone.size() - 1});
    for(int i = 0; i < crosses.first; i++) {
        chromosone.push_back(parent1.chromosone[i]);
    }

    for(int i = crosses.second; i < parent2.chromosone.size(); i++) {
        chromosone.push_back((parent2.chromosone[i]));
    }
}
    
// Individual::~Individual() {

// }

// TODO take an abc frame later
void Individual::calculateFitness(Abc_Frame_t *pAbc) {
    // Is a clone, fitness known already
    if(calculated) {
        return;
    }

    // Step 1: Run through all commands in chromosone
    for(const Gene &gene : chromosone) {
        // std::cout << gene.getCommand() << std::endl;
        if(gene.execute(pAbc)) {
            error = true;
            return;
        }
    }



    // TODO segfault causes by this command
    // Step 2: Run appropiate commands to get fitness values
    if(Abc_ApiCec(pAbc, 1, (char **) &"cec")) { // TODO possibility of ambiguity
        equivalent = false;
    } else {
        equivalent = true;
    }
    equivalent = true;
    
    nGates = Abc_NtkNodeNum(Abc_FrameReadNtk(pAbc));
    nLevels = Abc_NtkLevel(Abc_FrameReadNtk(pAbc));

    calculated = true;
}

Individual Individual::mutateGenes() {
    Individual child;

    for(int i = 0; i < this->chromosone.size(); i++) {
        if(CHANGE_RATE > randomFloat(0.0, 1.0)) {
            child.chromosone.push_back(this->chromosone[i]);
        } else {
            child.chromosone.push_back(Genome::getInstance().getRandomGene());
        }
    }

    return child;
}

Individual Individual::removeGenes() {
    // Creates a child and removes x genes randomly
    Individual child(this->chromosone);
    for(int i = 0; i < REMOVAL_NUM; i++) {
        const int index = randomInt(0, child.chromosone.size() - 1);
        child.chromosone.erase(child.chromosone.begin() + index);
    }

    // // Creates a child with a random number of genes missing
    // Individual child;

    // for(int i = 0; i < this->chromosone.size(); i++) {
    //     // Reverse, includes the gene if less than rate
    //     if(REMOVE_RATE < randomFloat(0.0, 1.0)) {
    //         child.chromosone.push_back(this->chromosone[i]);
    //     }
    // }

    return child;
}

Individual Individual::addGenes() {
    // Creates child and adds x genes randomly
    Individual child(this->chromosone);

    for(int i = 0; i < ADDITION_NUM; i++) {
        const int index = randomInt(0, child.chromosone.size());
        child.chromosone.insert(child.chromosone.begin() + index, Genome::getInstance().getRandomGene());
    }
    // // Creates child and adds a random number of extra genes
    // Individual child;

    // if(ADD_RATE < randomFloat(0.0, 1.0)) {
    //     child.chromosone.push_back(Genome::getInstance().getRandomGene());
    // }

    // for(int i = 0; i < this->chromosone.size(); i++) {
    //     child.chromosone.push_back(this->chromosone[i]);
           
    //     // Add gene after gene copy
    //     if(ADD_RATE > randomFloat(0.0, 1.0)) {
    //         child.chromosone.push_back(Genome::getInstance().getRandomGene());
    //     }
    // }

    return child;
}

bool operator<(const Individual &left, const Individual &right) {
    if(right.error && !left.error) {
        return false;
    } else if(!right.error && left.error) {
        return true;
    }

    if(!right.equivalent && left.equivalent) {
        return false;
    } else if(!left.equivalent && right.equivalent) {
        return true;
    }

    if(left.nLevels > right.nLevels) {
        return true;
    } else if (left.nLevels < right.nLevels) {
        return false;
    }

    if(left.nGates > right.nGates) {
        return true;
    } else if(left.nGates < right.nGates) {
        return false;
    }

    // Prefer smaller chromosones
    if(left.chromosone.size() > right.chromosone.size()) {
        return true;
    }

    return false;
}

bool operator>(const Individual &left, const Individual &right) {
    if(left.error && !right.error) {
        return false;
    } else if(!left.error && right.error) {
        return true;
    }

    if(!left.equivalent && right.equivalent) {
        return false;
    } else if(!right.equivalent && left.equivalent) {
        return true;
    }

    if(left.nLevels < right.nLevels) {
        return true;
    } else if (left.nLevels > right.nLevels) {
        return false;
    }

    if(left.nGates < right.nGates) {
        return true;
    } else if(left.nGates > right.nGates) {
        return false;
    }

    // Prefer smaller chromosones
    if(left.chromosone.size() < right.chromosone.size()) {
        return true;
    }

    return false;        
}

std::ostream& operator<<(std::ostream& out, const Individual& indivdual) {
    out << "Individual: " << std::endl; // TODO have some id number
    out << indivdual.chromosone.size() << std::endl;

    out << "\tChromosone: ";
    for(const Gene &gene : indivdual.chromosone) {
        out << gene << "; ";
    }
    out << std::endl;
    
    if(indivdual.error) {
        out << "\tError: This script produces errors when run." <<std::endl;
    }

    if(!indivdual.equivalent) {
        out << "\tError: This script produces a non-equivalent circuit" << std::endl;
    }

    out << "\tLevels: " << std::setfill('0') << std::setw(3) << indivdual.nLevels;
    out << "\tGates: " << std::setfill('0') << std::setw(6) << indivdual.nGates;

    return out;
}