#include "gameState.hpp"

// Takes in a list of units, and returns a list of heuristics
// GameUnit(int x, int y, float health_, int movementSpeed_, int attack_, int creationIndex_, int shielding_, int playerNumber_, float range_) {

int main(void) {
    std::vector<GameUnit> units;
    for (int i = 0; i < 1; ++i) {
        units.push_back(GameUnit(1, 12, 1000, 1, 20, 1000, 0, 0, 2.5));
    }

    for (int i = 0; i < 28; ++i) {
        if (i == 6) {
            units.push_back(GameUnit(i,14, 10, 0, 3, i, 0, 1, 2.5));
            continue;
        }
        units.push_back(GameUnit(i,14, 40, 0, 3, i, 0, 1, 2.5));
    }
    
    GameState state(units);
    std::vector<std::vector<int>> heuristics = state.simulate_round();
            /* Heuristics: 
        0: Damage to enemy health
        1: Damage to structural units
        2: Damage to mobile units
        3: Number of structural units destroyed
        4: Number of mobile units destroyed
        */

    for (int i = 0; i < 2; ++i) {
        std::cout << "Player " << i << std::endl;
        std::cout << "Damage to HP: " << heuristics[i][0] << std::endl;
        std::cout << "Dmg to Structural Units: " << heuristics[i][1] << std::endl   ;
        std::cout << "Dmg to Mobile Units: " << heuristics[i][2] << std::endl;
        std::cout << "Structural Units Destroyed: " << heuristics[i][3] << std::endl;
        std::cout << "Mobile Units Destroyed: " << heuristics[i][4] << std::endl;
    }
}
