#include "gene.hpp"

#include <boost/json/array.hpp>

#include "../abc.hpp"
#include "../arguments/arguments.hpp"

Gene::Gene(const std::string &command, const std::string &inputType, const std::string outputType, const std::vector<std::shared_ptr<Argument>> &arguements) : 
    command(command), inputType(inputType), outputType(outputType), arguements(arguements) {
}


// Gene::Gene(const std::string &command, const std::string &prefix, const std::string &postfix) :
//     command(command), prefix(prefix), postfix(postfix) {
// }

// Gene::Gene(const std::string &command, const std::string &prefix, const std::string &postfix, const std::vector<std::shared_ptr<Argument>> &arguements) :
//     command(command), prefix(prefix), postfix(postfix), arguements(arguements) {
// }

Gene & Gene::operator=(const Gene &other) {
    Gene *gene = new Gene(other.command, other.inputType, other.outputType, other.arguements);
    return *gene;
}


std::string Gene::execute(Abc_Frame_t *pAbc, const std::string &ntkType) const {
    // Add args to command
    std::string fullCommand = command;
    for(const std::shared_ptr<Argument> &argument : arguements) {
        fullCommand += argument->to_string();
    }

    // Convert network if required
    if(ntkType != inputType) {
        if(inputType == "aig") {
            if(Cmd_CommandExecute(pAbc, "strash")) {
                return "error";
            }
        } else if(inputType == "logic") {
            if(Cmd_CommandExecute(pAbc, "multi")) {
                return "error";
            }
        }
    }

    // Run command
    if(Cmd_CommandExecute(pAbc, fullCommand.c_str())) {
        return "error";
    }

    // Return the current network type
    return outputType;
}

// int Gene::execute(Abc_Frame_t *pAbc) const {
//     std::string fullCommand = command;
//     for(const std::shared_ptr<Argument> &argument : arguements) {
//         fullCommand += argument->to_string();
//     }

//     int retValue = 0;

//     // if(prefix != "")
//     //     if((retValue = Cmd_CommandExecute(pAbc, prefix.c_str())))
//     //         return retValue;

//     if((retValue = Cmd_CommandExecute(pAbc, fullCommand.c_str())))
//         return retValue;


//     // if(postfix != "")
//     //     if((retValue = Cmd_CommandExecute(pAbc, postfix.c_str())))
//     //         return retValue;

//     return retValue;
// }


const std::string Gene::getCommand(const std::string &ntkType) const {
    std::string retValue;

    if(ntkType != inputType) {
        if(inputType == "aig") {
            retValue += "strash ; ";
        } else if(inputType == "logic") {
            retValue += "multi ; ";
        }
    }

    retValue += command;

    for(auto arg : arguements) {
        retValue += arg->to_string();
    }

    return retValue;
}

// // TODO update this or add other commands
// const std::string Gene::getCommand() const {
//     std::string retValue;

//     // if(prefix != "")
//     //     retValue += prefix + "; ";

//     retValue += command;

//     for(auto arg : arguements) {
//         retValue += arg->to_string();
//     }

//     // if(postfix != "")
//     //     retValue += "; " + postfix;

//     return retValue;
// }

// // TODO remove this
// std::ostream& operator<<(std::ostream& out, const Gene &gene) {
//     // if(gene.prefix != "") 
//     //     out << gene.prefix << "; ";

//     out << gene.command;

//     for(auto arg : gene.arguements) {
//         out << *arg;
//     }

//     // if(gene.postfix != "") 
//     //     out << "; " << gene.postfix;

//     return out;
// }


json::object Gene::toJson() const {
    json::object gene;

    gene["command"] = json::object();
    gene["command"].as_object()["name"] = command;
    
    if(!arguements.empty()) {
        gene["command"].as_object()["arguments"] = json::array();
        
        for(std::shared_ptr<Argument> arg : arguements) {
            gene["command"].as_object()["arguments"].as_array().push_back(arg->to_json());
        }
    }

    gene["input"]  = inputType;
    gene["output"] = outputType;

    // if(prefix != "")
    //     gene["prefix"] = prefix;

    // if(postfix != "")
    //     gene["postfix"] = postfix;

    return gene;
}