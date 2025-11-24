#include "arguments.hpp"

#include <string>

BooleanArgument::BooleanArgument(const std::string &name, const bool present) : Argument(name), present(present) {

}

BooleanArgument::~BooleanArgument() {
    
}

std::string BooleanArgument::to_string() const {
    return present ? " -" + name : "";
}