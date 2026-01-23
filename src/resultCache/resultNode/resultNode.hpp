#pragma once

#include <string_view>
#include <string>
#include <mutex>
#include <map>

#include "../result/result.hpp"

typedef struct Abc_Frame_t_ Abc_Frame_t;
typedef struct Abc_Ntk_t_   Abc_Ntk_t;

class ResultNode {
private:
    std::mutex &m;

    std::map<std::string, ResultNode> results;
    Result result;
    Abc_Ntk_t *ntk = nullptr;

    bool containsNode(const std::string &command);
    ResultNode & getNode(const std::string &command);
    Abc_Ntk_t * cloneNtk();
    ResultNode & insertNode(const std::string &command, Abc_Ntk_t *ntk, const Result &result);

protected:

public:
    ResultNode();
    ResultNode(std::mutex &lock, Abc_Ntk_t *ntk, const Result &result);
    ~ResultNode();

    const Result & getResult(Abc_Frame_t *frame, std::string_view &str);
};