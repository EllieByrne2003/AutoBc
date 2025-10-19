import os
import random
import re

from subprocess import run

from population import Population

# genome = ["balance", "rewrite", "refactor", "resub", "fraig", "sweep", "cleanup", ""]
genome = ["balance", "rewrite", "refactor", "resub", "fraig", "", "cleanup", ""]

def intit_population(limit, length):
    population = []

    for _ in range(limit):
        population.append(init_chromosone(length))

    return population

def run_generation(population):
    # sort by fitness
    population.sort(key=chromosone_fitness)

    newPopulation = []

    # Clone top 10%
    s = int((10 * len(population)) / 100)
    newPopulation.extend(population[:s])

    # Mate some of top 50% together
    s = int((90 * len(population)) / 100)
    for _ in range(s):
        parent1 = random.choice(population[:50])
        parent2 = random.choice(population[:50])
        child = mate_chromosones(parent1, parent2)

        newPopulation.append(child)

    return newPopulation


def init_chromosone(length):
    c = []

    for _ in range(0, length):
        c.append(random.choice(genome))
    
    return c

def chromosone_fitness(chromosone):
    data = run(["./resources/abc", "-c", "read resources/circuits/i10.aig;" + convert_to_command(chromosone) + "print_stats;"], capture_output=True, text=True)

    nGates = re.findall(r'\d+', data.stdout)[-2]
    # print(data.stdout.split('\n')[-2])
    # print("Gates: " + nGates)

    return nGates

def mate_chromosones(parent1, parent2):
    child = []

    for i in range(len(parent1)):
        gene = random.choice([parent1[i], parent2[i]])

        child.append(gene)

    return child

def convert_to_command(chromosone):
    res = ""

    for gene in chromosone:
        res += gene
        res += "; "

    return res

def main():
    pop = Population(16, 20)

    # pop.createNewGeneration()

    while pop.generation < 1:
        pop.createNewGeneration()

    fittestIndividual = pop.fittest()
    print(fittestIndividual.convertToScript())
    print(fittestIndividual.fitness)

    # print("hi")

    # pop = intit_population(10, 50)
    # # run_generation(pop)

    # for _ in range(20):
    #     pop = run_generation(pop)

    # pop.sort(key=chromosone_fitness)

    # # c = init_chromosone(10)
    # c = pop[0]
    # print(c)

    # # os.system("ls")

    # response = run(["./resources/abc", "-c", "read resources/circuits/i10.aig;" + convert_to_command(c) + "print_stats;"], capture_output=True, text=True)

    # print(response.stdout)

    # chromosone_fitness(c)

if __name__ == "__main__":
    main()