#include "genePrototype.hpp"

#include <string>
#include <memory>

GenePrototype::GenePrototype(const std::string &name) : 
    GenePrototype(name, "", "", {}) {}

GenePrototype::GenePrototype(const std::string &name, const std::string &prefix, const std::string &suffix) :
    GenePrototype(name, prefix, suffix, {}) {}

GenePrototype::GenePrototype(const std::string &name, const std::vector<std::shared_ptr<ArgumentPrototype>> &arguments) :
    GenePrototype(name, "", "", arguments) {

}

GenePrototype::GenePrototype(const std::string &name, const std::string &prefix, const std::string &suffix, const std::vector<std::shared_ptr<ArgumentPrototype>> &arguments) :
    name(name), prefix(prefix), suffix(suffix), arguments(arguments) {

}

Gene GenePrototype::createGene() const {
    std::vector<std::shared_ptr<Argument>> args;

    for(const std::shared_ptr<ArgumentPrototype> &argPrototype : arguments) {
        args.push_back(std::shared_ptr<Argument>(argPrototype->createArgument()));
    }

    return Gene(name, prefix, suffix);
    // return Gene(name, prefix, suffix, args);
}