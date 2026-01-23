#include "resultCache.hpp"

#include "../abc.hpp"
#include <chrono>
#include <memory>
#include <string_view>

ResultCache::ResultCache(Abc_Ntk_t *base) {
    // Strash copy of network if needed
    Abc_Ntk_t *ntk = Abc_NtkDup(base);

    if(!Abc_NtkIsStrash(ntk)) {
        Abc_Ntk_t *temp = Abc_NtkStrash(ntk, 0, 1, 0);
        Abc_NtkDelete(ntk);
        ntk = temp;
    }

    // Get results
    const std::chrono::duration<double> time(0.0);
    const int numLevels = Abc_NtkLevel(ntk);
    const int numGates  = Abc_NtkNodeNum(ntk);

    begin = std::make_unique<ResultNode>(lock, base, Result(time, false, true, numLevels, numGates));
}

ResultCache::~ResultCache() {

}

const Result & ResultCache::getResult(Abc_Frame_t *frame, std::string_view &str) {
    return begin->getResult(frame, str);
}