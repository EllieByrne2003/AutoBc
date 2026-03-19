#include "resultNode.hpp"

#include <chrono>
#include <iostream>
#include <cassert>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "../../abc.hpp"

ResultNode::ResultNode() : m(*(new std::mutex)), time(0.0f), error(true) {
    assert(false); // This should never be used
}

ResultNode::ResultNode(std::mutex &m, const std::chrono::duration<double> time, Abc_Ntk_t *ntk, const bool error) : m(m), time(time), ntk(ntk, Abc_NtkDelete), error(error) {
    
}

ResultNode::ResultNode(std::mutex &m, const std::chrono::duration<double> time, std::shared_ptr<const Abc_Ntk_t> ntk, const bool error, const std::map<std::string, std::shared_ptr<ResultNode>> &results) : m(m), time(time), ntk(ntk), error(error), results(results) {
    assert(error == false);
}

ResultNode::~ResultNode() {
}

bool ResultNode::containsNode(const std::string &command) {
    std::unique_lock<std::mutex> lock(m);

    return results.find(command) != results.end();
}

std::shared_ptr<ResultNode> ResultNode::getNode(const std::string &command) {
    std::unique_lock<std::mutex> lock(m);

    return results[command];
}

Abc_Ntk_t * ResultNode::cloneNtk() {
    std::unique_lock<std::mutex> lock(m);

    return Abc_NtkDup(ntk.get());
}

std::shared_ptr<ResultNode> ResultNode::insertNode(const std::string &command, const std::chrono::duration<double> time, Abc_Ntk_t *ntk, const bool error) {
    std::unique_lock<std::mutex> lock(m);

    // We've been beaten here by another thread, free the ntk and return result
    if(results.find(command) != results.end()) {
        Abc_NtkDelete(ntk);
        return results[command];
    }

    results.emplace(command, std::make_shared<ResultNode>(m, time, ntk, error));
    return results[command]; // This saves a little unecessary locking
}

Result ResultNode::makeResult(Abc_Frame_t *frame, const std::string &finalFormat) {
    // 1. Get copy of ntk
    Abc_Ntk_t *copy = cloneNtk();

    std::cout << finalFormat << std::endl;

    // 2. Get the network type

    // 3. If type differs from final format, convert it
    if(finalFormat == "aig") {
        Abc_Ntk_t *strashedCopy = Abc_NtkStrash(copy, 0, 1, 0);
        Abc_FrameSetCurrentNetwork(frame, strashedCopy);
        
        if(Abc_ApiCec(frame, 1, (char **) &"cec")) {
            // Not equivalent, early return
            Abc_FrameDeleteAllNetworks(frame);
            Abc_NtkDelete(copy);

            return Result(); // Default constructor result returns poor result
        }
        
        const int levels = Abc_AigLevel(strashedCopy);
        const int gates  = Abc_NtkNodeNum(strashedCopy);

        // 5. Free networks
        Abc_NtkDelete(copy); // original copy
        Abc_FrameDeleteAllNetworks(frame); // strashed copy
    
        return Result(false, true, levels, gates);
    } else if(finalFormat.find("logic") == 0) { // Must be from start
        const int numPos = finalFormat.find("-") + 1;
        std::string faninNum = finalFormat.substr(numPos);

        Abc_FrameSetCurrentNetwork(frame, copy);
        if(Cmd_CommandExecute(frame, ("if -K " + faninNum).c_str())) {
            Abc_FrameDeleteAllNetworks(frame);

            return Result(); // Default constructor result returns poor result
        }

        if(Abc_ApiCec(frame, 1, (char **) &"cec")) {
            // Not equivalent, early return
            Abc_FrameDeleteAllNetworks(frame);

            return Result(); // Default constructor result returns poor result
        }

        copy = Abc_FrameReadNtk(frame);
        const int levels = Abc_NtkLevel(copy);
        const int gates = Abc_NtkNodeNum(copy);


        Abc_FrameDeleteAllNetworks(frame);
        return Result(false, true, levels, gates);
    }

    return Result();

    // 4. Check equivalence

    // 5. Get levels and gates (for format)

    // 6. Free networks

    // 7. Construct and return result

    // // 2. Strash it and set it
    // Abc_Ntk_t *strashedCopy = Abc_NtkStrash(copy, 0, 1, 0);
    // Abc_FrameSetCurrentNetwork(frame, strashedCopy);

    // // 3. Check combinational equivalence
    // if(Abc_ApiCec(frame, 1, (char **) &"cec")) {
    //     // Not equivalent, early return
    //     Abc_FrameDeleteAllNetworks(frame);
    //     Abc_NtkDelete(copy);

    //     return Result(); // Default constructor result returns poor result
    // }

    // // 4. Get levels and gates
    // // const int levels = Abc_NtkLevel(strashedCopy);
    // const int levels = Abc_AigLevel(strashedCopy);
    // const int gates  = Abc_NtkNodeNum(strashedCopy);

    // // 5. Free networks
    // Abc_FrameDeleteAllNetworks(frame); // strashed copy
    // // Abc_NtkDelete(copy); // original copy

    // // 6. Return result
    // return Result(false, true, levels, gates);
}

// const Result ResultNode::getResult(Abc_Frame_t *frame, std::string_view &str) {
//     // 1. Check if this result is errored or non equivalent, return result
//     if(error) {
//         return Result(); // Nothing else after matters, an error is an error
//     }

//     // 2. If str is empty, return result
//     if(str == "" || str.empty()) {
//         return makeResult(frame); // No more commands to run, this is what was asked for
//     }

//     // std::cout << "Got: \"" << str << "\"" << std::endl;

//     // 3. Look for command in cache, remove commands until found or empty
//     std::string command(str);
//     std::string remaining = "";
//     while(command != "") {
//         // 4a. Look for command, return result if exists
//         std::cout << "Looking for: \"" << command << "\"" << std::endl;
//         std::cout << "Remaining:   \"" << remaining << "\"" << std::endl; 
//         if(containsNode(command)) {
//             return getNode(command)->getResult(frame, str);
//         }

//         // 5a. lop off last command, add it to the start of remaining
//         const int pos = command.find_last_of(";");
//         if(pos == command.npos) {
//             break; // No command left to lop off
//         }
        
//         if(remaining == "") {
//             remaining = command.substr(pos + 2);
//         } else {
//             remaining = command.substr(pos + 2) + "; " + remaining;            
//         }
//         command   = command.substr(0, pos);
//     }

//     // 4b. Have to get result ourselves, get first command from str and go
//     command = str.substr(0, str.find(';'));
//     if(str.find(';') == str.npos) {
//         str = ""; // No other command, set to nothing
//     } else {
//         str.remove_prefix(str.find(';') + 2);
//     }
//     std::cout << "Running:   \"" << command << "\"" << std::endl;
//     std::cout << "Afterwards \"" << str << "\"" << std::endl << std::endl;

//     // 5. Else, clone this ntk, run command
//     Abc_FrameSetCurrentNetwork(frame, cloneNtk());
//     const auto start = std::chrono::system_clock::now();

//     // Run command
//     if(Cmd_CommandExecute(frame, command.c_str())) {
//         // Early return if errors occur
//         Abc_FrameDeleteAllNetworks(frame);

//         const auto end = std::chrono::system_clock::now();
//         const std::chrono::duration<double> timeElapsed = time + (end - start);
//         insertNode(command, timeElapsed, nullptr, true);
//         return Result();
//     }

//     Abc_Ntk_t *copy = Abc_NtkDup(Abc_FrameReadNtk(frame)); 

//     // Delete ntk from frame
//     Abc_FrameDeleteAllNetworks(frame);

//     const auto end = std::chrono::system_clock::now();
//     const std::chrono::duration<double> timeElapsed = time + (end - start);

//     // // 6. Insert result and call getResults on it
//     return insertNode(command, (end - start), copy, false)->getResult(frame, str);
// }


Result ResultNode::getResult(Abc_Frame_t *frame, std::string_view str, const std::string &finalFormat, const std::chrono::duration<double> minInsertionTime) {
    // 1. Check if this result is errored or non equivalent, return result
    if(error) {
        // std::cout << "Error made 1" << std::endl;
        return Result(); // Nothing else after matters, an error is an error
    }

    // 2. If str is empty, return result
    if(str == "" || str.empty()) {
        return makeResult(frame, finalFormat); // No more commands to run, this is what was asked for
    }

    // std::cout << "Got: \"" << str << "\"" << std::endl;

    // 3. Look for command in cache, remove commands until found or empty
    std::string command(str);
    std::string remaining = "";
    while(command != "") {
        // 4a. Look for command, return result if exists
        // std::cout << "Looking for: \"" << command << "\"" << std::endl;
        // std::cout << "Remaining:   \"" << remaining << "\"" << std::endl; 
        if(containsNode(command)) {
            str = remaining;
            return getNode(command)->getResult(frame, str, finalFormat, minInsertionTime);
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

        // std::cout << "Running:   \"" << command << "\"" << std::endl;
        // std::cout << "Afterwards \"" << str << "\"" << std::endl << std::endl;

        // 5c. Run command, check time, errors and equivalence
        const auto start = std::chrono::system_clock::now();
        if(Cmd_CommandExecute(frame, command.c_str())) {
            // Early return if errors occur
            Abc_FrameDeleteAllNetworks(frame);

            const auto end = std::chrono::system_clock::now();
            totalTime += (end - start);
            if(totalCommands == "") {
                totalCommands = command;
            } else {
                totalCommands = totalCommands + "; " + command;            
            }

            if(totalTime >= minInsertionTime) {
                insertNode(totalCommands, totalTime, nullptr, true);
            }

            return Result();
        }

        const auto end = std::chrono::system_clock::now();
        totalTime += (end - start);
        if(totalCommands == "") {
            totalCommands = command;
        } else {
            totalCommands = totalCommands + "; " + command;            
        }
    } while(totalTime < minInsertionTime && str != "");

    // 5c. Done running commands, get results
    Abc_Ntk_t *ntkCopy = Abc_NtkDup(Abc_FrameReadNtk(frame)); // Store for later, might want to save it
    Abc_FrameDeleteAllNetworks(frame);

    return insertNode(totalCommands, totalTime, ntkCopy, false)->getResult(frame, str, finalFormat, minInsertionTime);  
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