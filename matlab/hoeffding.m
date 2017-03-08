function [hy1, hyrand, hymin] = hoeffding
hy1 = zeros(11, 1);
hyrand = zeros(11, 1);
hymin = zeros(11, 1);
for i = 1:100000
    A = randi([0,1], 1000, 10);
    Ahead = sum(A, 2);
    hy1(Ahead(1)+1) = hy1(Ahead(1)+1) + 1;
    randindex = randi([1, 1000]);
    hyrand(Ahead(randindex)+1) = hyrand(Ahead(randindex)+1) + 1;
    hymin(min(Ahead)+1) = hymin(min(Ahead)+1) + 1;
end
hy1 = hy1./100000;
hyrand = hyrand./100000;
hymin = hymin./100000;
    