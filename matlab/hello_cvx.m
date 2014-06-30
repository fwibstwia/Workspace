m = 16; n = 8;
A = randn(m,n);
b = randn(m,1);

l1 = 4
u1 = 5
l2 = 2
u2 = 3
l3 = 6
u3 = 7
cvx_begin
    variable B
    variable C
    variable A
    minimize(B*B - 4*A*C)
    subject to 
        l1 <= B <= u1
        l2 <= A <= u2
        l3 <= C <= u3
cvx_end