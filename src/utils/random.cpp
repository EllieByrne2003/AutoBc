#include "random.hpp"

#include <random>

std::mt19937 & getGenerator() {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    return gen;
}

int randomInt(const int start, const int end) {
    std::uniform_int_distribution<int> dist(start, end);
    return dist(getGenerator());
}

std::pair<int, int> randomIntPair(const std::pair<int, int> &bounds1, const std::pair<int, int> &bounds2) {
    const float ratio = randomFloat(0.0, 1.0);

    const int n1 = static_cast<int>(ratio * (bounds1.second - bounds1.first));
    const int n2 = static_cast<int>(ratio * (bounds2.second - bounds2.first));

    return std::pair<int, int>(n1, n2);
}

float randomFloat(const float start, const float end) {
    std::uniform_real_distribution<float> dist(start, end);
    return dist(getGenerator());
}

bool randomBool() {
    return randomInt(0, 1) == 0;
}