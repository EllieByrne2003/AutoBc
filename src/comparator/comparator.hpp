#pragma once

typedef struct Abc_Ntk_t_ Abc_Ntk_t;

class Result;

class Comparator {


    Result createResult(Abc_Ntk_t *ntk);
    int compare(const Result &a, const Result &b);
};

// The comparator stores info on what is needed for comparisons
// It stores how to compare it as well