#include "arguments.hpp"

#include <string>

Argument::Argument(const std::string &name) : name(name) {}

std::ostream& operator<<(std::ostream& out, const Argument &gene) {
    out << gene.to_string();

    return out;
}