#include "arguments.hpp"

#include <string>

NumericArgument::NumericArgument(const std::string &name, const int value) :
    Argument(name), value(value) {
    
}

NumericArgument::~NumericArgument() {
    
}

std::string NumericArgument::to_string() const {
    return " -" + name + " " + std::to_string(value);
}


json::object NumericArgument::to_json() const {
    return json::object({
        {"name", name},
        {"value", value}
    });
}