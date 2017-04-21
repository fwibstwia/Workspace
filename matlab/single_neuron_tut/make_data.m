data0 = repmat([1 2], 500, 1) + 0.5 * randn(500, 2);
data1 = repmat([3 1], 500, 1) + 0.5 * randn(500, 2);
data = [data0; data1];
labels = [zeros(500, 1); ones(500, 1)];

scatter(data(:, 1), data(:, 2), 5, labels);
