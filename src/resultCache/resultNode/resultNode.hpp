#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include <string>
#include <mutex>
#include <map>

#include "../result/result.hpp"

typedef struct Abc_Frame_t_ Abc_Frame_t;
typedef struct Abc_Ntk_t_   Abc_Ntk_t;

class ResultNode {
private:
    std::mutex &m;

    const std::chrono::duration<double> time;
    std::map<std::string, std::shared_ptr<ResultNode>> results;
    // Result result;
    const bool error;
    std::shared_ptr<const Abc_Ntk_t> ntk = nullptr;
    // Abc_Ntk_t *ntk = nullptr;

    bool containsNode(const std::string &command);
    std::shared_ptr<ResultNode> getNode(const std::string &command);
    Abc_Ntk_t * cloneNtk();
    std::shared_ptr<ResultNode> insertNode(const std::string &command, const std::chrono::duration<double> time, Abc_Ntk_t *ntk, const bool error);

    Result makeResult(Abc_Frame_t *frame, const std::string &finalFormat);

protected:

public:
    ResultNode();
    ResultNode(std::mutex &lock, const std::chrono::duration<double> time, Abc_Ntk_t *ntk, const bool error);
    ResultNode(std::mutex &lock, const std::chrono::duration<double> time, std::shared_ptr<const Abc_Ntk_t> ntk, const bool error, const std::map<std::string, std::shared_ptr<ResultNode>> &results);
    ~ResultNode();

    // const Result getResult(Abc_Frame_t *frame, std::string_view &str);
    Result getResult(Abc_Frame_t *frame, std::string_view str, const std::string &finalFormat, const std::chrono::duration<double> minInsertionTime);

    int prune(const std::chrono::duration<double> &minTime);
    void remove_unused();
    std::chrono::duration<double> get_time();
    void collect_times(int &total, std::chrono::duration<double> &time);
    std::chrono::duration<double> get_min_time() const;
    std::chrono::duration<double> get_max_time() const;
    int get_node_count() const;
    int get_node_count_under_time(const std::chrono::duration<double> &time) const;
    void collapse(const std::string &name, std::vector<std::pair<std::string, std::shared_ptr<ResultNode>>> &newNodes);
    void collapse_children();
};