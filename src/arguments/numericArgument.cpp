#include "arguments.hpp"

#include <string>

#include "../utils/random.hpp"

NumericArgument::NumericArgument(const std::string &name, const int min, const int max) :
    Argument(name), min(min), max(max) {
    
}

std::string NumericArgument::to_string() const {
    return " -" + name + std::to_string(randomInt(min, max));
}