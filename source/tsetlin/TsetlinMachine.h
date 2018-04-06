#pragma once

#include <vector>
#include <random>
#include <unordered_set>
#include <assert.h>

class TsetlinMachine {
public:
    struct Clause {
        std::vector<int> _automataStates;
        std::unordered_set<int> _inclusions;

        int _state;
    };

    struct Output {
        std::vector<Clause> _clauses;

        int _sum;
    };

private:
    std::vector<int> _inputStates;
    
    std::vector<Output> _outputs;

    std::vector<int> _outputStates;

    void inclusionUpdate(int oi, int ci, int ai);
    void modifyI(int oi, int ci, float sInv, float sInvConj, std::mt19937 &rng);
    void modifyII(int oi, int ci);

public:
    void create(int numInputs, int numOutputs, int clausesPerOutput);

    const std::vector<int> &activate(const std::vector<int> &inputStates);

    void learn(const std::vector<int> &targetOutputStates, float s, int T, std::mt19937 &rng);
};