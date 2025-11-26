#include <string>
#include <thread>
#include <vector>
#include <iostream>

#include "constants.hpp"
#include "abc.hpp"

#include "genome/genome.hpp"
#include "individual/individual.hpp"
#include "population/population.hpp"
#include "utils/string.hpp"
#include "utils/config.hpp"

void show_usage(const std::string &command);

int main(int argc, char** rawArgv ) {
    std::vector<Individual> seedExamples;
    Genome &g = Genome::getInstance();
    if(!readConfig("resources/config.json", seedExamples, g)) {
        return 0;
    }

    int nThreads = std::thread::hardware_concurrency();

    int size = DEFAULT_STARTING_SIZE;
    int length = DEFAULT_STARTING_LENGTH;
    int generationLimit = DEFAULT_GENERATION_LIMIT;
    std::string inputFileName;

    // Get input file
    if(argc < 2) {
        std::cout << "Too few arguments, please use format below" << std::endl;
        show_usage(rawArgv[0]);
        
        return -1;
    }

    // Don't have to use strcmp like this
    std::vector<std::string> argv(rawArgv + 1, rawArgv + argc);

    if(argv[0] == "-h") {
        show_usage(rawArgv[0]);
        
        return 0;
    }

    inputFileName = argv[0];
    if(!ends_with(inputFileName, ".aig")) {
        std::cout << "File must be an aig file" << std::endl;
        show_usage(rawArgv[0]);
        
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
            if(i >= argv.size()) {
                std::cout << "Arguement needed after: " << arg << std::endl;
                show_usage(rawArgv[0]);

                return -1;
            }

            nThreads = std::atoi(argv[i++].c_str());
            if(nThreads <= 0) {
                std::cout << "Number of threads must be at least 1." << std::endl;
            }
        } else if(arg == "-s" || arg == "-size") {
            if(i >= argv.size()) {
                std::cout << "Arguement needed after: " << arg << std::endl;
                show_usage(rawArgv[0]);

                return -1;
            }

            size = std::atoi(argv[i++].c_str());
            if(size < 16) {
                std::cout << "Size of population must be at least 16, though higher is recommended." << std::endl;
                return -1;
            }
        } else if(arg == "-l" || arg == "-length") {
            if(i >= argv.size()) {
                std::cout << "Arguement needed after: " << arg << std::endl;
                show_usage(rawArgv[0]);

                return -1;
            }

            length = std::atoi(argv[i++].c_str());
            if(length < 1) {
                std::cout << "Starting length of chromosones must be at least 1" << std::endl;
                show_usage(rawArgv[0]);

                return -1;
            }
        } else if(arg == "-g" || arg == "-generation") {
            if(i > argv.size()) {
                std::cout << "Arguement needed after: " << arg << std::endl;
                show_usage(rawArgv[0]);

                return -1;
            }

            generationLimit = std::atoi(argv[i++].c_str());
            if(generationLimit < 1) {
                std::cout << "The generation limit must be at least 1, though higher is recommended." << std::endl;
                show_usage(rawArgv[0]);

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

    // Create population
    // Population population(256, 5);
    Population population(seedExamples, size, length);

    // Run x number of generations
    for(int i = 0; i < generationLimit; i++) {
        // Run generation
        population.runGeneration(pAbcs, pNtks, nThreads);
    }

    // Cleanup abc
    // TODO cleanup original ntk
    Abc_FrameEnd(pAbc);
    Abc_FrameDeallocate(pAbc);

    Abc_Stop();

    return 0;
}

void show_usage(const std::string &command) {
    std::cout << "usage: " << command << " file [-  num] [-]" << std::endl;

    // TODO decide on all args to use
    // TODO flesh this out

}