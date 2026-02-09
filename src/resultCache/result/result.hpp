#pragma once

#include <memory>
#include <chrono>
#include <string>

struct Result {
    const std::chrono::duration<double> time; // TODO remove this
    const bool error;
    const bool equivalent;
    const int numLevels;
    const int numGates;

    Result();
    Result(const Result &other);
    Result(const std::chrono::duration<double> &time, const bool &error, 
        const bool equivalent, const int numLevels, const int numGates);
    ~Result();

    Result operator=(const Result &other);
};