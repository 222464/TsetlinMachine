/*
MIT License

Copyright (c) 2018 Eric Laukien

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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