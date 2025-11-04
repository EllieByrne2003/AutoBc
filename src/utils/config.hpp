#pragma once

#include <string>
#include <vector>

#include "../genome/genome.hpp"
#include "../individual/individual.hpp"

bool readConfig(const std::string &filename, std::vector<Individual> &seeds, Genome &genome);