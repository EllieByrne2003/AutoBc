#pragma once

#include <utility>

int randomInt(const int start, const int end);
std::pair<int, int> randomIntPair(const std::pair<int, int> &bounds1, const std::pair<int, int> &bounds2);

float randomFloat(const float start, const float end);

bool randomBool();