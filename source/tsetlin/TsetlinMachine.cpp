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

#include "TsetlinMachine.h"

void TsetlinMachine::create(int numInputs, int numOutputs, int clausesPerOutput) {
    _inputStates.resize(numInputs, 0);

    _outputs.resize(numOutputs);

    for (int oi = 0; oi < numOutputs; oi++) {
        _outputs[oi]._clauses.resize(clausesPerOutput);

        for (int ci = 0; ci < clausesPerOutput; ci++)
            _outputs[oi]._clauses[ci]._automataStates.resize(numInputs * 2, 0);
    }

    _outputStates.resize(numOutputs, 0);
}

const std::vector<int> &TsetlinMachine::activate(const std::vector<int> &inputStates) {
    _inputStates = inputStates;

    for (int oi = 0; oi < _outputs.size(); oi++) {
        int sum = 0;

        for (int ci = 0; ci < _outputs[oi]._clauses.size(); ci++) {
            int state = 1;

            for (std::unordered_set<int>::const_iterator cit = _outputs[oi]._clauses[ci]._inclusions.begin(); cit != _outputs[oi]._clauses[ci]._inclusions.end(); cit++) {
                int ai = *cit;

                if (ai >= _inputStates.size())
                    state = state && !_inputStates[ai - _inputStates.size()];
                else
                    state = state && _inputStates[ai];
            }

            _outputs[oi]._clauses[ci]._state = state;

            sum += (ci % 2 == 0 ? state : -state);
        }

        _outputs[oi]._sum = sum;

        _outputStates[oi] = sum > 0;
    }

    return _outputStates;
}

void TsetlinMachine::inclusionUpdate(int oi, int ci, int ai) {
    int inclusion = _outputs[oi]._clauses[ci]._automataStates[ai] > 0;

    std::unordered_set<int>::iterator it = _outputs[oi]._clauses[ci]._inclusions.find(ai);

    if (inclusion) {
        if (it == _outputs[oi]._clauses[ci]._inclusions.end())
            _outputs[oi]._clauses[ci]._inclusions.insert(ai);
    }
    else {
        if (it != _outputs[oi]._clauses[ci]._inclusions.end())
            _outputs[oi]._clauses[ci]._inclusions.erase(it);
    }
}

void TsetlinMachine::modifyI(int oi, int ci, float sInv, float sInvConj, std::mt19937 &rng) {
    std::uniform_real_distribution<float> dist01(0.0f, 1.0f);

    int clauseState = _outputs[oi]._clauses[ci]._state;

    for (int ai = 0; ai < _outputs[oi]._clauses[ci]._automataStates.size(); ai++) {
        int input = (ai >= _inputStates.size() ? !_inputStates[ai - _inputStates.size()] : _inputStates[ai]);

        int inclusion = _outputs[oi]._clauses[ci]._automataStates[ai] > 0;

        if (clauseState) {
            if (input) {
                if (inclusion) {
                    if (dist01(rng) < sInvConj) {
                        _outputs[oi]._clauses[ci]._automataStates[ai] += 1;

                        inclusionUpdate(oi, ci, ai);
                    }
                }
                else {
                    if (dist01(rng) < sInvConj) {
                        _outputs[oi]._clauses[ci]._automataStates[ai] += 1;

                        inclusionUpdate(oi, ci, ai);
                    }
                }
            }
            else {
                if (inclusion) {
                    // NA
                }
                else {
                    if (dist01(rng) < sInv) {
                        _outputs[oi]._clauses[ci]._automataStates[ai] -= 1;

                        inclusionUpdate(oi, ci, ai);
                    }
                }
            }
        }
        else {
            if (input) {
                if (inclusion) {
                    if (dist01(rng) < sInv) {
                        _outputs[oi]._clauses[ci]._automataStates[ai] -= 1;

                        inclusionUpdate(oi, ci, ai);
                    }
                }
                else {
                    if (dist01(rng) < sInv) {
                        _outputs[oi]._clauses[ci]._automataStates[ai] -= 1;

                        inclusionUpdate(oi, ci, ai);
                    }
                }
            }
            else {
                if (inclusion) {
                    if (dist01(rng) < sInv) {
                        _outputs[oi]._clauses[ci]._automataStates[ai] -= 1;

                        inclusionUpdate(oi, ci, ai);
                    }
                }
                else {
                    if (dist01(rng) < sInv) {
                        _outputs[oi]._clauses[ci]._automataStates[ai] -= 1;

                        inclusionUpdate(oi, ci, ai);
                    }
                }
            }
        }
    }
}

void TsetlinMachine::modifyII(int oi, int ci) {
    int clauseState = _outputs[oi]._clauses[ci]._state;

    for (int ai = 0; ai < _outputs[oi]._clauses[ci]._automataStates.size(); ai++) {
        int input = (ai >= _inputStates.size() ? !_inputStates[ai - _inputStates.size()] : _inputStates[ai]);

        int inclusion = _outputs[oi]._clauses[ci]._automataStates[ai] > 0;

        if (clauseState) {
            if (!input) {
                if (!inclusion) {
                    _outputs[oi]._clauses[ci]._automataStates[ai] += 1;

                    inclusionUpdate(oi, ci, ai);
                }
            }
        }
    }
}

void TsetlinMachine::learn(const std::vector<int> &targetOutputStates, float s, int T, std::mt19937 &rng) {
    std::uniform_real_distribution<float> dist01(0.0f, 1.0f);

    float sInv = 1.0f / s;
    float sInvConj = 1.0f - sInv;

    for (int oi = 0; oi < _outputs.size(); oi++) {
        int clampedSum = std::min(T, std::max(-T, _outputs[oi]._sum));
        float rescale = 1.0f / static_cast<float>(2 * T);

        float probFeedBack0 = (T - clampedSum) * rescale;
        float probFeedBack1 = (T + clampedSum) * rescale;
    
        int sum = 0;

        for (int ci = 0; ci < _outputs[oi]._clauses.size(); ci++) {
            if (ci % 2 == 0) {
                if (targetOutputStates[oi]) {
                    if (dist01(rng) < probFeedBack0)
                        modifyI(oi, ci, sInv, sInvConj, rng);
                }
                else {
                    if (dist01(rng) < probFeedBack1)
                        modifyII(oi, ci);
                }
            }
            else {
                if (targetOutputStates[oi]) {
                    if (dist01(rng) < probFeedBack0)
                        modifyII(oi, ci);
                }
                else {
                    if (dist01(rng) < probFeedBack1)
                        modifyI(oi, ci, sInv, sInvConj, rng);
                }
            }
        }
    }
}