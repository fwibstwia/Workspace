function pi = EstimatePi(n)
numberin = 0;
for i = 1 : n
    x = 2 * rand - 1;
    y = 2 * rand - 1;
    if x^2 + y ^2 < 1
        numberin = numberin + 1;
    end;
end;
pio4 = numberin/n;
pi = 4 * pio4;
        