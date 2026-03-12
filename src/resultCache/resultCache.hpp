#pragma once

#include <map>
#include <mutex>
#include <string>
#include <memory>

#include "result/result.hpp"
#include "resultNode/resultNode.hpp"

typedef struct Abc_Ntk_t_ Abc_Ntk_t;

class ResultCache{
private:
    std::mutex m;

    std::map<std::string, Result> resultMap;
    std::unique_ptr<ResultNode> resultTree = nullptr;
    std::chrono::duration<double> minInsertionTime = std::chrono::duration<double>(0.0f);

    bool mapContains(const std::string &key);
    const Result & getFromMap(const std::string &key);
    void insertIntoMap(const std::string &key, const Result &result);

protected:

public:
    ResultCache(Abc_Ntk_t *base);
    ~ResultCache();

    // const Result getResult(Abc_Frame_t *frame, std::string_view &str);
    const Result getResult(Abc_Frame_t *frame, std::string_view &str, const std::string &finalFormat = "aig");

    bool prune();
    // void remove_unused();
};