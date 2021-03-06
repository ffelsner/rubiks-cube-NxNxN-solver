
from rubikscubennnsolver import RubiksCube
from rubikscubennnsolver.LookupTable import LookupTable
from rubikscubennnsolver.RubiksCube222 import moves_222
import logging

log = logging.getLogger(__name__)

moves_333 = moves_222
solved_333 = 'UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB'
centers_333 = (5, 14, 23, 32, 41, 50)

edges_333 = (
    2, 4, 6, 8,
    11, 13, 15, 17,
    20, 22, 24, 26,
    29, 31, 33, 35,
    38, 40, 42, 44,
    47, 49, 51, 53
)

corners_333 = (
    1, 3, 7, 9,
    10, 12, 16, 18,
    19, 21, 25, 27,
    28, 30, 34, 36,
    37, 39, 43, 45,
    46, 48, 52, 54
)


class LookupTable333Phase2Edges(LookupTable):
    """
    lookup-table-3x3x3-step11-edges.txt
    ===================================
    1 steps has 10 entries (0 percent, 0.00x previous step)
    2 steps has 68 entries (0 percent, 6.80x previous step)
    3 steps has 456 entries (0 percent, 6.71x previous step)
    4 steps has 3063 entries (0 percent, 6.72x previous step)
    5 steps has 18202 entries (1 percent, 5.94x previous step)
    6 steps has 86691 entries (8 percent, 4.76x previous step)
    7 steps has 290812 entries (30 percent, 3.35x previous step)
    8 steps has 434814 entries (44 percent, 1.50x previous step)
    9 steps has 120488 entries (12 percent, 0.28x previous step)
    10 steps has 11818 entries (1 percent, 0.10x previous step)
    11 steps has 1114 entries (0 percent, 0.09x previous step)
    12 steps has 144 entries (0 percent, 0.13x previous step)

    Total: 967680 entries
    Average: 7.601348 moves
    """

    def __init__(self, parent):
        LookupTable.__init__(
            self,
            parent,
            'lookup-table-3x3x3-step11-edges.txt',
            'UUUULLLLFFFFRRRRBBBBDDDD',
            linecount=967680,
            max_depth=12)

    def state(self):
        parent_state = self.parent.state
        result = ''.join([parent_state[x] for x in edges_333])
        return result


class LookupTable333Phase2Corners(LookupTable):
    """
    lookup-table-3x3x3-step12-corners.txt
    =====================================
    1 steps has 10 entries (0 percent, 0.00x previous step)
    2 steps has 68 entries (0 percent, 6.80x previous step)
    3 steps has 330 entries (0 percent, 4.85x previous step)
    4 steps has 752 entries (1 percent, 2.28x previous step)
    5 steps has 1400 entries (3 percent, 1.86x previous step)
    6 steps has 2752 entries (6 percent, 1.97x previous step)
    7 steps has 4384 entries (10 percent, 1.59x previous step)
    8 steps has 6208 entries (15 percent, 1.42x previous step)
    9 steps has 7136 entries (17 percent, 1.15x previous step)
    10 steps has 8064 entries (20 percent, 1.13x previous step)
    11 steps has 6528 entries (16 percent, 0.81x previous step)
    12 steps has 2432 entries (6 percent, 0.37x previous step)
    13 steps has 256 entries (0 percent, 0.11x previous step)

    Total: 40320 entries
    Average: 8.858929 moves
    """

    def __init__(self, parent):
        LookupTable.__init__(
            self,
            parent,
            'lookup-table-3x3x3-step12-corners.txt',
            'UUUULLLLFFFFRRRRBBBBDDDD',
            linecount=40320,
            max_depth=13)

    def state(self):
        parent_state = self.parent.state
        result = ''.join([parent_state[x] for x in corners_333])
        return result


class RubiksCube333(RubiksCube):

    def phase(self):
        return 'Solve 3x3x3'

    def solve(self):
        self.rotate_U_to_U()
        self.rotate_F_to_F()

        if self.get_state_all() != 'UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD':
            self.solve_333()
            self.compress_solution()

    def lt_init(self):
        if self.lt_init_called:
            return
        self.lt_init_called = True

        self.lt_phase2_edges = LookupTable333Phase2Edges(self)
        self.lt_phase2_corners = LookupTable333Phase2Corners(self)

swaps_333 = {'B': (0, 30, 33, 36, 4, 5, 6, 7, 8, 9, 3, 11, 12, 2, 14, 15, 1, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 54, 31, 32, 53, 34, 35, 52, 43, 40, 37, 44, 41, 38, 45, 42, 39, 46, 47, 48, 49, 50, 51, 10, 13, 16),
 "B'": (0, 16, 13, 10, 4, 5, 6, 7, 8, 9, 52, 11, 12, 53, 14, 15, 54, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 1, 31, 32, 2, 34, 35, 3, 39, 42, 45, 38, 41, 44, 37, 40, 43, 46, 47, 48, 49, 50, 51, 36, 33, 30),
 'B2': (0, 54, 53, 52, 4, 5, 6, 7, 8, 9, 36, 11, 12, 33, 14, 15, 30, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 16, 31, 32, 13, 34, 35, 10, 45, 44, 43, 42, 41, 40, 39, 38, 37, 46, 47, 48, 49, 50, 51, 3, 2, 1),
 'D': (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 43, 44, 45, 19, 20, 21, 22, 23, 24, 16, 17, 18, 28, 29, 30, 31, 32, 33, 25, 26, 27, 37, 38, 39, 40, 41, 42, 34, 35, 36, 52, 49, 46, 53, 50, 47, 54, 51, 48),
 "D'": (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 25, 26, 27, 19, 20, 21, 22, 23, 24, 34, 35, 36, 28, 29, 30, 31, 32, 33, 43, 44, 45, 37, 38, 39, 40, 41, 42, 16, 17, 18, 48, 51, 54, 47, 50, 53, 46, 49, 52),
 'D2': (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 34, 35, 36, 19, 20, 21, 22, 23, 24, 43, 44, 45, 28, 29, 30, 31, 32, 33, 16, 17, 18, 37, 38, 39, 40, 41, 42, 25, 26, 27, 54, 53, 52, 51, 50, 49, 48, 47, 46),
 'F': (0, 1, 2, 3, 4, 5, 6, 18, 15, 12, 10, 11, 46, 13, 14, 47, 16, 17, 48, 25, 22, 19, 26, 23, 20, 27, 24, 21, 7, 29, 30, 8, 32, 33, 9, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 34, 31, 28, 49, 50, 51, 52, 53, 54),
 "F'": (0, 1, 2, 3, 4, 5, 6, 28, 31, 34, 10, 11, 9, 13, 14, 8, 16, 17, 7, 21, 24, 27, 20, 23, 26, 19, 22, 25, 48, 29, 30, 47, 32, 33, 46, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 12, 15, 18, 49, 50, 51, 52, 53, 54),
 'F2': (0, 1, 2, 3, 4, 5, 6, 48, 47, 46, 10, 11, 34, 13, 14, 31, 16, 17, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 29, 30, 15, 32, 33, 12, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 9, 8, 7, 49, 50, 51, 52, 53, 54),
 'L': (0, 45, 2, 3, 42, 5, 6, 39, 8, 9, 16, 13, 10, 17, 14, 11, 18, 15, 12, 1, 20, 21, 4, 23, 24, 7, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 52, 40, 41, 49, 43, 44, 46, 19, 47, 48, 22, 50, 51, 25, 53, 54),
 "L'": (0, 19, 2, 3, 22, 5, 6, 25, 8, 9, 12, 15, 18, 11, 14, 17, 10, 13, 16, 46, 20, 21, 49, 23, 24, 52, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 7, 40, 41, 4, 43, 44, 1, 45, 47, 48, 42, 50, 51, 39, 53, 54),
 'L2': (0, 46, 2, 3, 49, 5, 6, 52, 8, 9, 18, 17, 16, 15, 14, 13, 12, 11, 10, 45, 20, 21, 42, 23, 24, 39, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 25, 40, 41, 22, 43, 44, 19, 1, 47, 48, 4, 50, 51, 7, 53, 54),
 'R': (0, 1, 2, 21, 4, 5, 24, 7, 8, 27, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 48, 22, 23, 51, 25, 26, 54, 34, 31, 28, 35, 32, 29, 36, 33, 30, 9, 38, 39, 6, 41, 42, 3, 44, 45, 46, 47, 43, 49, 50, 40, 52, 53, 37),
 "R'": (0, 1, 2, 43, 4, 5, 40, 7, 8, 37, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 3, 22, 23, 6, 25, 26, 9, 30, 33, 36, 29, 32, 35, 28, 31, 34, 54, 38, 39, 51, 41, 42, 48, 44, 45, 46, 47, 21, 49, 50, 24, 52, 53, 27),
 'R2': (0, 1, 2, 48, 4, 5, 51, 7, 8, 54, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 43, 22, 23, 40, 25, 26, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 38, 39, 24, 41, 42, 21, 44, 45, 46, 47, 3, 49, 50, 6, 52, 53, 9),
 'U': (0, 7, 4, 1, 8, 5, 2, 9, 6, 3, 19, 20, 21, 13, 14, 15, 16, 17, 18, 28, 29, 30, 22, 23, 24, 25, 26, 27, 37, 38, 39, 31, 32, 33, 34, 35, 36, 10, 11, 12, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54),
 "U'": (0, 3, 6, 9, 2, 5, 8, 1, 4, 7, 37, 38, 39, 13, 14, 15, 16, 17, 18, 10, 11, 12, 22, 23, 24, 25, 26, 27, 19, 20, 21, 31, 32, 33, 34, 35, 36, 28, 29, 30, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54),
 'U2': (0, 9, 8, 7, 6, 5, 4, 3, 2, 1, 28, 29, 30, 13, 14, 15, 16, 17, 18, 37, 38, 39, 22, 23, 24, 25, 26, 27, 10, 11, 12, 31, 32, 33, 34, 35, 36, 19, 20, 21, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54),
 'x': (0, 19, 20, 21, 22, 23, 24, 25, 26, 27, 12, 15, 18, 11, 14, 17, 10, 13, 16, 46, 47, 48, 49, 50, 51, 52, 53, 54, 34, 31, 28, 35, 32, 29, 36, 33, 30, 9, 8, 7, 6, 5, 4, 3, 2, 1, 45, 44, 43, 42, 41, 40, 39, 38, 37),
 "x'": (0, 45, 44, 43, 42, 41, 40, 39, 38, 37, 16, 13, 10, 17, 14, 11, 18, 15, 12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 30, 33, 36, 29, 32, 35, 28, 31, 34, 54, 53, 52, 51, 50, 49, 48, 47, 46, 19, 20, 21, 22, 23, 24, 25, 26, 27),
 'x2': (0, 46, 47, 48, 49, 50, 51, 52, 53, 54, 18, 17, 16, 15, 14, 13, 12, 11, 10, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 1, 2, 3, 4, 5, 6, 7, 8, 9),
 'y': (0, 7, 4, 1, 8, 5, 2, 9, 6, 3, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 10, 11, 12, 13, 14, 15, 16, 17, 18, 48, 51, 54, 47, 50, 53, 46, 49, 52),
 "y'": (0, 3, 6, 9, 2, 5, 8, 1, 4, 7, 37, 38, 39, 40, 41, 42, 43, 44, 45, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 52, 49, 46, 53, 50, 47, 54, 51, 48),
 'y2': (0, 9, 8, 7, 6, 5, 4, 3, 2, 1, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 54, 53, 52, 51, 50, 49, 48, 47, 46),
 'z': (0, 16, 13, 10, 17, 14, 11, 18, 15, 12, 52, 49, 46, 53, 50, 47, 54, 51, 48, 25, 22, 19, 26, 23, 20, 27, 24, 21, 7, 4, 1, 8, 5, 2, 9, 6, 3, 39, 42, 45, 38, 41, 44, 37, 40, 43, 34, 31, 28, 35, 32, 29, 36, 33, 30),
 "z'": (0, 30, 33, 36, 29, 32, 35, 28, 31, 34, 3, 6, 9, 2, 5, 8, 1, 4, 7, 21, 24, 27, 20, 23, 26, 19, 22, 25, 48, 51, 54, 47, 50, 53, 46, 49, 52, 43, 40, 37, 44, 41, 38, 45, 42, 39, 12, 15, 18, 11, 14, 17, 10, 13, 16),
 'z2': (0, 54, 53, 52, 51, 50, 49, 48, 47, 46, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 45, 44, 43, 42, 41, 40, 39, 38, 37, 9, 8, 7, 6, 5, 4, 3, 2, 1)}

def rotate_333(cube, step):
    return [cube[x] for x in swaps_333[step]]
