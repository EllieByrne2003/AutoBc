#include "chromosone.hpp"

bool operator<(const Chromosone &left, const Chromosone &right) {
    if(!left.equivalent){
        return true;
    }

    if(left.nLevels > right.nLevels) {
        return true;
    } else if (left.nLevels < right.nLevels) {
        return false;
    }

    if(left.nGates > right.nGates) {
        return true;
    }

    return false;
}

bool operator>(const Chromosone &left, const Chromosone &right) {
    if(!right.equivalent){
        return true;
    }

    if(left.nLevels < right.nLevels) {
        return true;
    } else if (left.nLevels > right.nLevels) {
        return false;
    }

    if(left.nGates < right.nGates) {
        return true;
    }

    return false;        
}