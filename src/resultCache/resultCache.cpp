#include "resultCache.hpp"

#include "../abc.hpp"
#include <chrono>
#include <memory>
#include <string_view>
#include <iostream>

ResultCache::ResultCache(Abc_Ntk_t *base) {
    // Strash copy of network if needed
    // Abc_Ntk_t *ntk = Abc_NtkDup(base);

    // if(!Abc_NtkIsStrash(ntk)) {
    //     Abc_Ntk_t *temp = Abc_NtkStrash(ntk, 0, 1, 0);
    //     Abc_NtkDelete(ntk);
    //     ntk = temp;
    // }

    // // Get results
    const std::chrono::duration<double> time(0.0);
    // const int numLevels = Abc_NtkLevel(ntk);
    // const int numGates  = Abc_NtkNodeNum(ntk);

    // Remove network
    // Abc_NtkDelete(ntk);

    resultTree = std::make_unique<ResultNode>(m, time, base, false);
}

ResultCache::~ResultCache() {

}

bool ResultCache::mapContains(const std::string &key) {
    std::unique_lock<std::mutex> lock(m);

    return resultMap.find(key) != resultMap.end();
}

const Result & ResultCache::getFromMap(const std::string &key) {
    std::unique_lock<std::mutex> lock(m);

    return resultMap[key];
}

void ResultCache::insertIntoMap(const std::string &key, const Result &result) {
    std::unique_lock<std::mutex> lock(m);

    if(resultMap.find(key) != resultMap.end()) {
        return;
    }

    resultMap[key] = result;
}

const Result ResultCache::getResult(Abc_Frame_t *frame, std::string_view &str, const std::string &finalFormat) {
    std::cout << "Running: " << str << std::endl;

    const auto start = std::chrono::system_clock::now();

    if(mapContains(std::string(str))) {
        return getFromMap(std::string(str));
    }
    
    const Result result = resultTree->getResult(frame, str, finalFormat, minInsertionTime);
    // const Result &result = resultTree->getResult(frame, str);
    insertIntoMap(std::string(str), result);

    const auto end = std::chrono::system_clock::now();
    const std::chrono::duration<double> timeElapsed = end - start;
    std::cout << "Finished: " << str << ", it took: " << timeElapsed.count() << "s" << std::endl;

    return result;
}

// bool ResultCache::prune() {
//     std::unique_lock<std::mutex> lock(m);

//     const int maxAttempts = 4;

//     int pruneAttempts = 0;
//     const int targetNum = 8192;
//     const int numNodes = begin->get_node_count();
//     const int pruneCount = numNodes - targetNum;
//     std::cout << "PRUNE | going to prune: " << pruneCount << " nodes." << std::endl;


//     const std::chrono::duration<double> minTime = begin->get_min_time();
//     const std::chrono::duration<double> maxTime = begin->get_max_time();
//     const std::chrono::duration<double> diffTime   = maxTime - minTime;

//     while(begin->get_node_count() > targetNum && pruneAttempts < maxAttempts) {
//         std::cout << "PRUNE | Attempting prune: " << pruneAttempts + 1 << " time." << std::endl;
//         const std::chrono::duration<double> newMinTime = minTime + (diffTime / (maxAttempts - pruneAttempts));

//         begin->prune(newMinTime);
//         pruneAttempts++;
//     }

//     const int remainingNodes = begin->get_node_count();
//     std::cout << "PRUNE | There are: " << remainingNodes << " nodes left." << std::endl;

//     // If we failed return false
//     if(begin->get_node_count() > targetNum) {
//         return false;
//     }

//     return true;
// }

bool ResultCache::prune() {
    std::unique_lock<std::mutex> lock(m);

    const int targetNum  = 4096;
    const int currentNum = resultTree->get_node_count();

    // std::cout << "PRUNE | map count is: " << resultMap.size() << std::endl;
    // std::cout << "PRUNE | node count at start is: " << currentNum << std::endl;

    if(currentNum <= targetNum) {
        return true;
    }

    int tryNum = 0;
    const int maxTries = 16;
    std::chrono::duration<double> low  = resultTree->get_min_time();
    std::chrono::duration<double> high = resultTree->get_max_time();
    std::chrono::duration<double> mid  = (low + high) / 2;
    const int needRemoved = currentNum - targetNum;
    int numRemoved = resultTree->get_node_count_under_time(mid);

    // std::cout << "PRUNE | min time is: " << low.count() << std::endl;
    // std::cout << "PRUNE | max time is: " << high.count() << std::endl;
    while(tryNum < maxTries) {
        if(numRemoved < needRemoved) {
            low = mid;
        } else if(numRemoved > needRemoved) {
            high = mid;
        } else {
            break;
        }

        mid = (low + high) / 2;
        numRemoved = resultTree->get_node_count_under_time(mid);
        tryNum++;
    }

    // Update minInsertionTime
    minInsertionTime = std::min(mid, std::chrono::duration<double>(1.0f));

    std::cout << "PRUNE | time aim is: " << mid.count() << std::endl;
    std::cout << "PRUNE | need to prune: " << needRemoved << " nodes." << std::endl;
    std::cout << "PRUNE | trying to prune: " << numRemoved << " nodes." << std::endl;

    const bool success = resultTree->prune(mid) >= needRemoved;

    const int remainingNodes = resultTree->get_node_count();
    std::cout << "PRUNE | successfully pruned: " << currentNum - remainingNodes << " nodes." << std::endl;
    std::cout << "PRUNE | There are: " << remainingNodes << " nodes left." << std::endl;

    return success;
}

// void ResultCache::remove_unused() {
//     std::unique_lock<std::mutex> lock(m);
//     std::cout << "PRUNE | Removing unused leaves." << std::endl;
//     const int currNodes = begin->get_node_count();

//     begin->remove_unused();

//     const int currNodes1 = begin->get_node_count();

//     std::cout << "PRUNE | Removed: " << currNodes - currNodes1 << " nodes." << std::endl;
// }