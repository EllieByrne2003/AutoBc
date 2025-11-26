#pragma once

#include <string>

class Argument {
private:

protected:
    const std::string name;

public:
    Argument(const std::string &name);

    virtual ~Argument() = default;

    virtual std::string to_string() const = 0;

    friend std::ostream& operator<<(std::ostream& out, const Argument& gene);
};

class BooleanArgument : public Argument {
private:
    const bool present;

public:
    BooleanArgument(const std::string &name, const bool present);

    ~BooleanArgument() override;

    std::string to_string() const override;
};

class NumericArgument : public Argument {
private:
    const int value;

public:
    NumericArgument(const std::string &name, const int value);

    ~NumericArgument() override;

    std::string to_string() const override;
};