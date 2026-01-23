#include "resultNode.hpp"

#include <iostream>
#include <cassert>
#include <mutex>
#include <string>

#include "../../abc.hpp"

ResultNode::ResultNode() : m(*(new std::mutex)), result() {
    assert(false); // This should never be used
}

ResultNode::ResultNode(std::mutex &m, Abc_Ntk_t *ntk, const Result &result) : m(m), ntk(ntk), result(result) {

}

ResultNode::~ResultNode() {

}


bool ResultNode::containsNode(const std::string &command) {
    std::unique_lock<std::mutex> lock(m);

    return results.find(command) != results.end();
}

ResultNode & ResultNode::getNode(const std::string &command) {
    std::unique_lock<std::mutex> lock(m);

    return results[command];
}

Abc_Ntk_t * ResultNode::cloneNtk() {
    std::unique_lock<std::mutex> lock(m);

    return Abc_NtkDup(ntk);
}

ResultNode & ResultNode::insertNode(const std::string &command, Abc_Ntk_t *ntk, const Result &result) {
    std::unique_lock<std::mutex> lock(m);

    // We've been beaten here by another thread, free the ntk and return result
    if(results.find(command) != results.end()) {
        Abc_NtkDelete(ntk);
        return results[command];
    }

    results.emplace(command, ResultNode(m, ntk, result));
    return results[command]; // This saves a little unecessary locking
}

const Result & ResultNode::getResult(Abc_Frame_t *frame, std::string_view &str) {
    // 1. Check if this result is errored, return result
    if(result.error) {
        return result; // Nothing else after matters, an error is an error
    }

    // Remove leading whitespace and trailing whitespace/';'
    str.remove_prefix(str.find_first_not_of(" \t\n\v\f\r;"));
    // str.remove_suffix(str.length - str.find_last_not_of(" \t\n\v\f\r;"));

    // 2. If str is empty, return result
    if(str == "" || str.empty()) {
        return result; // No more commands to run, this is what was asked for
    }

    std::cout << "Running: \"" << std::string(str) << "\"" << std::endl;

    // 3. Split str into next command and rest
    std::string command(str.substr(0, str.find(';')));
    str.remove_prefix(str.find(';') + 1);

    if(command == "" || command.empty()) {
        return result; // No more commands to run, this is what was asked for
    }

    std::cout << "Looking for: \"" << command << "\"" << std::endl;

    // 4. If next command in results map, call this function on it
    if(containsNode(command)) {
        return getNode(command).getResult(frame, str);
    }

    // 5. Else, clone this ntk, run command
    Abc_FrameSetCurrentNetwork(frame, cloneNtk());
    const auto start = std::chrono::system_clock::now();

    // Run command
    if(Cmd_CommandExecute(frame, command.c_str())) {
        // Early return if errors occur
        Abc_FrameDeleteAllNetworks(frame);

        insertNode(command, nullptr, Result());
        return results[command].result;
    }

    const auto end = std::chrono::system_clock::now();
    const std::chrono::duration<double> timeElapsed = result.time + (end - start);

    // Strash copy of network if needed
    Abc_Ntk_t *ntkCopy = Abc_NtkDup(Abc_FrameReadNtk(frame));

    if(!Abc_NtkIsStrash(ntkCopy)) {
        Abc_Ntk_t *temp = Abc_NtkStrash(ntkCopy, 0, 1, 0);
        Abc_NtkDelete(ntkCopy);
        ntkCopy = temp;
    }

    // Get results
    // const bool isEquivalent = Abc_ApiCec2(ntkCopy);
    const bool isEquivalent = true;
    const int numLevels     = Abc_NtkLevel(ntkCopy);
    const int numGates      = Abc_NtkNodeNum(ntkCopy);

    Abc_NtkDelete(ntkCopy);

    // Copy network
    Abc_Ntk_t *ntk = Abc_NtkDup(Abc_FrameReadNtk(frame));

    // Delete ntk from frame
    Abc_FrameDeleteAllNetworks(frame);

    // 6. Insert result and call getResults on it
    return insertNode(command, ntk, Result(timeElapsed, false, isEquivalent, numLevels, numGates)).getResult(frame, str);
}