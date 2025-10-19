#include <vector>

#include "abc.hpp"

#include "genome/genome.hpp"
#include "individual/individual.hpp"
#include "population/population.hpp"


int main() {
    // std::cout << "Hello world" << std::endl;
    const int nThreads = 16;

    // Initialise abc
    Abc_Start();

    // Get frame
    Abc_Frame_t * pAbc = Abc_FrameAllocate();
    Abc_FrameInit(pAbc);

    Abc_Frame_t * pAbcs[nThreads];
    for(int i = 0; i < nThreads; i++) {
        pAbcs[i] = Abc_FrameAllocate();
        Abc_FrameInit(pAbcs[i]);
    }

    // Abc_Frame_t *pAbc2 = Abc_FrameAllocate();
    // Abc_FrameInit(pAbc2);

    // Load file into aig format and keep copy for easy use later
    Abc_Ntk_t * originalNtk; // TODO may need to delete ntk and/or backup
    Cmd_CommandExecute(pAbc, "read resources/circuits/i10.aig");
    originalNtk = Abc_NtkDup(Abc_FrameReadNtk(pAbc));


    Abc_Ntk_t * pNtks[nThreads];
    for(int i = 0; i < nThreads; i++) {
        pNtks[i] = Abc_NtkDup(originalNtk);
    }

    // Abc_FrameSetCurrentNetwork(pAbc2, ntkClone);

    // Cmd_CommandExecute(pAbc, "print_stats");
    // Cmd_CommandExecute(pAbc2, "cleanup");
    // Cmd_CommandExecute(pAbc2, "print_stats");

    // int retValue = Abc_ApiCec(pAbc2, 1, (char **) &"cec");
    // std::cout << "retVaue: " << retValue << std::endl;

    // Initialise genome
    Genome &genome = Genome::getInstance();

    genome.addGene("balance");
    genome.addGene("resub");
    genome.addGene("rewrite");
    genome.addGene("refactor");
    genome.addGene("fraig");
    genome.addGene("cleanup"); // Exepect this gone
    genome.addGene("renode", "",  "strash");
    // genome.addGene("strash");

    genome.addPrototype("balance", std::vector<Argument>{});
    // genome.addPrototype("resub");
    genome.addPrototype("rewrite", std::vector<Argument>{});
    genome.addPrototype("refactor", std::vector<Argument>{Argument("N", 2, 15), Argument("M", 1, 16)}); // M has no max
    genome.addPrototype("fraig");
    genome.addPrototype("cleanup"); // Exepect this gone
    genome.addPrototype("renode", "",  "strash", std::vector<Argument>{Argument("K", 4, 15), Argument("C", 1, 16)});
    genome.addPrototype("resub", std::vector<Argument>{Argument("K", 4, 16)});

    // Create population
    Population population(256, 5);

    // Run x number of generations
    for(int i = 0; i < 25; i++) {
        // Run generation
        population.runGeneration(pAbcs, pNtks, nThreads);
    }
    //   Run generation
    //   Show fitest and fitness values

    // Take fittest in population
    // Run it again but after have file written out and command shown

    // Cleanup abc
    // TODO cleanup original ntk
    Abc_FrameEnd(pAbc);
    Abc_FrameDeallocate(pAbc);

    Abc_Stop();

    return 0;
}