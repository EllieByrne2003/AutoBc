#include "argumentPrototypes.hpp"

#include "../utils/random.hpp"

NumericArgumentPrototype::NumericArgumentPrototype(const std::string &name, const int min, const int max) : ArgumentPrototype(name), min(min), max(max) {

}

NumericArgumentPrototype::~NumericArgumentPrototype() {

}

NumericArgument * NumericArgumentPrototype::createArgument() const {
    return new NumericArgument(name, randomInt(min, max));
}