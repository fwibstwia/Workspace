w = 0.01 * randn(size(data, 2), 1);
b = 0;
epsilon = 0.0001;

x = data;
t = labels;

for i=1:1000
  show_neuron(x, t, w, b);
  pause(0.1);
  z = x * w + b;
  y = 1 ./ (1 + exp(-z));

  prediction = y > 0.5;

  acc = mean(prediction == t);

  L = 0.5 * sum((y - t).^2);
  fprintf('w(1) = %.4f w(2) = %.4f b = %.4f L = %.4f acc = %.4f\n', w(1), w(2), b, sum(L), acc);

  % Compute dw, db.
  dLbydy = y - t;
  dLbydz = dLbydy .* (y .* (1 - y));
  dLbydw = x' * dLbydz;
  dLbydb = sum(dLbydz);

  dw = -dLbydw * epsilon;
  db = -dLbydb * epsilon;

  w = w + dw;
  b = b + db;
end
