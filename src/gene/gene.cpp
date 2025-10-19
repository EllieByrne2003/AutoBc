#include "gene.hpp"

#include "../abc.hpp"
#include <iostream>

Gene::Gene(const std::string &command) : Gene(command,  "", "") {
}


Gene::Gene(const std::string &command, const std::string &prefix, const std::string &postfix) :
    command(command), prefix(prefix), postfix(postfix) {
}

int Gene::execute(Abc_Frame_t *pAbc) const {
    int retValue = 0;

    if(prefix != "")
        if((retValue = Cmd_CommandExecute(pAbc, prefix.c_str())))
            return retValue;

    if((retValue = Cmd_CommandExecute(pAbc, command.c_str())))
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

    if(postfix != "")
        retValue += "; " + postfix;

    return retValue;
}


std::ostream& operator<<(std::ostream& out, const Gene &gene) {
    if(gene.prefix != "") 
        out << gene.prefix << "; ";

    out << gene.command;

    if(gene.postfix != "") 
        out << "; " << gene.postfix;

    return out;
}