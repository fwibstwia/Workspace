
from z3 import *
import numpy as np
from fractions import Fraction
rx = Real('rx')
ry = Real('ry')
rz = Real('rz')
sx = Real('sx')
sy = Real('sy')
sz = Real('sz')
s1 = Real('s1')
s2 = Real('s2')
s3 = Real('s3')
radius = Real('radius')
A, B, C, D, E, F = Reals('A B C D E F')
A = rx*rx + ry*ry + rz*rz + sx*sx + sy*sy + sz*sz 
s = Solver()
s.add(A == 12.37)
set_option(rational_to_decimal=True)
set_param('nlsat.seed', 1024)
print get_param('nlsat.shuffle_vars')
fo = open("sum3.txt", "wb")
c = 1;

f = np.float32(0);
g = np.float32(0);
h = np.float32(0);
k = np.float32(0);

while (c == 1):
   s.check()
   m = s.model()

   if(is_rational_value(m[rx])):
      f = np.float32(m[rx].as_fraction())
   else:
      f = np.float32(m[rx].approx(20).as_fraction())

   if(is_rational_value(m[ry])):
      g = np.float32(m[ry].as_fraction())
   else:
      g = np.float32(m[ry].approx(20).as_fraction())

   if(is_rational_value(m[rz])):
      h = np.float32(m[rz].as_fraction())
   else:
      h = np.float32(m[rz].approx(20).as_fraction())
   
   i = np.float32((f*f + g*g) + h*h)
   j = np.float32(f*f + (g*g + h*h))
   i_string = "{0:.20f}".format(i)
   j_string = "{0:.20f}".format(j)
   print i_string
   print j_string
   if(i != j):
      c = 0
   fo.write("{0} {1} {2}\n".format(m[rx], m[ry], m[rz]))   
   nf = f + 0.00000001
   pf = f - 0.00000001
   nf_bound = nf;
   pf_bound = pf;
   s.add(Or(rx < pf_bound, rx > nf_bound))
   s.add(ry != m[ry])
   s.add(rz != m[rz])
fo.close()
