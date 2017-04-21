function show_neuron(data, labels, w, b)

scatter(data(:, 1), data(:, 2), 5, labels);

xmin = min(data(:, 1));
xmax = max(data(:, 1));

x = xmin:0.01:xmax;
y = -b/w(2) - (w(1)/w(2)) * x;

hold on;
plot(x, y, 'g');
hold off;
