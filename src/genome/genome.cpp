#include "genome.hpp"

#include <string>
#include <vector>

#include "../utils/random.hpp"

Genome::~Genome() {
    for(int i = 0; i < genes.size(); i++) {
        delete genes[i];
    }
}


void Genome::addPrototype(const std::string &name, const std::string &inputType, const std::string outputType) {
    prototypes.push_back(GenePrototype(name, inputType, outputType));
}

// void Genome::addPrototype(const std::string &name, const std::string &prefix, const std::string &postfix) {
//     prototypes.push_back(GenePrototype(name, prefix, postfix));
// }

void Genome::addPrototype(const std::string &name, const std::string &inputType, const std::string outputType, const std::vector<std::shared_ptr<ArgumentPrototype>> &arguements) {
    prototypes.push_back(GenePrototype(name, inputType, outputType, arguements));
}

// void Genome::addPrototype(const std::string &name, const std::string &prefix, const std::string &postfix, const std::vector<std::shared_ptr<ArgumentPrototype>> &arguements) {
//     prototypes.push_back(GenePrototype(name, prefix, postfix, arguements));
// }

// void Genome::addGene(const std::string &name) {
//     genes.push_back(new Gene(name));
// }

// void Genome::addGene(const std::string &name, const std::string &prefix, const std::string &postfix) {
//     genes.push_back(new Gene(name, prefix, postfix));
// }


// void Genome::addGene(const std::string &name, std::vector<std::tuple<const std::string, const int, const int>> &arguements) {
//     for(const auto & argument : arguements) {
//         const std::string &argname = std::get<0>(argument);
//         const int argMin = std::get<1>(argument);
//         const int argMax = std::get<2>(argument);

//         for(int value = argMin; value <= argMax; value++) {
//             genes.push_back(new Gene(name + " -" + argname + " " + std::to_string(value)));
//         }
//     }
// }

const Gene Genome::getRandomGene() {
    return prototypes[randomInt(0, prototypes.size() - 1)].createGene();
}