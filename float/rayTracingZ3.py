from z3 import *
rx = Real('rx')
ry = Real('ry')
rz = Real('rz')
sx = Real('sx')
sy = Real('sy')
sz = Real('sz')
radius = Real('radius')
s = Solver()
A = rx * rx + ry * ry + rz * rz
B = -2.0 * (sx * rx + sy * ry + sz * rz)
C = sx * sx + sy * sy + sz * sz - radius 
D = B * B - 4 * A * C
ldist = (-B - Sqrt(D))/(2*A) 
s.add(D > 0)
s.add(ry == -1.0450602)
s.add(rz == 3.2025185)
s.add(sx == -1.6350498)
s.add(sy == 4.8326674)
s.add(sz == -3.6311855)
s.add(ldist == 0.0)
set_option(rational_to_decimal=True)
print(s.check())
print(s.model())

