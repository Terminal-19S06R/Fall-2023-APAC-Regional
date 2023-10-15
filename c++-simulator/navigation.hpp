#include <queue>
#include "gameMap.hpp"

// Copy of navigation.py class

struct Node {
        bool visited_idealness = false;
        bool visited_validate = false;
        bool blocked = false;
        int pathlength = -1;
};

struct ShortestPathFinder {
    std::vector<std::vector<Node*>> pathFinderMap;
    GameMap* map;
    int HORIZONTAL = 1;
    int VERTICAL = 2;

    ShortestPathFinder() {}

    ShortestPathFinder(GameMap* map_) {
        map = map_;
    }

    std::queue<std::vector<int>> calculatePath(GameUnit* unit) {
        initialise_Map();
        std::vector<int> most_ideal = idealnessSearch(unit);
        validatePath(unit, most_ideal);
        return get_path(unit);
    }

    private:
    std::queue<std::vector<int>> get_path(GameUnit* unit) {
        std::queue<std::vector<int>> path;
        std::vector<int> current = unit->loc;

        bool isLastMoveHorizontal = unit -> isAtSpawnLocation() ? true : unit -> lastMoveWasHorizontal();

        while (pathFinderMap[current[0]][current[1]]->pathlength != 0) {
            std::vector<int> next = chooseNextMove(current, unit, isLastMoveHorizontal);
            isLastMoveHorizontal = current[1] == next[1];
            path.push(next);
            current = next;
        }
        return path;
    }

    std::vector<int> chooseNextMove(std::vector<int>& currentLocation, GameUnit* unit, bool isLastMoveHorizontal) {
        // PrevMoveDirection: 0 is horizontal, 1 is vertical
        std::vector<int> bestNextLocation;

        for (std::vector<int> neighbour : map -> getNeighbours(currentLocation)) {
            if (pathFinderMap[neighbour[0]][neighbour[1]]->pathlength != pathFinderMap[currentLocation[0]][currentLocation[1]]->pathlength - 1) {
                continue;
            }

            if (bestNextLocation.size() == 0) {
                bestNextLocation = neighbour;
                continue;
            } else {
                bestNextLocation = chooseBestNextLocation(bestNextLocation, neighbour, currentLocation, unit, isLastMoveHorizontal);
            }
        }

        return bestNextLocation;
    }

    std::vector<int> chooseBestNextLocation(
        std::vector<int>& nextLocation, 
        std::vector<int>& nextLocationContender, 
        std::vector<int>& currentLocation, 
        GameUnit* unit,
        bool isLastMoveHorizontal) {
        if (isLastMoveHorizontal) {
            if (nextLocation[1] > currentLocation[1] && nextLocationContender[1] <= currentLocation[1]) {
                return nextLocation;
            } else if (nextLocationContender[1] > currentLocation[1] && nextLocation[1] <= currentLocation[1]) {
                return nextLocationContender;
            }
        } else {
            if (nextLocation[0] > currentLocation[0] && nextLocationContender[0] <= currentLocation[0]) {
                return nextLocation;
            } else if (nextLocationContender[0] > currentLocation[0] && nextLocation[0] <= currentLocation[0]) {
                return nextLocationContender;
            }
        }

        std::vector<int> endpointDirection = unit -> getEndpointDirection();
        int differingIndex;
        if (nextLocation[0] == nextLocationContender[0]) {
            differingIndex = 1;
        } else {
            differingIndex = 0;
        }

        if (endpointDirection[differingIndex] == 0) {
            if (nextLocation[differingIndex] < nextLocationContender[differingIndex]) {
                return nextLocation;
            } else {
                return nextLocationContender;
            }
        } else {
            if (nextLocation[differingIndex] > nextLocationContender[differingIndex]) {
                return nextLocation;
            } else {
                return nextLocationContender;
            }
        }
    }

    std::vector<int> idealnessSearch(GameUnit* unit) {
        std::queue<std::vector<int>> current;

        current.push(unit->loc);
        std::vector<int> unitEndpointDirection = unit -> getEndpointDirection();
        int best_idealness = map -> getIdealness(unit -> loc, unitEndpointDirection);
        pathFinderMap[unit->loc[0]][unit->loc[1]]->visited_idealness = true;
        std::vector<int> most_ideal = unit->loc;

        while (!current.empty()) {
            std::vector<int> current_loc = current.front(); current.pop();
            for (std::vector<int> neighbour : map -> getNeighbours(current_loc)) {
                if (!map -> in_arena(neighbour[0], neighbour[1]) || pathFinderMap[neighbour[0]][neighbour[1]]->visited_idealness 
                    || pathFinderMap[neighbour[0]][neighbour[1]]->blocked) {
                        continue;
                }
                pathFinderMap[neighbour[0]][neighbour[1]]->visited_idealness = true;
                current.push(neighbour);
                int idealness = map -> getIdealness(neighbour, unitEndpointDirection);
                if (idealness > best_idealness) {
                    best_idealness = idealness;
                    most_ideal = neighbour;
                }
            }
        }
        return most_ideal;
    }

    void validatePath(GameUnit* unit, std::vector<int> most_ideal) {
        std::queue<std::vector<int>> current;
        std::vector<std::vector<int>> unitEndpoints = unit -> getEndpoints();
        if (std::find(unitEndpoints.begin(), unitEndpoints.end(), most_ideal) != unitEndpoints.end()) {
            for (std::vector<int> endpoint : unitEndpoints) {
                if (pathFinderMap[endpoint[0]][endpoint[1]]->blocked) {
                    continue;
                }
                current.push(endpoint);
                pathFinderMap[endpoint[0]][endpoint[1]]->pathlength = 0;
                pathFinderMap[endpoint[0]][endpoint[1]]->visited_validate = true;
            }
        } else {
            current.push(most_ideal);
            pathFinderMap[most_ideal[0]][most_ideal[1]]->pathlength = 0;
            pathFinderMap[most_ideal[0]][most_ideal[1]]->visited_validate = true;
        }

        while (!current.empty()) {
            std::vector<int> loc = current.front(); current.pop();
            for (std::vector<int> neighbour : map -> getNeighbours(loc)) {
                if (!map -> in_arena(neighbour[0], neighbour[1]) || pathFinderMap[neighbour[0]][neighbour[1]] -> blocked) {
                    continue;
                }
                
                Node* neighbourNode = pathFinderMap[neighbour[0]][neighbour[1]];
                if (neighbourNode -> visited_validate) {
                    continue;
                }
                
                neighbourNode->pathlength = pathFinderMap[loc[0]][loc[1]]->pathlength + 1;
                neighbourNode->visited_validate = true;
                current.push(neighbour);
            }
        }
    }

    void initialise_Map() {
        pathFinderMap = std::vector<std::vector<Node*>>(MAP_LENGTH, std::vector<Node*>(MAP_LENGTH));
        for (int i = 0; i < map->layout.size(); i++) {
            for (int j = 0; j < map->layout[i].size(); j++) {
                pathFinderMap[i][j] = new Node();
                if (map->blocked[i][j]) {
                    pathFinderMap[i][j]->blocked = true;
                }
            }
        }
    }
};