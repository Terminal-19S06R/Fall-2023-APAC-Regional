#include "gameState.hpp"

float* parse_and_run(float* array, int numUnits, float* result) {
    std::vector<GameUnit> units;
    for (int i = 0; i < numUnits; ++i) {
        GameUnit u = GameUnit(
            array[i * 9 + 0], 
            array[i * 9 + 1],
            array[i * 9 + 2], 
            array[i * 9 + 3], 
            array[i*9+4], array[i*9+5], array[i*9+6], array[i*9+7], array[i*9+8]);
        units.push_back(u);
    }

    std::vector<std::vector<float>> heuristics = GameState(units).simulate_round();
    for (int i = 0; i < 2; ++i) {
        result[i * 5] = heuristics[i][0];
        result[i * 5 + 1] = heuristics[i][1];
        result[i * 5 + 2] = heuristics[i][2];
        result[i * 5 + 3] = heuristics[i][3];
        result[i * 5 + 4] = heuristics[i][4];
    }

    return result;
}

extern "C" // required when using C++ compiler
float* run_simulation(float* array, int numUnits, float* result) {
    return parse_and_run(array, numUnits, result);
}