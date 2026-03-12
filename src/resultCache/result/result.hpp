#pragma once

#include <memory>
#include <chrono>
#include <string>

struct Result {
    bool error;
    bool equivalent;
    int numLevels;
    int numGates;

    Result();
    Result(const Result &other);
    Result(const bool &error, const bool equivalent, const int numLevels, const int numGates);
    ~Result();

    // Result operator=(const Result &other);
};