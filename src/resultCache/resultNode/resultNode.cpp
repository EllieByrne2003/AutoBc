#include "resultNode.hpp"

#include <chrono>
#include <iostream>
#include <cassert>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "../../abc.hpp"

ResultNode::ResultNode() : m(*(new std::mutex)), time(0.0f), error(true) {
    assert(false); // This should never be used
}

ResultNode::ResultNode(std::mutex &m, const std::chrono::duration<double> time, Abc_Ntk_t *ntk, const bool error) : m(m), time(time), ntk(ntk, Abc_NtkDelete), error(error) {

}

ResultNode::ResultNode(std::mutex &m, const std::chrono::duration<double> time, std::shared_ptr<Abc_Ntk_t> ntk, const bool error, const std::map<std::string, std::shared_ptr<ResultNode>> &results) : m(m), time(time), ntk(ntk), error(error), results(results) {

}

ResultNode::~ResultNode() {
    std::cout << "destroyed node" << std::endl;
    // if(ntk != nullptr) {
    //     Abc_NtkDelete(ntk);
    // }
}

bool ResultNode::containsNode(const std::string &command) {
    std::unique_lock<std::mutex> lock(m);

    return results.find(command) != results.end();
}

std::shared_ptr<ResultNode> & ResultNode::getNode(const std::string &command) {
    std::unique_lock<std::mutex> lock(m);

    return results[command];
}

Abc_Ntk_t * ResultNode::cloneNtk() {
    std::unique_lock<std::mutex> lock(m);

    return Abc_NtkDup(ntk.get());
}

std::shared_ptr<ResultNode> & ResultNode::insertNode(const std::string &command, const std::chrono::duration<double> time, Abc_Ntk_t *ntk, const bool error) {
    std::unique_lock<std::mutex> lock(m);

    // We've been beaten here by another thread, free the ntk and return result
    if(results.find(command) != results.end()) {
        Abc_NtkDelete(ntk);
        return results[command];
    }

    results.emplace(command, std::make_shared<ResultNode>(m, time, ntk, error));
    return results[command]; // This saves a little unecessary locking
}


Result ResultNode::makeResult(Abc_Frame_t *frame) {
    // 1. Get copy of ntk
    Abc_Ntk_t *copy = cloneNtk();

    // 2. Strash it and set it
    Abc_Ntk_t *strashedCopy = Abc_NtkStrash(copy, 0, 1, 0);
    Abc_FrameSetCurrentNetwork(frame, strashedCopy);

    // 3. Check combinational equivalence
    if(Abc_ApiCec(frame, 1, (char **) &"cec")) {
        // Not equivalent, early return
        Abc_FrameDeleteAllNetworks(frame);
        Abc_NtkDelete(copy);

        return Result(); // Default constructor result returns poor result
    }

    // 4. Get levels and gates
    // const int levels = Abc_NtkLevel(strashedCopy);
    const int levels = Abc_AigLevel(strashedCopy);
    const int gates  = Abc_NtkNodeNum(strashedCopy);

    // 5. Free networks
    Abc_FrameDeleteAllNetworks(frame); // strashed copy
    Abc_NtkDelete(copy); // original copy

    // 6. Return result
    return Result(time, false, true, levels, gates);
}

const Result ResultNode::getResult(Abc_Frame_t *frame, std::string_view &str) {
    // 1. Check if this result is errored or non equivalent, return result
    if(error) {
        return Result(); // Nothing else after matters, an error is an error
    }

    // Remove leading whitespace and trailing whitespace/';'
    // str.remove_prefix(str.find_first_not_of(" \t\n\v\f\r;"));
    // str.remove_suffix(str.length - str.find_last_not_of(" \t\n\v\f\r;"));

    // 2. If str is empty, return result
    if(str == "" || str.empty()) {
        return makeResult(frame); // No more commands to run, this is what was asked for
    }

    std::cout << "Got: \"" << str << "\"" << std::endl;

    // 3. Look for command in cache, remove commands until found or empty
    std::string command(str);
    std::string remaining = "";
    while(command != "") {
        // 4a. Look for command, return result if exists
        std::cout << "Looking for: \"" << command << "\"" << std::endl;
        std::cout << "Remaining:   \"" << remaining << "\"" << std::endl; 
        if(containsNode(command)) {
            return getNode(command)->getResult(frame, str);
        }

        // 5a. lop off last command, add it to the start of remaining
        const int pos = command.find_last_of(";");
        if(pos == command.npos) {
            break; // No command left to lop off
        }
        
        if(remaining == "") {
            remaining = command.substr(pos + 2);
        } else {
            remaining = command.substr(pos + 2) + "; " + remaining;            
        }
        command   = command.substr(0, pos);
    }

    // 4b. Have to get result ourselves, get first command from str and go
    command = str.substr(0, str.find(';'));
    if(str.find(';') == str.npos) {
        str = ""; // No other command, set to nothing
    } else {
        str.remove_prefix(str.find(';') + 2);
    }
    std::cout << "Running:   \"" << command << "\"" << std::endl;
    std::cout << "Afterwards \"" << str << "\"" << std::endl << std::endl;

    // // 3. Split str into next command and rest
    // std::string command(str.substr(0, str.find(';')));
    // str.remove_prefix(str.find(';') + 1);

    // if(command == "" || command.empty()) {
    //     return result; // No more commands to run, this is what was asked for
    // }

    // std::cout << "Looking for: \"" << command << "\"" << std::endl;

    // // 4. If next command in results map, call this function on it
    // if(containsNode(command)) {
    //     return getNode(command).getResult(frame, str);
    // }

    // 5. Else, clone this ntk, run command
    Abc_FrameSetCurrentNetwork(frame, cloneNtk());
    const auto start = std::chrono::system_clock::now();

    // Run command
    if(Cmd_CommandExecute(frame, command.c_str())) {
        // Early return if errors occur
        Abc_FrameDeleteAllNetworks(frame);

        const auto end = std::chrono::system_clock::now();
        const std::chrono::duration<double> timeElapsed = time + (end - start);
        insertNode(command, timeElapsed, nullptr, true);
        return Result();
    }


    // // const auto start = std::chrono::system_clock::now();
    // Abc_Ntk_t *temp = Abc_NtkDup(Abc_FrameReadNtk(frame));
    // if(Abc_ApiCec(frame, 1, (char **) &"cec")) {
    //     // Not equivalent, early return
    //     Abc_FrameDeleteAllNetworks(frame);
    //     Abc_NtkDelete(temp);

    //     const auto end = std::chrono::system_clock::now();
    //     return insertNode(command, (end - start), nullptr, Result())->result;
    // }

    // Abc_FrameSetCurrentNetwork(frame, temp);

    // // Strash copy of network if needed
    // Abc_Ntk_t *ntkCopy = Abc_NtkDup(Abc_FrameReadNtk(frame));

    // if(!Abc_NtkIsStrash(ntkCopy)) {
    //     Abc_Ntk_t *temp = Abc_NtkStrash(ntkCopy, 0, 1, 0);
    //     Abc_NtkDelete(ntkCopy);
    //     ntkCopy = temp;
    // }

    // // Get results
    // // const bool isEquivalent = Abc_ApiCec2(ntkCopy);
    // const bool isEquivalent = true;
    // const int numLevels     = Abc_NtkLevel(ntkCopy);
    // const int numGates      = Abc_NtkNodeNum(ntkCopy);

    // Abc_NtkDelete(ntkCopy);

    Abc_Ntk_t *ntk = Abc_NtkDup(Abc_FrameReadNtk(frame)); 

    // // Copy network
    // Abc_Ntk_t *ntk = nullptr;
    // if(isEquivalent) {
    //     ntk = Abc_NtkDup(Abc_FrameReadNtk(frame));
    // }
    // Delete ntk from frame
    Abc_FrameDeleteAllNetworks(frame);

    const auto end = std::chrono::system_clock::now();
    const std::chrono::duration<double> timeElapsed = time + (end - start);

    // // 6. Insert result and call getResults on it
    // return insertNode(command, (end - start), ntk, Result(timeElapsed, false, isEquivalent, numLevels, numGates))->getResult(frame, str);
    return insertNode(command, (end - start), ntk, false)->getResult(frame, str);
}


const Result ResultNode::getResult(Abc_Frame_t *frame, std::string_view &str, const std::chrono::duration<double> minInsertionTime) {
    // 1. Check if this result is errored or non equivalent, return result
    if(error) {
        return Result(); // Nothing else after matters, an error is an error
    }

    // Remove leading whitespace and trailing whitespace/';'
    // str.remove_prefix(str.find_first_not_of(" \t\n\v\f\r;"));
    // str.remove_suffix(str.length - str.find_last_not_of(" \t\n\v\f\r;"));

    // 2. If str is empty, return result
    if(str == "" || str.empty()) {
        return makeResult(frame); // No more commands to run, this is what was asked for
    }

    std::cout << "Got: \"" << str << "\"" << std::endl;

    // 3. Look for command in cache, remove commands until found or empty
    std::string command(str);
    std::string remaining = "";
    while(command != "") {
        // 4a. Look for command, return result if exists
        std::cout << "Looking for: \"" << command << "\"" << std::endl;
        std::cout << "Remaining:   \"" << remaining << "\"" << std::endl; 
        if(containsNode(command)) {
            return getNode(command)->getResult(frame, str, minInsertionTime);
        }

        // 5a. lop off last command, add it to the start of remaining
        const int pos = command.find_last_of(";");
        if(pos == command.npos) {
            break; // No command left to lop off
        }
        
        if(remaining == "") {
            remaining = command.substr(pos + 2);
        } else {
            remaining = command.substr(pos + 2) + "; " + remaining;            
        }
        command   = command.substr(0, pos);
    }

    // 4b. Have to get the result ourselves. Run n commands until over minInsertionTime
    std::chrono::duration<double> totalTime(0.0f);
    std::string totalCommands("");
    Abc_FrameSetCurrentNetwork(frame, cloneNtk());

    do {
        // 5b. Get next command
        command = str.substr(0, str.find(';'));
        if(str.find(';') == str.npos) {
            str = ""; // No other command, set to nothing
        } else {
            str.remove_prefix(str.find(';') + 2);
        }

        std::cout << "Running:   \"" << command << "\"" << std::endl;
        std::cout << "Afterwards \"" << str << "\"" << std::endl << std::endl;

        // 5c. Run command, check time, errors and equivalence
        const auto start = std::chrono::system_clock::now();
        if(Cmd_CommandExecute(frame, command.c_str())) {
            // Early return if errors occur
            Abc_FrameDeleteAllNetworks(frame);

            const auto end = std::chrono::system_clock::now();
            totalTime += (end - start);
            if(remaining == "") {
                totalCommands = command;
            } else {
                totalCommands = command + "; " + totalCommands;            
            }

            if(totalTime >= minInsertionTime) {
                insertNode(totalCommands, totalTime, nullptr, true);
            }

            return Result();
        }

        const auto end = std::chrono::system_clock::now();
        totalTime += (end - start);
        if(remaining == "") {
            totalCommands = command;
        } else {
            totalCommands = command + "; " + totalCommands;            
        }
    } while(totalTime < minInsertionTime && str != "");


    // const auto start = std::chrono::system_clock::now();
    // Abc_Ntk_t *temp = Abc_NtkDup(Abc_FrameReadNtk(frame));
    // if(Abc_ApiCec(frame, 1, (char **) &"cec")) {
    //     // Not equivalent, early return
    //     Abc_FrameDeleteAllNetworks(frame);
    //     Abc_NtkDelete(temp);

    //     if(remaining == "") {
    //         totalCommands = command;
    //     } else {
    //         totalCommands = command + "; " + totalCommands;            
    //     }

    //     if(totalTime >= minInsertionTime) {
    //         insertNode(totalCommands, totalTime, nullptr, Result());
    //     }

    //     return Result();
    // }

    // Abc_FrameSetCurrentNetwork(frame, temp);

    // 5c. Done running commands, get results
    // Abc_Ntk_t *resultNtk = Abc_FrameReadNtk(frame);
    Abc_Ntk_t *ntkCopy   = Abc_NtkDup(Abc_FrameReadNtk(frame)); // Store for later, might want to save it

    // // Strash result
    // if(!Abc_NtkIsStrash(resultNtk)) {
    //     resultNtk = Abc_NtkStrash(resultNtk, 0, 1, 0);
    // }

    // // Get results
    // const int numLevels     = Abc_NtkLevel(resultNtk);
    // const int numGates      = Abc_NtkNodeNum(resultNtk);

    // Abc_NtkDelete(result); 
    Abc_FrameDeleteAllNetworks(frame);

    // const auto end = std::chrono::system_clock::now();
    // totalTime += (end - start);

    return insertNode(command, totalTime, ntkCopy, false)->getResult(frame, str, minInsertionTime);  
}

int ResultNode::prune(const std::chrono::duration<double> &minTime) {
    int pruned = 0;

    // For each in results: call prune
    for(auto nv : results) {
        pruned += nv.second->prune(minTime);
    }

    // For each in results: if time < minTime, collapse/remove it
    std::vector<std::pair<std::string, std::shared_ptr<ResultNode>>> newNodes;
    for(auto first = results.begin(), end = results.end(); first != end;) {
        if(first->second->time < minTime) {
            if(first->second->results.size() > 0) {
                first->second->collapse(first->first, newNodes);
            }

            first = results.erase(first);
            pruned++;
        } else {
            first++;
        }
    }

    // Insert new nodes
    for(auto nv : newNodes) {
        results.insert(nv);
    }

    return pruned;
}

std::chrono::duration<double> ResultNode::get_min_time() const {
    std::chrono::duration<double> minTime = time;

    for(auto nv : results) {
        std::chrono::duration<double> childMinTime = nv.second->get_min_time();
        if(childMinTime < minTime) {
            minTime = childMinTime;
        }
    }

    return minTime;
}

std::chrono::duration<double> ResultNode::get_max_time() const {
    std::chrono::duration<double> maxTime = time;

    for(auto nv : results) {
        std::chrono::duration<double> childMaxTime = nv.second->get_max_time();
        if(childMaxTime > maxTime) {
            maxTime = childMaxTime;
        }
    }

    return maxTime;
}

int ResultNode::get_node_count() const {
    int count = 1; // This node

    for(auto nv : results) {
        count += nv.second->get_node_count();
    }

    return count;
}

int ResultNode::get_node_count_under_time(const std::chrono::duration<double> &time) const {
    int count;
    if(this->time < time) {
        count++;
    }

    for(auto nv : results) {
        count += nv.second->get_node_count_under_time(time);
    }

    return count;
}

void ResultNode::collapse(const std::string &name, std::vector<std::pair<std::string, std::shared_ptr<ResultNode>>> &newNodes) {
    for(auto nv : results) {
        const std::shared_ptr<ResultNode> &node = nv.second;
        // const Result &result = node->result;
        const std::chrono::duration<double> totalTime = time + node->time;

        const std::string newName   = name + "; " + nv.first;
        std::shared_ptr<ResultNode> newNode = std::make_shared<ResultNode>(m, totalTime, node->ntk, false, node->results);

        newNodes.push_back({newName, newNode});
    }
}