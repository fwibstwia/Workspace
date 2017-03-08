function x = GaussEliminate(A, b)
n = length(b);
for j = 1:n-1
    for i = j+1:n
        mult = A(i,j) / A(j,j);
        A(i,j+1:n) = A(i, j+1:n) - mult*A(j, j+1:n);
        A(i,j) = mult;
    end
    y = lSolve(A, b);
    x = uSolve(A, y);
end
function y = lSolve(L, b)
y = zeros(length(b), 1);
y(1) = b(1);
for i = 2 : length(b)
    for j = 1 : i - 1
        s = L(i,j) * y(j);
    end
    y(i) = b(i) - s;
end
function x = uSolve(U, y)
n = length(y);
for i = n:-1:1
    x(i) = y(i);
    for j = n:-1:i + 1
        x(i) = x(i) - U(i,j)*x(j);
    end
    x(i) = x(i)/U(i,i);
end

