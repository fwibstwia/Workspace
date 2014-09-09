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
A == rx * rx + ry * ry + rz * rz,
B == -2.0 * (sx * rx + sy * ry + sz * rz),
C == sx * sx + sy * sy + sz * sz - radius,
D == B * B - 4 * A * C,
D == E * E,
E > 0,
F == (-B - E)/(2*A)
]

problem = [
rx == 0.125,
ry == -1.0450602,
rz == 3.2025185,
sx == -1.6350498,
sz == -3.6311855,
C == -0.00001
]
set_option(rational_to_decimal=True)
solve(equations + problem)


