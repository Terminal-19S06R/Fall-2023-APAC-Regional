#include <vector>
#include <unordered_set>
#include "navigation.hpp"

struct GameState {
    public:
    GameMap map;
    std::unordered_set<GameUnit*> units;
    std::unordered_set<GameUnit*> mobileUnits;
    ShortestPathFinder pathFinder;
    std::vector<std::vector<float>> heuristics;

    GameState(std::vector<GameUnit>& units_) {
        map = GameMap();
        // Initialise units
        for (int i = 0; i < units_.size(); i++) {
            GameUnit* unit = &units_[i];
            map.addUnit(unit);
            units.insert(unit);
            if (unit -> isMobileUnit()) {
                mobileUnits.insert(unit);
            }
        }
        pathFinder = ShortestPathFinder(&map);
        heuristics = std::vector<std::vector<float>>(2, std::vector<float> (5, 0));
    }

    std::vector<std::vector<float>> simulate_round() {
        /* Heuristics: 
        0: Damage to enemy health
        1: Damage to structural units
        2: Damage to mobile units
        3: Number of structural units destroyed
        4: Number of mobile units destroyed
        */

        // Spawn units: TODO
        int frame = 0;

        // Used to determine if we need to recalculate unit paths

        bool isStructureUnitDestroyedLastFrame = true;

        while (!mobileUnits.empty()) {
            if (isStructureUnitDestroyedLastFrame) {
                recalculate_paths();
            }

            frame++;
            shield_phase();
            move_phase(frame);
            attack_phase();
            isStructureUnitDestroyedLastFrame = cleanup_phase();
        }
        
        return heuristics;
    }

    private:
    void shield_phase() {
        for (GameUnit* u : units) {
            if (u -> isSupportUnit()) {
                shieldFriendlyUnits(u);
            }
        }
    }

    void shieldFriendlyUnits(GameUnit* shielder) {
        std::vector<GameUnit*> unitsInRange(0);
        find_all_friendly_units_in_range(unitsInRange, shielder);
        for (GameUnit* u : unitsInRange) {
            u -> shield(u);
        }
    }

    void find_all_friendly_units_in_range(std::vector<GameUnit*>& unitsInRange, GameUnit* u) {
        find_all_units_in_range(unitsInRange, u, u -> playerNumber);
    }

    void find_all_enemy_units_in_range(std::vector<GameUnit*>& unitsInRange, GameUnit* u) {
        find_all_units_in_range(unitsInRange, u, !u -> playerNumber);
    }

    void find_all_units_in_range(std::vector<GameUnit*>& unitsInRange, GameUnit* u, const int& playerNumber) {
        float range = u -> range;
        int x = u -> loc[0];
        int y = u -> loc[1];
        for (int i = x - range; i <= x + range; ++i) {
            for (int j = y - range; j <= y + range; ++j) {
                if (euclidean_distance(x, y, i, j) <= range) {
                    addToRange(unitsInRange, i, j, playerNumber);
                }
            }
        }
    }

    float euclidean_distance(int x1, int y1, int x2, int y2) {
        return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
    }

    GameUnit* find_enemy_target(GameUnit* u) {
        int targetPlayerNumber = !u -> playerNumber;

        std::vector<GameUnit*> targets(0);
        find_all_enemy_units_in_range(targets, u);
        filter_for_alive_enemies(targets);

        if (!u -> isMobileUnit()) {
            filter_for_mobile_units(targets);
        }

        if (targets.size() == 0) {
            return nullptr;
        } else if (targets.size() == 1) {
            return targets[0];
        }

        filter_for_mobile_units_if_exists(targets);
        if (targets.size() == 1) {
            return targets[0];
        }

        filter_for_nearest_targets(u, targets);
        if (targets.size() == 1) {
            return targets[0];
        }

        filter_for_lowest_health(targets);
        if (targets.size() == 1) {
            return targets[0];
        }

        filter_for_nearest_own_side(u -> playerNumber, targets);
        if (targets.size() == 1) {
            return targets[0];
        }

        filter_for_nearest_to_edge(targets);

        return targets[0];
    }

    void filter_for_mobile_units(std::vector<GameUnit*>& targets) {
        std::vector<GameUnit*> mobileUnits;
        for (GameUnit* u : targets) {
            if (u -> isMobileUnit()) {
                mobileUnits.push_back(u);
            }
        }
        targets = mobileUnits;
    }

    void filter_for_alive_enemies(std::vector<GameUnit*>& targets) {
        std::vector<GameUnit*> aliveUnits;
        for (GameUnit* u : targets) {
            if (!u -> isDestroyed()) {
                aliveUnits.push_back(u);
            }
        }
        targets = aliveUnits;
    }

    void filter_for_nearest_to_edge(std::vector<GameUnit*>& targets) {
        // TODO
        int smallest_dist_to_edge = 1000;
        std::vector<GameUnit*> nearestUnits;
        for (GameUnit* u : targets) {
            int dist_to_edge = map.calculate_smallest_distance_to_edge(u -> loc[0], u -> loc[1]);
            if (dist_to_edge < smallest_dist_to_edge) {
                smallest_dist_to_edge = dist_to_edge;
                nearestUnits.clear();
                nearestUnits.push_back(u);
            } else if (dist_to_edge == smallest_dist_to_edge) {
                nearestUnits.push_back(u);
            }
        }
        targets = nearestUnits;
    }

    void filter_for_nearest_own_side(const int& playerNumber, std::vector<GameUnit*>& targets) {
        // TODO
        if (playerNumber) {
            // Player 2
            int largesty = -1;
            std::vector<GameUnit*> nearestUnits;
            for (GameUnit* u : targets) {
                if (u -> loc[1] > largesty) {
                    largesty = u -> loc[1];
                    nearestUnits.clear();
                    nearestUnits.push_back(u);
                } else if (u -> loc[1] == largesty) {
                    nearestUnits.push_back(u);
                }
            }
        } else {
            // Player 1
            int smallesty = 1000;
            std::vector<GameUnit*> nearestUnits;
            for (GameUnit* u : targets) {
                if (u -> loc[1] < smallesty) {
                    smallesty = u -> loc[1];
                    nearestUnits.clear();
                    nearestUnits.push_back(u);
                } else if (u -> loc[1] == smallesty) {
                    nearestUnits.push_back(u);
                }
            }
        }
    }

    void filter_for_lowest_health(std::vector<GameUnit*>& targets) {
        int lowest_health = 1000;
        std::vector<GameUnit*> lowestHealthUnits;
        for (GameUnit* u : targets) {
            if (u -> health < lowest_health) {
                lowest_health = u -> health;
                lowestHealthUnits.clear();
                lowestHealthUnits.push_back(u);
            } else if (u -> health == lowest_health) {
                lowestHealthUnits.push_back(u);
            }
        }
        targets = lowestHealthUnits;
    }

    void filter_for_nearest_targets(GameUnit* u, std::vector<GameUnit*>& units) {
        // TODO
        int ux = u -> loc[0];
        int uy = u -> loc[1];
        int x, y;
        float shortest_distance = 1000;
        std::vector<GameUnit*> nearestUnits;
        for (GameUnit* u : units) {
            x = u -> loc[0];
            y = u -> loc[1];
            float distance = euclidean_distance(ux, uy, x, y);
            if (distance < shortest_distance) {
                shortest_distance = distance;
                nearestUnits.clear();
                nearestUnits.push_back(u);
            } else if (distance == shortest_distance) {
                nearestUnits.push_back(u);
            }
        }
        units = nearestUnits;
    }

    void filter_for_mobile_units_if_exists(std::vector<GameUnit*>& units) {
        bool mobileUnitExists = false;
        for (GameUnit* u : units) {
            if (u -> isMobileUnit()) {
                mobileUnitExists = true;
                break;
            }
        }

        if (mobileUnitExists) {
            std::vector<GameUnit*> mobileUnits;
            for (GameUnit* u : units) {
                if (u -> isMobileUnit()) {
                    mobileUnits.push_back(u);
                }
            }
            units = mobileUnits;
        }
    }

    void addToRange(std::vector<GameUnit*>& unitsInRange, const int& x, const int& y, const int& targetPlayerNumber) {
        for (GameUnit* u : map.getUnitsInCoord(x,y)) {
            if (u -> playerNumber == targetPlayerNumber) {
                unitsInRange.push_back(u);
            }
        }
    }

    void move_phase(const int& frame) {
        for (GameUnit* u : mobileUnits) {
            std::vector<int> loc = u -> loc;
            bool reached_end = u -> moveUnit(frame);
            map.moveUnit(u, loc);
            if (reached_end) {
                if (should_self_destruct(u)) {
                    self_destruct(u);
                } else {
                    score(u);
                }
            }
        }
    }

    void attack_phase() {
        for (GameUnit* u : units) {
            GameUnit* target = find_enemy_target(u);
            if (target == nullptr) {
                continue;
            }
            float damage = u -> attack_unit(target);
            if (target -> isMobileUnit()) {
                heuristics[u -> playerNumber][2] += damage;
            } else {
                heuristics[u -> playerNumber][1] += damage;
            }
        }
    }

    bool cleanup_phase() {
        bool structureUnitDestroyed = false;

        std::vector<GameUnit*> unitsToRemove(0);

        for (GameUnit* u : units) {
            if (u -> isDestroyed()) {
                unitsToRemove.push_back(u);
                if (!u -> isMobileUnit()) {
                    structureUnitDestroyed = true;
                }
            }
        }

        for (GameUnit* u : unitsToRemove) {
            remove_unit_and_update_heuristics(u);
        }

        return structureUnitDestroyed;
    }

    void remove_unit_and_update_heuristics(GameUnit* u) {
        mobileUnits.erase(u);
        units.erase(u);
        map.removeUnit(u);

        if (u -> isMobileUnit()) {
            ++heuristics[!u -> playerNumber][4];
        } else {
            ++heuristics[!u -> playerNumber][3];
        }
    }

    void recalculate_paths() {
        for (GameUnit* u : mobileUnits) {
            std::queue<std::vector<int>> path = pathFinder.calculatePath(u);
            u -> updatePath(path);
        }
    }

    bool should_self_destruct(GameUnit* u) {
        // Return true if unit is not at an edge
        return !map.unitIsAtTargetEdge(u);
    }

    void self_destruct(GameUnit* self_destructing_unit) {
        self_destructing_unit -> health = 0;

        if (!self_destructing_unit->hasMoved5Spaces()) {
            // Does not deal self-destruct damage if it has not moved 5 spaces
            return;
        }

        int x = self_destructing_unit -> loc[0];
        int y = self_destructing_unit -> loc[1];
        
        for (int i = x - 1; i <= x + 1; ++i) {
            for (int j = y - 1; j <= y + 1; ++j) {
                if (i == j) {
                    continue;
                }
                for (GameUnit* u : map.getUnitsInCoord(i, j)) {
                    if (u->isDestroyed()) {
                        continue;
                    }
                    if (u -> playerNumber != self_destructing_unit -> playerNumber) {
                        self_destructing_unit -> self_destruct(u);
                    }
                }
            }
        }
    }

    void score(GameUnit* u) {
        u -> health = 0;
        ++heuristics[u -> playerNumber][0];
        --heuristics[!u -> playerNumber][4];
    }
};