#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../argumentPrototypes/argumentPrototypes.hpp"
#include "../gene/gene.hpp"

class GenePrototype {
private:
    const std::string name;
    // const std::string prefix;
    // const std::string suffix;
    const std::string inputType;
    const std::string outputType;

    std::vector<std::shared_ptr<ArgumentPrototype>> arguments;

public:
    GenePrototype(const std::string &name, const std::string &inputType, const std::string outputType);
    // GenePrototype(const std::string &name, const std::string &prefix, const std::string &suffix);

    GenePrototype(const std::string &name, const std::string &inputType, const std::string outputType, const std::vector<std::shared_ptr<ArgumentPrototype>> &arguments);
    // GenePrototype(const std::string &name, const std::string &prefix, const std::string &suffix, const std::vector<std::shared_ptr<ArgumentPrototype>> &arguments);

    Gene createGene() const;
};