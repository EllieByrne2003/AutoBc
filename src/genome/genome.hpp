#pragma once

#include <cassert>
#include <string>
#include <tuple>
#include <vector>

#include "../gene/gene.hpp"

#include "../utils/random.hpp"

struct Argument {
    const std::string name;
    const int min;
    const int max;
    const int step; // TODO something using this value

    Argument(const std::string &name, const int min, const int max) :
        Argument(name, min, max, 1) { }

    Argument(const std::string &name, const int min, const int max, const int step) :
        name(name), min(min), max(max), step(step) {
        assert(min <= max);
        assert(step >= 1);
    }

    std::string to_string() const {
        return " -" + name + " " + std::to_string(randomInt(min, max));
    }
};

struct GenePrototype {
    const std::string name;
    const std::string prefix;
    const std::string postfix;

    const std::vector<Argument> arguments;

    GenePrototype(const std::string &name) : name(name) {}

    GenePrototype(const std::string &name, const std::string &prefix, const std::string &postfix) :
        name(name), prefix(prefix), postfix(postfix) {}

    GenePrototype(const std::string &name, const std::vector<Argument> &arguements) :
        name(name), arguments(arguements) {} 

    GenePrototype(const std::string &name, const std::string &prefix, const std::string &postfix, const std::vector<Argument> &arguements) :
        name(name), prefix(prefix), postfix(postfix), arguments(arguements) {}

    Gene * createGene() const {
        std::string nameWithArgs = name + makeArgs(0);

        return new Gene(nameWithArgs, prefix, postfix);
    }

    std::string makeArgs(const int i) const {
        if(i >= arguments.size()) {
            return "";
        }

        return arguments[i].to_string() + makeArgs(i + 1);
    }
};

class Genome {
private:
    std::vector<const Gene *> genes;
    std::vector<std::pair<int, GenePrototype>> prototypes;
    int totalGenes;

    Genome() {};
    ~Genome();

public:
    static Genome & getInstance() {
        static Genome genome;
        return genome;
    }

    Genome(Genome const &) = delete;
    void operator=(Genome const &) = delete;

    void addPrototype(const std::string &name);
    void addPrototype(const std::string &name, const std::string &prefix, const std::string &postfix);

    void addPrototype(const std::string &name, const std::vector<Argument> &arguements);
    void addPrototype(const std::string &name, const std::string &prefix, const std::string &postfix, const std::vector<Argument> &arguements);

    void addGene(const std::string &name);
    void addGene(const std::string &name, const std::string &prefix, const std::string &postfix);
    void addGene(const std::string &name, std::vector<std::tuple<const std::string, const int, const int>> &arguements);

    const Gene * getRandomGene();
};