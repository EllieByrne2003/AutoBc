#include "arguments.hpp"

#include <string>

#include "../utils/random.hpp"

BooleanArgument::BooleanArgument(const std::string &name) : Argument(name) {}

std::string BooleanArgument::to_string() const {
    return randomBool() ? " -" + name : "";
}