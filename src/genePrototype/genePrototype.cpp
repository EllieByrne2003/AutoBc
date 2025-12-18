#include "genePrototype.hpp"

#include <string>
#include <memory>

GenePrototype::GenePrototype(const std::string &name, const std::string &inputType, const std::string outputType) : 
    GenePrototype(name, inputType, outputType, {}) {}

// GenePrototype::GenePrototype(const std::string &name, const std::string &prefix, const std::string &suffix) :
//     GenePrototype(name, prefix, suffix, {}) {}

GenePrototype::GenePrototype(const std::string &name, const std::string &inputType, const std::string outputType, const std::vector<std::shared_ptr<ArgumentPrototype>> &arguments) :
    name(name), inputType(inputType), outputType(outputType), arguments(arguments) {

}

// GenePrototype::GenePrototype(const std::string &name, const std::string &prefix, const std::string &suffix, const std::vector<std::shared_ptr<ArgumentPrototype>> &arguments) :
//     name(name), prefix(prefix), suffix(suffix), arguments(arguments) {

// }

Gene GenePrototype::createGene() const {
    std::vector<std::shared_ptr<Argument>> args;

    for(const std::shared_ptr<ArgumentPrototype> &argPrototype : arguments) {
        args.push_back(std::shared_ptr<Argument>(argPrototype->createArgument()));
    }

    return Gene(name, inputType, outputType, args);
}