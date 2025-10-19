#pragma once

#include <string>
#include <vector>

class Argument {
private:
    const std::string name;
    const int min;
    const int max;

public:
    Argument(std::string &name, const int min, const int max);

    std::vector<std::string> getAllValues(std::vector<Argument> &args, const int n);
    std::vector<std::string> getAllValues();
};