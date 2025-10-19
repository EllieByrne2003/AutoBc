import random
import re

from subprocess import run

MUTATION_RATE = 0.1
GENOME = ["balance", "rewrite", "refactor", "resub", "fraig", "cleanup", "renode ; strash"]

def convert_to_command(chromosone):
    res = ""

    for gene in chromosone:
        res += gene
        res += "; "

    return res

class Individual(object):
    def __init__(self, chromosone):
        self.chromosone = chromosone
        self.fitness = self.calcFitness()

    def calcFitness(self):
        data = run(["./../resources/abc", "-c", "read ../resources/circuits/sqrt.aig;" + convert_to_command(self.chromosone) + "cec; print_stats;"], capture_output=True, text=True)

        # TODO maybe use both?
        if data.stdout.split("\n")[-3] == "Miter computation has failed.":
            return int(9999999999999) 
        nLevels = int(re.findall(r'\d+', data.stdout)[-1])
        nGates =  int(re.findall(r'\d+', data.stdout)[-2])
        return nGates

    def mate(self, partner):
        childChromosone = []

        for gene1, gene2 in zip(self.chromosone, partner.chromosone):
            prob = random.random()

            if prob < MUTATION_RATE:
                childChromosone.append(random.choice(GENOME))
            elif prob < (1.0 - MUTATION_RATE) / 2:
                childChromosone.append(gene1)
            else:
                childChromosone.append(gene2)
        
        return Individual(childChromosone)

    def convertToScript(self):
        # TODO let it use it's own file instead
        script = "read resources/circuits/sqrt.aig;"

        for gene in self.chromosone:
            script += " " + gene + ";"

        script += " print_stats;"
        return script