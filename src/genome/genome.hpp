#pragma once

#include <cassert>
#include <string>
#include <vector>

#include "../gene/gene.hpp"
#include "../genePrototype/genePrototype.hpp"

class Genome {
private:
    std::vector<const Gene *> genes;
    std::vector<GenePrototype> prototypes;

    Genome() {};
    ~Genome();

public:
    static Genome & getInstance() {
        static Genome genome;
        return genome;
    }

    Genome(Genome const &) = delete;
    void operator=(Genome const &) = delete;

    void addPrototype(const std::string &name, const std::string &inputType, const std::string outputType);
    // void addPrototype(const std::string &name, const std::string &prefix, const std::string &postfix);

    void addPrototype(const std::string &name, const std::string &inputType, const std::string outputType, const std::vector<std::shared_ptr<ArgumentPrototype>> &arguements);
    // void addPrototype(const std::string &name, const std::string &prefix, const std::string &postfix, const std::vector<std::shared_ptr<ArgumentPrototype>> &arguements);

    // void addGene(const std::string &name);
    // void addGene(const std::string &name, const std::string &prefix, const std::string &postfix);
    // void addGene(const std::string &name, std::vector<std::tuple<const std::string, const int, const int>> &arguements);

    const Gene getRandomGene();
};