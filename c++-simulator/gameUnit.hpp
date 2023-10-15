#include <vector>
#include <queue>
#include <unordered_set>
#include <math.h>
#include <iostream>

float SELF_DESTRUCT_DAMAGE = 1.5;
std::vector<std::vector<int>> TOP_RIGHT {{14, 27}, {15, 26}, {16, 25}, {17, 24}, {18, 23}, {19, 22}, {20, 21}, {21, 20}, {22, 19}, {23, 18}, {24, 17}, {25, 16}, {26, 15}, {27, 14}};
std::vector<std::vector<int>> TOP_LEFT {{0,14}, {1, 15}, {2, 16}, {3, 17}, {4, 18}, {5, 19}, {6, 20}, {7, 21}, {8, 22}, {9, 23}, {10, 24}, {11, 25}, {12, 26}, {13, 27}};
std::vector<std::vector<int>> BOTTOM_RIGHT {{14,0}, {15, 1}, {16, 2}, {17, 3}, {18, 4}, {19, 5}, {20, 6}, {21, 7}, {22, 8}, {23, 9}, {24, 10}, {25, 11}, {26, 12}, {27, 13}};
std::vector<std::vector<int>> BOTTOM_LEFT {{0,13}, {1, 12}, {2, 11}, {3, 10}, {4, 9}, {5, 8}, {6, 7}, {7, 6}, {8, 5}, {9, 4}, {10, 3}, {11, 2}, {12, 1}, {13, 0}};

struct GameUnit {
    public:
    std::vector<int> spawnloc;
    std::vector<int> loc;
    int spaces_moved = 0;
    float health;
    int movementSpeed;
    int attack;
    int shielding;
    int creationIndex;
    int playerNumber;
    bool isLastMoveHorizontal = true;
    float range;
    std::unordered_set<GameUnit*> unitsShielded;
    std::queue<std::vector<int>> path;
    std::vector<std::vector<int>> endpoints;

    // Use this to create Mobile Units
    GameUnit(int x, int y, float health_, int movementSpeed_, int attack_, int creationIndex_, int shielding_, int playerNumber_, float range_) {
        loc = std::vector<int> {x, y};
        health = health_;
        movementSpeed = movementSpeed_;
        attack = attack_;
        creationIndex = creationIndex_;
        shielding = shielding_;
        playerNumber = playerNumber_;
        range = range_;
        spawnloc = loc;
        if (movementSpeed > 0) {
            endpoints = spawnloc[0] > 13 ? (spawnloc[1] > 13 ? BOTTOM_LEFT : TOP_LEFT) : (spawnloc[1] > 13 ? BOTTOM_RIGHT : TOP_RIGHT);
        }
    }

    bool lastMoveWasHorizontal() {
        return isLastMoveHorizontal;
    }

    // Returns true if the unit can move but cannot move anywhere anymore
    bool moveUnit(const int& frame) {
        if (frame % movementSpeed != 0) {
            return false;
        }

        if (path.size() == 0) {
            return true;
        }

        isLastMoveHorizontal = loc[1] == path.front()[1];
        loc = path.front(); path.pop();
        ++spaces_moved;

        return false;
    }

    void updatePath(std::queue<std::vector<int>>& path_) {
        path = path_;
    }

    // returns damage dealt
    float attack_unit(GameUnit* target) {
        target -> health -= attack;
        return std::min((float) attack, target -> health + attack);
    }

    float self_destruct(GameUnit* target) {
        target -> health -= SELF_DESTRUCT_DAMAGE;
        return std::min(1.5f, target -> health + SELF_DESTRUCT_DAMAGE);
    }

    bool hasMoved5Spaces() {
        return spaces_moved >= 5;
    }

    bool isDestroyed() {
        return health <= 0;
    }

    bool reachedEnd() {
        return path.size() == 0;
    }

    bool isMobileUnit() {
        return movementSpeed > 0;
    }

    bool isSupportUnit() {
        return shielding > 0;
    }

    bool isAtSpawnLocation() {
        return loc == spawnloc;
    }

    void shield(GameUnit* shieldee) {
        if (unitHasBeenShielded(shieldee)) {
            return;
        } else {
            shieldee -> health += shielding;
        }
    }

    std::vector<std::vector<int>> getEndpoints() {
        return endpoints;
    }

    std::vector<int> getEndpointDirection() {
        if (spawnloc[0] > 13) {
            if (spawnloc[1] > 13) {
                // TOP RIGHT SPAWN
                return std::vector<int> {0,0};
            } else {
                // BOTTOM RIGHT SPAWN
                return std::vector<int> {0, 1};
            }
        } else {
            if (spawnloc[1] > 13) {
                // TOP LEFT SPAWN
                return std::vector<int> {1, 0};
            } else {
                // BOTTOM LEFT SPAWN
                return std::vector<int> {1, 1};
            }
        }
    }

    private:
    bool unitHasBeenShielded(GameUnit* unit) {
        return unitsShielded.find(unit) != unitsShielded.end();
    }

};