numsims = 5000;  % Parameter controlling the number of simulations

% Connectivity matrix of the network
G = [0 0 0 1 0; 1 0 0 0 0; 1 0 0 0 0; 0 1 1 0 1; 0 0 1 0 0];
n = length(G);  % size of network

state = 1;      % initial state of the system

p = 0.85;       % probability of following link on web page
M = zeros(n,n); % create the matrix of probabilities
for i = 1:n;
    for j = 1:n;
        M(i,j) = p*G(i,j)/sum(G(i,:)) + (1-p)/n;
    end
end

pages = zeros(1,n);   % vector to hold number of times page visited

fprintf('\n\n');
fprintf('=====================================================\n');
fprintf('Computed Probabilities from %d simulations\n\n',numsims);
fprintf('Simulations  Page 1  Page 2  Page 3  Page 4  Page 5 \n');
fprintf('=====================================================\n');

% Simulate a surfer's session
for i=1:numsims
    prob = rand;
    if prob < M(state,1)
        state = 1;
    elseif prob < M(state,1) + M(state,2)
        state = 2;
    elseif prob < M(state,1) + M(state,2) + M(state,3)
        state = 3;
    elseif prob < M(state,1) + M(state,2) + M(state,3) + M(state,4)
        state = 4;
    else state = 5;
    end;

    pages(state) = pages(state) + 1;

    if ~mod(i,numsims/5)
       fprintf('%6d       %5.3f   %5.3f   %5.3f   %5.3f   %5.3f \n',i,pages/sum(pages));
    end
end
