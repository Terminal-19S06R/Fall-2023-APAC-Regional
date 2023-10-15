import gamelib
import random
import math
import warnings
from sys import maxsize
import json


"""
Most of the algo code you write will be in this file unless you create new
modules yourself. Start by modifying the 'on_turn' function.

Advanced strategy tips: 

  - You can analyze action frames by modifying on_action_frame function

  - The GameState.map object can be manually manipulated to create hypothetical 
  board states. Though, we recommended making a copy of the map to preserve 
  the actual current map state.
"""

class AlgoStrategy(gamelib.AlgoCore):
    def __init__(self):
        super().__init__()
        seed = random.randrange(maxsize)
        random.seed(seed)
        gamelib.debug_write('Random seed: {}'.format(seed))

    def on_game_start(self, config):
        """ 
        Read in config and perform any initial setup here 
        """
        gamelib.debug_write('Configuring your custom algo strategy...')
        self.config = config
        global WALL, SUPPORT, TURRET, SCOUT, DEMOLISHER, INTERCEPTOR, MP, SP, RIGHT, LEFT
        RIGHT = "defend"
        LEFT = "defend"
        WALL = config["unitInformation"][0]["shorthand"]
        SUPPORT = config["unitInformation"][1]["shorthand"]
        TURRET = config["unitInformation"][2]["shorthand"]
        SCOUT = config["unitInformation"][3]["shorthand"]
        DEMOLISHER = config["unitInformation"][4]["shorthand"]
        INTERCEPTOR = config["unitInformation"][5]["shorthand"]
        MP = 1
        SP = 0
        # This is a good place to do initial setup
        self.scored_on_locations = []
        self.REBUILT = []
        self.STAGGER_LEFT = [[20,6],[19,5]]
        self.STAGGER_RIGHT = [[7,6],[8,5]]

    def on_turn(self, turn_state):
        """
        This function is called every turn with the game state wrapper as
        an argument. The wrapper stores the state of the arena and has methods
        for querying its state, allocating your current resources as planned
        unit deployments, and transmitting your intended deployments to the
        game engine.
        """
        game_state = gamelib.GameState(self.config, turn_state)
        gamelib.debug_write('Performing turn {} of your custom algo strategy'.format(game_state.turn_number))
        game_state.suppress_warnings(True)  #Comment or remove this line to enable warnings.

        self.starter_strategy(game_state)

        game_state.submit_turn()


    """
    NOTE: All the methods after this point are part of the sample starter-algo
    strategy and can safely be replaced for your custom algo.
    """
 
    def starter_strategy(self, game_state):

        self.rebuild_defences(game_state)
        sp = game_state.get_resources(0)[0]
        if game_state.turn_number > 4 and :
            sp = self.build_defences_corner_layer2(game_state, sp)
            sp = self.build_defences_corner_layer1(game_state, sp)
        sp = self.check_defences(game_state, sp)
        sp = self.build_defences_corner_layer1(game_state, sp)
        sp = self.build_reactive_middle(game_state, sp)
        sp = self.build_corner_upgrade1(game_state, sp)
        sp = self.build_defences_corner_layer2(game_state, sp)
        #sp = self.build_corner_turret(game_state, sp)
        sp = self.build_middle_upgrade(game_state, sp)
        sp = self.build_corner_upgrade2(game_state, sp)
        sp = self.build_support(game_state, sp)
        sp = self.build_more_support(game_state, sp)
        trap_location = []
        if RIGHT == "attack":
            trap_location += [[13,0],]
        if LEFT == "attack":
            trap_location += [[14,0],]
    """
    DEFENCE
    """
    def rebuild_defences(self, game_state):
        remaining = []
        if len(self.REBUILT) > 0:
            for wall in self.REBUILT:
                value = game_state.attempt_spawn(WALL, wall)
                if not value:
                    remaining.append(wall)
        self.REBUILT = remaining
            
    
    def check_defences(self, game_state, sp):
        plus5 = sp + 5
        wall_upgrade_protection = [[7,7], [20,7], [ 0, 13],[ 27, 13],[ 1, 12],[ 26, 12],[ 2, 11],[ 25, 11],[ 3, 10],[24, 10],[ 4, 9],[ 23, 9],[ 5, 8],[ 22, 8],[ 6, 7],[ 21, 7],]
        for wall in wall_upgrade_protection:
            value = game_state.contains_stationary_unit(wall)
            if value and value.unit_type == WALL and value.health<= 70 and plus5>0:
                if game_state.attempt_remove(wall):
                    self.REBUILT.append(wall)
                    plus5 -= 1
            if value and value.unit_type == WALL and value.health<= 60 and plus5>0 and not value.upgraded:
                    game_state.attempt_upgrade(wall)
                    plus5 -= 1
        return min(plus5, sp)

    def build_defences_corner_layer1(self, game_state, sp):
        # Useful tool for setting up your base locations: https://www.kevinbai.design/terminal-map-maker
        # More community tools available at: https://terminal.c1games.com/rules#Download
        wall_locations = [[ 0, 13],[ 27, 13],[ 1, 12],[ 26, 12],[ 2, 11],[ 25, 11],]
        for wall in wall_locations:
            if sp > 0:
                if game_state.attempt_spawn(WALL, wall):
                    sp -= 1
        return sp

    def build_defences_corner_layer2(self, game_state, sp):
        wall_locations = [[24, 10],[ 4, 9],[ 23, 9],[ 5, 8],[ 22, 8],[ 6, 7],[ 21, 7],[3,10]]
        for wall in wall_locations:
            if sp > 0:
                if game_state.attempt_spawn(WALL, wall):
                    sp -= 1
        return sp
            
    def build_reactive_middle(self, game_state, sp):
        main_turret = [[ 5, 11],[ 22, 11]]
        main_turret_wall = [[ 5, 12],[ 22, 12]]
        wall_locations = [[ 6, 10],[ 21, 10],[ 7, 9],[ 20, 9],[ 8, 8],[ 9, 8],[ 10, 8],[ 11, 8],[ 12, 8],[ 13, 8],[ 14, 8],[ 15, 8],[ 16, 8],[ 17, 8],[ 18, 8],[ 19, 8]]
        trap_right = [7,7]
        trap_left = [20, 7]        
        if RIGHT == "defend":
            if sp >0:
                value = game_state.attempt_spawn(WALL, trap_right)
                if value:
                    sp-=1
        if LEFT == "defend":
            if sp>0:
                value = game_state.attempt_spawn(WALL, trap_left)
                if value:
                    sp-=1
        for turret in main_turret:
            if sp > 0:
                value = game_state.attempt_spawn(TURRET, turret)
                if value:
                    sp-=2
            if sp > 0:
                value = game_state.attempt_upgrade(turret)
                if value:
                    sp-=4
        for wall in main_turret_wall:
            if sp > 0:
                value = game_state.attempt_spawn(WALL, wall)
                if value:
                    sp-=1
            if sp > 0:
                value = game_state.attempt_upgrade(wall)
                if value:
                    sp-=1
        for wall in wall_locations:
            if sp > 0:
                value = game_state.attempt_spawn(WALL, wall)
                if value:
                    sp-=1
        return sp
    def build_corner_upgrade1(self, game_state, sp):
        wall_locations = [[ 0, 13],[ 27, 13],[ 1, 12],[ 26, 12],[ 2, 11],[ 25, 11],]
        for wall in wall_locations:
            if sp > 0:
                if game_state.attempt_upgrade(wall):
                    sp -= 1
        return sp

    def build_corner_upgrade2(self, game_state, sp):
        wall_locations = [[3,10], [ 24, 10],[ 4, 9],[ 23, 9],[ 5, 8],[ 22, 8],[ 6, 7],[ 21, 7],]
        for wall in wall_locations:
            if sp > 0:
                if game_state.attempt_upgrade(wall):
                    sp -= 1
        return sp

    def build_middle_upgrade(self, game_state, sp):
        turret_locations = [[ 6, 11],[ 7, 11],[ 20, 11],[ 21, 11],[ 7, 10],[ 20, 10]]        
        for turret in turret_locations:
            if sp > 0:
                value = game_state.attempt_spawn(TURRET, turret)
                if value:
                    sp-=2
        for turret in turret_locations: 
            if sp > 0:
                if game_state.attempt_upgrade(turret):
                    sp -= 4
        return sp
    def build_support(self, game_state, sp):
        support_locations = [[10,7],[17,7],[11,6],[16,6]]
        for support in support_locations:
            if sp > 0:
                value = game_state.attempt_spawn(SUPPORT, support)
                if value:
                    sp-=4
        return sp
    def build_corner_turret(self, game_state, sp):
        turret_locations = [[ 1, 13],[ 26, 13]]
        for turret in turret_locations:
            if sp > 0:
                value = game_state.attempt_spawn(TURRET, turret)
                if value:
                    sp-=2
        return sp
    def build_more_support(self, game_state, sp):
        support_locations = [[ 12, 6],[ 13, 6],[ 14, 6],[ 15, 6]]
        for support in support_locations:
            if sp > 0:
                value = game_state.attempt_spawn(SUPPORT, support)
                if value:
                    sp-=4
        return sp


    """
    ATTACK
    """
    def heuristic():
        return False
        return True
    

    def calculate_future(self, game_state):
        most_efficient = []
        # api call
        for x iterate:
        if heuristic():
            #removewalls
            set attack;
            #attack
        else:
            #buildwalls
            setdefend;

    def calculate_current(self, game_state):
        most_efficient = []
        #api call
        for x iterate:
        

    def on_action_frame(self, turn_string):
        """
        This is the action frame of the game. This function could be called 
        hundreds of times per turn and could slow the algo down so avoid putting slow code here.
        Processing the action frames is complicated so we only suggest it if you have time and experience.
        Full doc on format of a game frame at in json-docs.html in the root of the Starterkit.
        """
        # Let's record at what position we get scored on
        state = json.loads(turn_string)
        events = state["events"]
        breaches = events["breach"]
        for breach in breaches:
            location = breach[0]
            unit_owner_self = True if breach[4] == 1 else False
            # When parsing the frame data directly, 
            # 1 is integer for yourself, 2 is opponent (StarterKit code uses 0, 1 as player_index instead)
            if not unit_owner_self:
                gamelib.debug_write("Got scored on at: {}".format(location))
                self.scored_on_locations.append(location)
                gamelib.debug_write("All locations: {}".format(self.scored_on_locations))


if __name__ == "__main__":
    algo = AlgoStrategy()
    algo.start()
