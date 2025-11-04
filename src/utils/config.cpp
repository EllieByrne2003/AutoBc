#include "config.hpp"

#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <fstream>
#include <iterator>
#include <iostream>

#include <boost/json/parse.hpp>
#include <boost/json/stream_parser.hpp>
#include <boost/json.hpp>
namespace json = boost::json;

// TODO this library returns a lot of references, copying might be expensive

bool readSeeds(json::array &jsonSeeds, std::vector<Individual> &seeds);
bool readGenome(json::object &jsonGenome, Genome &genome);

bool readConfig(const std::string &filename, std::vector<Individual> &seeds, Genome &genome) {
    std::ifstream ifs(filename);
    if(!ifs.is_open()) {
        std::cout << "Failed to open: " << filename << std::endl;
        return false;
    }

    std::string input(std::istreambuf_iterator<char>(ifs), {});

    auto val= json::parse(input);

    auto test = val.try_at("test");
    test.has_value();

    auto jsonObj = val.as_object();

    auto seed = jsonObj.at("seeds").as_array();

    if(!readGenome(jsonObj.at("genome").as_object(), genome)) {
        return false;
    }

    if(!readSeeds(jsonObj.at("seeds").as_array(), seeds)) {
        return false;
    }

    return true;
}

bool readSeeds(json::array &jsonSeeds, std::vector<Individual> &seeds) {
    for(int i = 0; i< jsonSeeds.size(); i++) {
        json::object jsonSeed = jsonSeeds[i].as_object();

        json::array jsonGenes = jsonSeed.at("genes").as_array();

        std::vector<Gene> genes;

        for(int j = 0; j < jsonSeeds.size(); j++) {
            json::object jsonGene = jsonGenes[j].as_object();

            std::string prefix(""), name, postfix("");
            std::vector<std::pair<std::string, int>> arguments;

            if(jsonGene.contains(prefix)) {
                prefix = jsonGene.at("prefix").as_string();
            }

            if(jsonGene.contains("prefix")) {
                postfix = jsonGene.at("postfix").as_string();
            }

            json::object jsonCommand = jsonGene.at("command").as_object();

            name = jsonCommand.at("name").as_string();

            if(jsonCommand.contains("arguments")) {
                json::array jsonArguments = jsonCommand.at("arguments").as_array();

                for(int k = 0; k < jsonArguments.size(); k++) {
                    json::object jsonArgument = jsonArguments[k].as_object();

                    std::string argName;
                    argName = jsonArgument.at("name").as_string();

                    if(jsonArgument.contains("min") && jsonArgument.contains("max")) {
                        int value = jsonArgument.at("value").as_int64();

                        arguments.push_back({argName, value});
                    } else {
                        // TODO this spot is for arguments with no variables
                    }
                }
            }

            // TODO in future genes might store their arguments
            for(const std::pair<std::string, int> &arg : arguments) {
                name += " -" + arg.first + " " + std::to_string(arg.second);
            }

            genes.push_back(Gene(prefix, name, postfix));
        }

        seeds.push_back(Individual(genes));
    }

    return true;
}

bool readGenome(json::object &jsonGenome, Genome &genome) {
    json::array prototypes = jsonGenome.at("genePrototypes").as_array();

    for(int i = 0; i < prototypes.size(); i++) {
        json::object prototype = prototypes[i].as_object();

        if(!prototype.contains("command")) {
            std::cout << "All genes must have a command." << std::endl;
            std::cout << "Please fix: " << prototype << std::endl;
            
            return false;
        }

        std::string prefix(""), name, postfix("");
        std::vector<Argument> arguments;

        if(prototype.contains(prefix)) {
            prefix = prototype.at("prefix").as_string();
        }

        if(prototype.contains("prefix")) {
            postfix = prototype.at("postfix").as_string();
        }

        json::object command = prototype.at("command").as_object();

        name = command.at("name").as_string();

        if(command.contains("arguments")) {
            json::array jsonArguments = command.at("arguments").as_array();

            for(int j = 0; j < jsonArguments.size(); j++) {
                json::object argument = jsonArguments[j].as_object();

                std::string argName;
                argName = argument.at("name").as_string();

                if(argument.contains("min") && argument.contains("max")) {
                    int min = argument.at("min").as_int64();
                    int max = argument.at("max").as_int64();

                    arguments.push_back(Argument(argName, min, max));
                } else {
                    // TODO this spot is for arguments with no variables
                }
            }
        }

        genome.addPrototype(name, prefix, postfix, arguments);
    }

    return true;
}