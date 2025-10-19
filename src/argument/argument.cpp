#include "argument.hpp"

#include <string>
#include <vector>

Argument::Argument(std::string &name, const int min, const int max) :
    name(name), min(min), max(max) {

}

std::vector<std::string> Argument::getAllValues(std::vector<Argument> &args, const int n) {
    if(n >= args.size()) {
        return getAllValues();
    }

    std::vector<std::string> retValues;
    std::vector<std::string> ownValues = getAllValues();
    std::vector<std::string> values = args[n + 1].getAllValues(args, n + 1);
    for(const std::string &ownValue : ownValues) {
        for(const std::string &value : values) {
            retValues.push_back(ownValue + value);
        }
    }

    return retValues;
}


std::vector<std::string> Argument::getAllValues() {
    std::vector<std::string> values;
    
    for(int value = min; value <= max; value++) {
        values.push_back(" -" +name + " " + std::to_string(value));
    }

    return values;
}