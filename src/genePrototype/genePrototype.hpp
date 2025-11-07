#pragma once

#include <string>
#include <vector>

#include "../arguments/arguments.hpp"
#include "../gene/gene.hpp"

class GenePrototype {
private:
    const std::string name;
    const std::string prefix;
    const std::string suffix;

    const std::vector<Argument *> arguments;

public:
    GenePrototype(const std::string &name);
    GenePrototype(const std::string &name, const std::string &prefix, const std::string &suffix);

    GenePrototype(const std::string &name, const std::vector<Argument *> &arguments);
    GenePrototype(const std::string &name, const std::string &prefix, const std::string &suffix, const std::vector<Argument *> &arguments);

    ~GenePrototype();

    Gene createGene() const;
};