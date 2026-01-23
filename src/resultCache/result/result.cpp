#include "result.hpp"
#include <chrono>
#include <limits>

#include "../../abc.hpp"

Result::Result() : Result(std::chrono::duration<double>(0.0), true, false, std::numeric_limits<int>::max(), std::numeric_limits<int>::max()) {

}

Result::Result(
    const std::chrono::duration<double> &time, const bool &error, const bool equivalent, const int numLevels,
    const int numGates) : 
    time(time), error(error), equivalent(equivalent), numLevels(numLevels), 
    numGates(numGates) {
    
}

Result::~Result() {
}