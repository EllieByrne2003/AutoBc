#include <vector>
#include <iostream>

#include "constants.hpp"
#include "abc.hpp"

#include "genome/genome.hpp"
#include "individual/individual.hpp"
#include "population/population.hpp"
#include "utils/string.hpp"
#include "utils/config.hpp"

int main(int argc, char** rawArgv ) {
    std::vector<Individual> seedExamples;
    Genome &g = Genome::getInstance();
    if(!readConfig("resources/config.json", seedExamples, g)) {
        return 0;
    }

    int nThreads = 16;

    int size = DEFAULT_STARTING_SIZE;
    int length = DEFAULT_STARTING_LENGTH;
    int generationLimit = DEFAULT_GENERATION_LIMIT;
    std::string inputFileName;

    // Get input file
    if(argc < 2) {
        std::cout << "Too few arguments, please use format below" << std::endl;
        std::cout << "autobc [inputFile] [args...]" << std::endl;
        return -1;
    }

    // Don't have to use strcmp like this
    std::vector<std::string> argv(rawArgv + 1, rawArgv + argc);

    if(argv[0] == "-h") {
        // TODO list all args and how to use them here
        return 0;
    }

    inputFileName = argv[0];
    if(!ends_with(inputFileName, ".aig")) {
        std::cout << "File must be an aig file" << std::endl;
        return 0;
    }
    // TODO check if above is a valid file

    // Get arguments
    int i = 1;
    while(i < argv.size()) {
        std::string arg = argv[i++];

        if(arg == "-h") {
            // TODO list all args and how to use them here
            return 0;
        } else if(arg == "-t" || arg == "-threads") {
            if(i + 1 >= argv.size()) {
                std::cout << "Arguement needed after: " << arg << std::endl;
                return -1;
            }

            nThreads = std::atoi(argv[i++].c_str());
            if(nThreads <= 0) {
                std::cout << "Number of threads must be at least 1." << std::endl;
            }
        } else if(arg == "-s" || arg == "-size") {
            if(i + 1 >= argv.size()) {
                std::cout << "Arguement needed after: " << arg << std::endl;
                return -1;
            }

            size = std::atoi(argv[i++].c_str());
            if(size < 16) {
                std::cout << "Size of population must be at least 16, though higher is recommended." << std::endl;
                return -1;
            }
        } else if(arg == "-l" || arg == "-length") {
            if(i + 1 >= argv.size()) {
                std::cout << "Arguement needed after: " << arg << std::endl;
                return -1;
            }

            length = std::atoi(argv[i++].c_str());
            if(length < 1) {
                std::cout << "Starting length of chromosones must be at least 1" << std::endl;
                return -1;
            }
        } else if(arg == "-g" || arg == "-generation") {
            if(i + 1 > argv.size()) {
                std::cout << "Arguement needed after: " << arg << std::endl;
                return -1;
            }

            generationLimit = std::atoi(argv[i++].c_str());
            if(generationLimit < 1) {
                std::cout << "The generation limit must be at least 1, though higher is recommended." << std::endl;
                return -1;
            }            
        }

        // TODO implement input file, seed file, command file
    }

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
    Cmd_CommandExecute(pAbc, ("read " + inputFileName).c_str());
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

    // genome.addGene("balance");
    // genome.addGene("resub");
    // genome.addGene("rewrite");
    // genome.addGene("refactor");
    // genome.addGene("fraig");
    // genome.addGene("cleanup"); // Exepect this gone
    // genome.addGene("renode", "",  "strash");
    // // genome.addGene("strash");

    // genome.addPrototype("balance", std::vector<Argument>{});
    // // genome.addPrototype("resub");
    // genome.addPrototype("rewrite", std::vector<Argument>{});
    // genome.addPrototype("refactor", std::vector<Argument>{Argument("N", 2, 15), Argument("M", 1, 16)}); // M has no max
    // genome.addPrototype("fraig");
    // genome.addPrototype("cleanup"); // Exepect this gone
    // genome.addPrototype("renode", "",  "strash", std::vector<Argument>{Argument("K", 4, 15), Argument("C", 1, 8)}); // Memory usage explodes if using -C 16 or close to it
    // genome.addPrototype("resub", std::vector<Argument>{Argument("K", 4, 16)});

    // // Create seed examples
    // std::vector<Individual> seedExamples;
    // seedExamples.push_back(Individual(std::vector<Gene>{
    //     Gene("renode -K 10 -C 3", "", "strash"),
    //     Gene("resub -K 13", "", ""),
    //     Gene("renode -K 7 -C 16", "", "strash"),
    //     Gene("renode -K 7 -C 16", "", "strash"),
    //     Gene("resub -K 9", "", ""),
    //     Gene("renode -K 14 -C 13", "", "strash"),
    //     Gene("rewrite", "", ""),
    //     Gene("renode -K 11 -C 8", "", "strash"),
    //     Gene("resub -K 14", "", ""),
    //     Gene("renode -K 15 -C 8", "", "strash")
    // }));

    // seedExamples.push_back(Individual(std::vector<Gene>{
    //     Gene("renode -K 12 -C 6", "", "strash"),
    //     Gene("renode -K 15 -C 11", "", "strash"),
    //     Gene("resub -K 10", "", ""),
    //     Gene("renode -K 13 -C 15", "", "strash"),
    //     Gene("balance", "", ""),
    //     Gene("renode -K 14 -C 11", "", "strash"),
    //     Gene("resub -K 15", "", ""),
    //     Gene("renode -K 6 -C 8", "", "strash"),
    //     Gene("balance", "", ""),
    //     Gene("fraig", "", "")
    // }));

    // seedExamples.push_back(Individual(std::vector<Gene>{
    //     Gene("renode -K 6 -C 8", "", "strash"),
    //     Gene("resub -K 9", "", ""),
    //     Gene("renode -K 15 -C 12", "", "strash"),
    //     Gene("resub -K 12", "", ""),
    //     Gene("renode -K 9 -C 16", "", "strash"),
    //     Gene("renode -K 11 -C 14", "", "strash"),
    //     Gene("renode -K 11 -C 14", "", "strash"),
    //     Gene("balance", "", ""),
    //     Gene("rewrite", "", ""),
    //     Gene("fraig", "", ""),
    //     Gene("resub -K 13", "", ""),
    // }));

    // Create population
    // Population population(256, 5);
    Population population(seedExamples, size, length);

    // Run x number of generations
    for(int i = 0; i < generationLimit; i++) {
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