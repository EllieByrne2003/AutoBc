#include "genePrototype.hpp"

#include <string>


GenePrototype::GenePrototype(const std::string &name) : 
    GenePrototype(name, "", "", {}) {}

GenePrototype::GenePrototype(const std::string &name, const std::string &prefix, const std::string &suffix) :
    GenePrototype(name, prefix, suffix, {}) {}

GenePrototype::GenePrototype(const std::string &name, const std::vector<Argument *> &arguments) :
    GenePrototype(name, "", "", arguments) {}

GenePrototype::GenePrototype(const std::string &name, const std::string &prefix, const std::string &suffix, const std::vector<Argument *> &arguments) :
    name(name), prefix(prefix), suffix(suffix), arguments(arguments) {}

Gene GenePrototype::createGene() const {
    std::string command = name;

    for(const Argument *arg : arguments) {
        command += arg->to_string();
    }

    return Gene(command, prefix, suffix);
}

GenePrototype::~GenePrototype() {
    for(Argument *arg : arguments) {
        delete arg;
    }
}