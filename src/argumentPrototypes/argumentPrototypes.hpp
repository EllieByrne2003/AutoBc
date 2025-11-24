#pragma once

#include <string>

#include "../arguments/arguments.hpp"

class ArgumentPrototype {
private:

protected:
    const std::string name;

public:
    ArgumentPrototype(const std::string &name);
    virtual ~ArgumentPrototype() = default;

    virtual Argument * createArgument() const = 0;
};

class BooleanArgumentPrototype : public ArgumentPrototype {
private:

protected:

public:
    BooleanArgumentPrototype(const std::string &name);
    ~BooleanArgumentPrototype() override;

    BooleanArgument * createArgument() const override;
};

class NumericArgumentPrototype : public ArgumentPrototype {
private:
    const int min;
    const int max;

protected:

public:
    NumericArgumentPrototype(const std::string &name, const int min, const int max);
    ~NumericArgumentPrototype() override;

    NumericArgument * createArgument() const override;
};