N = input(' Enter number of sample points: ');
rho = inline('exp(0.5*z)');     % Here rho is a function of z only.

volumeOfBox = 10*10*10;         % Volume of surrounding box.

vol = 0;  mass = 0;             % Initialize volume and mass of object.
volsq = 0;  masssq = 0;         % Initialize the squares of these random variables
                                % (to be used in computing variance and standard deviation).

for i=1:N,                      % Loop over sample points.
  x = -5 + 10*rand;             % Generate a point from surrounding box.
  y = -5 + 10*rand;
  z = -5 + 10*rand;
  if x*y*z <= 1,                % Check if point is inside object.
    vol = vol + 1;              %    If so, add to vol and mass.
    mass = mass + rho(z);
    volsq = volsq + 1;          %    Also add to square of vol and square of mass.
    masssq = masssq + rho(z)^2;
  end;
end;

volumeOfObject = (vol/N)*volumeOfBox   % Fraction of points inside times volume of box
volvar = (1/N)*((volsq/N) - (vol/N)^2)*volumeOfBox^2;  % Variance in volume of object:
                                       % (Expected value of volsq - square of expected
                                       % value of vol) divided by N and multiplied by
                                       % square of factor volumeOfBox.
volstd = sqrt(volvar)                  % Standard deviation in volume of object.

massOfObject = (mass/N)*volumeOfBox    % Average mass (0 outside object) times volume of box
massvar = (1/N)*((masssq/N) - (mass/N)^2)*volumeOfBox^2; % Variance in mass of object:
                                       % (Expected value of masssq - square of expected
                                       % value of mass) divided by N and multiplied by
                                       % square of factor volumeOfBox.
massstd = sqrt(massvar)                % Standard deviation in mass of object.
