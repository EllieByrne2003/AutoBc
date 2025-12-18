#pragma once

#include <string>
#include <vector>
#include <memory>
#include <boost/json/object.hpp>
namespace json = boost::json;

class Argument;

typedef struct Abc_Frame_t_ Abc_Frame_t;

// Wrapping genes in a class allows me to alter genes easier later
class Gene {
private:
    const std::string command; // Actual command to be doing work
    // const std::string prefix;  // Command that must be run before command
    // const std::string postfix; // Command that must be run after command
    const std::string inputType;
    const std::string outputType;

    std::vector<std::shared_ptr<Argument>> arguements;

public:
    Gene(const std::string &command, const std::string &inputType, const std::string outputType, const std::vector<std::shared_ptr<Argument>> &arguements);
    // Gene(const std::string &command, const std::string &prefix, const std::string &postfix);
    // Gene(const std::string &command, const std::string &prefix, const std::string &postfix, const std::vector<std::shared_ptr<Argument>> &arguements);

    Gene & operator=(const Gene &other);

    std::string execute(Abc_Frame_t *pAbc, const std::string &ntkType) const;
    // int execute(Abc_Frame_t *pAbc) const;

    const std::string getCommand(const std::string &ntkType) const;
    // const std::string getCommand() const;

    // friend std::ostream& operator<<(std::ostream& out, const Gene& gene);

    json::object toJson() const;
};