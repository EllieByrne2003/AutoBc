#pragma once

#include <map>
#include <mutex>
#include <string>
#include <memory>

#include "resultNode/resultNode.hpp"

typedef struct Abc_Ntk_t_ Abc_Ntk_t;

class ResultCache{
private:
    std::mutex lock;

    std::unique_ptr<ResultNode> begin = nullptr;

protected:

public:
    ResultCache(Abc_Ntk_t *base);
    ~ResultCache();

    const Result & getResult(Abc_Frame_t *frame, std::string_view &str);
};