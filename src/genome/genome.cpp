#include "genome.hpp"

#include <string>
#include <vector>

#include "../utils/random.hpp"

Genome::~Genome() {
    for(int i = 0; i < genes.size(); i++) {
        delete genes[i];
    }
}


void Genome::addPrototype(const std::string &name) {
    prototypes.push_back(GenePrototype(name));
}

void Genome::addPrototype(const std::string &name, const std::string &prefix, const std::string &postfix) {
    prototypes.push_back(GenePrototype(name, prefix, postfix));
}


void Genome::addPrototype(const std::string &name, const std::vector<std::shared_ptr<ArgumentPrototype>> &arguements) {
    // int newGenes = 1;
    
    // for(const Argument &arg : arguements) {
    //     // If min == max, theres 1 possibility
    //     newGenes *= (1 + (arg.max - arg.min));
    // }

    prototypes.push_back(GenePrototype(name, arguements));

    // totalGenes += newGenes;
}

#include <iostream>

void Genome::addPrototype(const std::string &name, const std::string &prefix, const std::string &postfix, const std::vector<std::shared_ptr<ArgumentPrototype>> &arguements) {
    // int newGenes = 1;

    // for(const Argument &arg : arguements) {
    //     // If min == max, theres 1 possibility
    //     newGenes *= (1 + (arg.max - arg.min));
    // }

    prototypes.push_back(GenePrototype(name, prefix, postfix, arguements));
    std::cout << "args: " << arguements.size() << std::endl;
    for(auto arg : arguements) {
        std::cout << arg->createArgument()->to_string();
    }
    // totalGenes += newGenes;
}

void Genome::addGene(const std::string &name) {
    genes.push_back(new Gene(name));
}

void Genome::addGene(const std::string &name, const std::string &prefix, const std::string &postfix) {
    genes.push_back(new Gene(name, prefix, postfix));
}


void Genome::addGene(const std::string &name, std::vector<std::tuple<const std::string, const int, const int>> &arguements) {
    for(const auto & argument : arguements) {
        const std::string &argname = std::get<0>(argument);
        const int argMin = std::get<1>(argument);
        const int argMax = std::get<2>(argument);

        for(int value = argMin; value <= argMax; value++) {
            genes.push_back(new Gene(name + " -" + argname + " " + std::to_string(value)));
        }
    }
}

const Gene Genome::getRandomGene() {
    // const int n = randomInt(0, totalGenes);
    
    // int currentGene = 0;
    // for(const auto &prototype : prototypes) {
    //     if(n > currentGene + prototype.first) {
    //         currentGene += prototype.first;
    //         continue;
    //     }

    //     return prototype.second.createGene();
    // }

    return prototypes[randomInt(0, prototypes.size() - 1)].createGene();

    assert(false);

    // const auto pro = prototypes[randomInt(0, prototypes.size() - 1)];

    // std::cout << pro.second.createGene()->getCommand() << std::endl;


    // return genes[randomInt(0, genes.size() - 1)];
}