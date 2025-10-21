#pragma once

#include <string>

typedef struct Abc_Frame_t_ Abc_Frame_t;

// Wrapping genes in a class allows me to alter genes easier later
class Gene {
private:
    const std::string command; // Actual command to be doing work
    const std::string prefix;  // Command that must be run before command
    const std::string postfix; // Command that must be run after command

public:
    Gene(const std::string &command);
    Gene(const std::string &command, const std::string &prefix, const std::string &postfix);
    
    Gene & operator=(const Gene &other);

    int execute(Abc_Frame_t *pAbc) const;

    const std::string getCommand() const;

    friend std::ostream& operator<<(std::ostream& out, const Gene& gene);
};