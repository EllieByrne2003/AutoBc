#pragma once

#include <string>

class Argument {
private:

protected:
    const std::string name;

public:
    Argument(const std::string &name);

    virtual std::string to_string() const = 0;
};

class BooleanArgument : public Argument {
private:

public:
    BooleanArgument(const std::string &name);

    std::string to_string() const;
};

class NumericArgument : public Argument {
private:
    const int min;
    const int max;

public:
    NumericArgument(const std::string &name, const int min, const int max);

    std::string to_string() const;
};