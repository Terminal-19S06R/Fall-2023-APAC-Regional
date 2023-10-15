#include <vector>
#include "gameUnit.hpp"

int MAP_LENGTH = 28;

struct GameMap {
    public:
    std::vector<std::vector<std::vector<GameUnit*>>> layout;
    std::vector<std::vector<bool>> blocked;

    GameMap() {
        layout = std::vector<std::vector<std::vector<GameUnit*>>>(
            MAP_LENGTH, std::vector<std::vector<GameUnit*>>(MAP_LENGTH, std::vector<GameUnit*>()));
            
        blocked = std::vector<std::vector<bool>>(MAP_LENGTH, std::vector<bool>(MAP_LENGTH, false));
    }

    void addUnit(GameUnit* unit) {
        layout[unit->loc[0]][unit->loc[1]].push_back(unit);
        if (!unit -> isMobileUnit()) {
            blocked[unit->loc[0]][unit->loc[1]] = true;
        }
    }

    void removeUnit(GameUnit* unit) {
        int x = unit -> loc[0];
        int y = unit -> loc[1];
        std::vector<GameUnit*>::iterator position = std::find(layout[x][y].begin(), layout[x][y].end(), unit);
        layout[x][y].erase(position);

        if (!unit -> isMobileUnit()) {
            blocked[x][y] = false;
        }
    }

    void moveUnit(GameUnit* unit, std::vector<int>& prevloc) {
        int x = unit -> loc[0];
        int y = unit -> loc[1];
        int prevx = prevloc[0];
        int prevy = prevloc[1];
        if (prevx == x && prevy == y) {
            return;
        }

        std::vector<GameUnit*>::iterator position = std::find(layout[prevx][prevy].begin(), layout[prevx][prevy].end(), unit);
        layout[prevx][prevy].erase(position);
        layout[x][y].push_back(unit);
    }

    bool contains_stationary_unit(int x, int y) {
        std::vector<GameUnit*> unitsAtLocation = getUnitsInCoord(x, y);
        for (GameUnit* u : unitsAtLocation) {
            if (!u -> isMobileUnit()) {
                return true;
            }
        }
        return false;
    }

    bool unitIsAtTargetEdge(GameUnit* unit) {
        std::vector<int> startingLoc = unit->spawnloc;
        if (startingLoc[0] > 14) {
            if (startingLoc[1] > 13) {
                // TOP RIGHT
                return calculate_distance_to_bottom_left(unit->loc[0], unit->loc[1]) <= 1;
            } else {
                // BOTTOM RIGHT
                return calculate_distance_to_top_left(unit->loc[0], unit->loc[1]) <= 1;
            }
        } else {
            if (startingLoc[1] > 13) {
                // TOP LEFT
                return calculate_distance_to_bottom_right(unit->loc[0], unit->loc[1]) <= 1;
            } else {
                // BOTTOM LEFT
                return calculate_distance_to_top_right(unit->loc[0], unit->loc[1]) <= 1;
            }
        }
    }

    std::vector<GameUnit*> getUnitsInCoord(const int& x, const int& y) {
        if (!in_arena(x, y)) {
            return std::vector<GameUnit*>();
        }
        return layout[x][y];
    }

    int calculate_distance_to_top_right(int x, int y) {
        int dist = 0;
        while (in_arena(x, y)) {
            ++x;
            ++dist;
            if (in_arena(x, y)) {
                ++y;
                ++dist;
            }
        }
        return dist;
    }

    int calculate_distance_to_bottom_right(int x, int y) {
        int dist = 0;
        while (in_arena(x, y)) {
            ++x;
            ++dist;
            if (in_arena(x, y)) {
                --y;
                ++dist;
            }
        }
        return dist;
    }
    
    int calculate_distance_to_top_left(int x, int y) {
        int dist = 0;
        while (in_arena(x, y)) {
            --x;
            ++dist;
            if (in_arena(x, y)) {
                ++y;
                ++dist;
            }
        }
        return dist;
    }

    int calculate_distance_to_bottom_left(int x, int y) {
        int dist = 0;
        while (in_arena(x, y)) {
            --x;
            ++dist;
            if (in_arena(x, y)) {
                --y;
                ++dist;
            }
        }
        return dist;
    }

    int calculate_smallest_distance_to_edge(int x, int y) {
        int dist = 0;
        if (x > 14) {
            if (y > 13) {
                // TOP RIGHT
                return calculate_distance_to_top_right(x, y);

            } else {
                // BOTTOM RIGHT
                return calculate_distance_to_bottom_right(x, y);
            }
        } else {
            if (y > 13) {
                // TOP LEFT
                return calculate_distance_to_top_left(x, y);
            } else {
                // BOTTOM LEFT
                return calculate_distance_to_bottom_left(x, y);
            }
        }
    }

    bool in_arena(const int x, const int y) {
        int emptyColumns = 0;
        if (y > 13) {
            emptyColumns = y - 14;
        } else {
            emptyColumns = 13 - y;
        }

        return x >= emptyColumns && x < MAP_LENGTH - emptyColumns;
    }

    std::vector<std::vector<int>> getNeighbours(std::vector<int> location) {
        std::vector<std::vector<int>> result;

        if (in_arena(location[0] - 1, location[1])) {
            result.push_back(std::vector<int> {location[0] - 1, location[1]});
        }

        if (in_arena(location[0] + 1, location[1])) {
            result.push_back(std::vector<int> {location[0] + 1, location[1]});
        }

        if (in_arena(location[0], location[1] - 1)) {
            result.push_back(std::vector<int> {location[0], location[1] - 1});
        }

        if (in_arena(location[0], location[1] + 1)) {
            result.push_back(std::vector<int> {location[0], location[1] + 1});
        }

        return result;
    }

    int getIdealness(const std::vector<int>& loc, const std::vector<int>& direction) {
        if (direction[0] == 0) {
            if (direction[1] == 0) {
                if (std::find(BOTTOM_LEFT.begin(), BOTTOM_LEFT.end(), loc) != BOTTOM_LEFT.end()) {
                    return 1000;
                } else {
                    return (27 - loc[1]) * 28 + (27 - loc[0]); 
                }
            } else {
                if (std::find(TOP_LEFT.begin(), TOP_LEFT.end(), loc) != TOP_LEFT.end()) {
                    return 1000;
                } else {
                    return loc[1] * 28 + (27 - loc[0]);
                }
            }
        } else {
            if (direction[1] == 0) {
                if (std::find(BOTTOM_RIGHT.begin(), BOTTOM_RIGHT.end(), loc) != BOTTOM_RIGHT.end()) {
                    return 1000;
                } else {
                    return (27 - loc[1]) * 28 + loc[0];
                }
            } else {
                if (std::find(TOP_RIGHT.begin(), TOP_RIGHT.end(), loc) != TOP_RIGHT.end()) {
                    return 1000;
                } else {
                    return loc[1] * 28 + loc[0];
                }
            }
        }
    }
};