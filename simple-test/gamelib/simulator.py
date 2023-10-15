from gamelib import GameUnit
from gamelib import GameState
from gamelib import debug_write
import os
import ctypes

class Simulator:
    def __init__(self, game_state):
        self.config = game_state.config
        self.largestCreationIndex = 0
        self.cSimLib = ctypes.pydll.LoadLibrary(os.path.dirname(__file__) + "/simulator_script/simulator.so")
        self.cSimLib.run_simulation.restype = ctypes.POINTER(ctypes.c_float)

        self.allUnits = []
        debug_write("P1 Units: {}".format(game_state.p1_units))
        for unit in game_state.p1_units:
            self.largestCreationIndex = max(unit.creationIndex, self.largestCreationIndex)
            self.allUnits.append((
                unit.x, 
                unit.y, 
                unit.health,
                0, 
                max(unit.damage_f, unit.damage_i), 
                unit.creationIndex, 
                unit.shieldPerUnit if not unit.upgraded else unit.shieldPerUnit + (unit.shieldBonusPerY * unit.y),
                unit.player_index, 
                max(unit.attackRange, unit.shieldRange)))
            
        for unit in game_state.p2_units:
            self.largestCreationIndex = max(unit.creationIndex, self.largestCreationIndex)
            self.allUnits.append((
                unit.x, 
                unit.y, 
                unit.health,
                0, 
                max(unit.damage_f, unit.damage_i), 
                unit.creationIndex, 
                unit.shieldPerUnit if not unit.upgraded else unit.shieldPerUnit + (unit.shieldBonusPerY * (27 - unit.y)),
                unit.player_index, 
                max(unit.attackRange, unit.shieldRange)))
                
        for unit_to_build in game_state._build_stack:
            if unit_to_build[0] == self.config["unitInformation"][6]["shorthand"] or unit_to_build[0] == self.config["unitInformation"][7]["shorthand"]:
                continue
            else:
                self.largestCreationIndex += 1
                unit = GameUnit(unit_to_build[0], self.config, player_index=0, x=unit_to_build[1], y=unit_to_build[2], creationIndex=self.largestCreationIndex)
                self.allUnits.append((
                    unit.x, 
                    unit.y, 
                    unit.health,
                    0, 
                    max(unit.damage_f, unit.damage_i), 
                    unit.creationIndex, 
                    unit.shieldPerUnit if not unit.upgraded else unit.shieldPerUnit + (unit.shieldBonusPerY * unit.y),
                    unit.player_index, 
                    max(unit.attackRange, unit.shieldRange)))
                
        for unit_to_build in game_state._deploy_stack:
            self.largestCreationIndex += 1
            unit = GameUnit(unit_to_build[0], self.config, player_index=0, x=unit_to_build[1], y=unit_to_build[2], creationIndex=self.largestCreationIndex)
            self.allUnits.append((
                unit.x, 
                unit.y, 
                unit.health,
                1 / unit.speed, 
                max(unit.damage_f, unit.damage_i), 
                unit.creationIndex, 
                0,
                unit.player_index, 
                max(unit.attackRange, unit.shieldRange)))

    def simluate_spawn(self, unit_type, locations, num=1):
        if type(locations[0]) == int:
            locations = [locations]

        for location in locations:
            for i in range(num):
                self.largestCreationIndex += 1
                x, y = map(int, location)
                unit = GameUnit(unit_type, self.config, player_index=0, x=x, y=y, creationIndex=self.largestCreationIndex)
                self.allUnits.append((
                    unit.x, 
                    unit.y, 
                    unit.health,
                    1 / unit.speed, 
                    max(unit.damage_f, unit.damage_i), 
                    unit.creationIndex,
                    unit.shieldPerUnit if not unit.upgraded else unit.shieldPerUnit + (unit.shieldBonusPerY * unit.y),
                    unit.player_index, 
                    max(unit.attackRange, unit.shieldRange)))

    def simulate_remove(self, location):
        x, y = map(int, location)
        self.allUnits = [unit for unit in self.allUnits if unit[0] != x or unit[1] != y]

    def run_simulation(self):
        num_units = len(self.allUnits)
        units_flattened = (ctypes.c_float * (9 * num_units))()

        for i in range(num_units):
            for j in range(9):
                units_flattened[i * 9 + j] = self.allUnits[i][j]

        debug_write("Printing args to sim....")
        for i in units_flattened:
            debug_write(i)

        debug_write("End of args...")
        resultArr = (ctypes.c_float * 10)()


        result = self.cSimLib.run_simulation(units_flattened, num_units, resultArr)

        resultList = []
        for i in range(10):
            resultList.append(result[i])
        return resultList
        
        