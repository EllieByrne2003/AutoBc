#include "argumentPrototypes.hpp"

#include "../utils/random.hpp"

BooleanArgumentPrototype::BooleanArgumentPrototype(const std::string &name) : ArgumentPrototype(name) {

}

BooleanArgumentPrototype::~BooleanArgumentPrototype() {

}

BooleanArgument * BooleanArgumentPrototype::createArgument() const {
    return new BooleanArgument(name, randomBool());
}