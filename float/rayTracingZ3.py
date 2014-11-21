from z3 import *
rx = Real('rx')
ry = Real('ry')
rz = Real('rz')
sx = Real('sx')
sy = Real('sy')
sz = Real('sz')
radius = Real('radius')
A, B, C, D, E, F = Reals('A B C D E F')

equations = [
A == rx * rx * rx *rx + rx*rx*ry * ry + 1,
B == 4*rx*rx*ry*ry + 2,
D == A*A + B*B
]

problem = [
10 < rx < 100,
D == 100
]
set_option(rational_to_decimal=True)
solve(equations + problem)


