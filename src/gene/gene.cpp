#include "gene.hpp"

#include <iostream>

#include "../abc.hpp"
#include "../arguments/arguments.hpp"

Gene::Gene(const std::string &command) : Gene(command,  "", "") {
}


Gene::Gene(const std::string &command, const std::string &prefix, const std::string &postfix) :
    command(command), prefix(prefix), postfix(postfix) {
}

Gene::Gene(const std::string &command, const std::string &prefix, const std::string &postfix, const std::vector<std::shared_ptr<Argument>> &arguements) :
    command(command), prefix(prefix), postfix(postfix), arguements(arguements) {
}

Gene & Gene::operator=(const Gene &other) {
    Gene *gene = new Gene(other.command, other.prefix, other.postfix, other.arguements);
    return *gene;
}

int Gene::execute(Abc_Frame_t *pAbc) const {
    std::string fullCommand = command;
    for(const std::shared_ptr<Argument> &argument : arguements) {
        fullCommand += argument->to_string();
    }

    int retValue = 0;

    if(prefix != "")
        if((retValue = Cmd_CommandExecute(pAbc, prefix.c_str())))
            return retValue;

    if((retValue = Cmd_CommandExecute(pAbc, fullCommand.c_str())))
        return retValue;


    if(postfix != "")
        if((retValue = Cmd_CommandExecute(pAbc, postfix.c_str())))
            return retValue;

    return retValue;
}

// TODO update this or add other commands
const std::string Gene::getCommand() const {
    std::string retValue;

    if(prefix != "")
        retValue += prefix + "; ";

    retValue += command;

    for(auto arg : arguements) {
        retValue += arg->to_string();
    }

    if(postfix != "")
        retValue += "; " + postfix;

    return retValue;
}


std::ostream& operator<<(std::ostream& out, const Gene &gene) {
    if(gene.prefix != "") 
        out << gene.prefix << "; ";

    out << gene.command;

    for(auto arg : gene.arguements) {
        out << *arg;
    }

    if(gene.postfix != "") 
        out << "; " << gene.postfix;

    return out;
}