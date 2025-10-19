import random

from individual import Individual
from individual import GENOME

# GENOME = ["balance", "rewrite", "refactor", "resub", "fraig", "cleanup"]

class Population(object):
    def __init__(self, limit, chromosoneLength):
        self.generation = 1
        self.limit = limit
        self.individuals = [Individual([random.choice(GENOME) for _ in range(chromosoneLength)]) for _ in range(self.limit)]

    def createNewGeneration(self):
        self.individuals.sort(key=lambda x:x.fitness)

        newIndividuals = []

        s = int((10 * self.limit) / 100)
        newIndividuals.extend(self.individuals[:s])

        s = int((90 * self.limit) / 100 )
        for _ in range(s):
            parent1 = random.choice(self.individuals[:50])
            parent2 = random.choice(self.individuals[:50])

            child = parent1.mate(parent2)

            newIndividuals.append(child)
    
        self.individuals = newIndividuals
        self.generation += 1

    def fittest(self):
        return min(self.individuals, key=lambda x: x.fitness)
