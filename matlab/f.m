function [y] = f(fname, x)
z = feval(fname, x);
y = 2*z.^2 + 3*z - 1;