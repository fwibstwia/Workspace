%  User specifies his first two cards, the number of players, and the number of simulations
%  to run.  This routine runs the desired number of simulations of Texas Holdem and
%  reports the number of wins, losses, and ties.
%
%  Cards:
%    rank: 2=2, ..., 10=10, J=11, Q=12, K=13, A=14.  suit: C=1, D=2, H=3, S=4.
%    cardno = (rank-2)*4 + suit    (1--52)
%
taken = zeros(52,1);    %  Keep track of cards that have already been dealt.

%  Get input from user.
no_players = input(' Enter number of players: ');
first_two = input(' Enter first two cards: [cardno1; cardno2] ');
taken(first_two(1,1)) = 1;  taken(first_two(2,1)) = 1;   % Mark user's cards as taken.

no_plays = input(' Enter number of times to play this hand: ');

%  Loop over simulations, counting wins, losses, and ties.
wins = 0;  ties = 0;  losses = 0; takeninit = taken;
for play=1:no_plays,

%    Deal first two cards to other players.
  for m=2:no_players,
    for i=1:2,
      first_two(i,m) = fix(52*rand) + 1;      % Use uniform random distribution.
      while taken(first_two(i,m))==1,         % If card is already taken, try again.
        first_two(i,m) = fix(52*rand) + 1;
      end;
      taken(first_two(i,m)) = 1;              % Mark this card as taken.
    end;
  end;
%    Deal the flop, the turn, and the river card.
  show_cards = zeros(5,1);
  for i=1:5,
    show_cards(i) = fix(52*rand) + 1;         % Use uniform random distribution.
    while taken(show_cards(i))==1,            % If card is already taken, try again.
      show_cards(i) = fix(52*rand) + 1;
    end;
    taken(show_cards(i)) = 1;                 % Mark this card as taken.
  end;

%    See if hand 1 wins, loses, or ties.
  [winner, tiedhands] = whowins(no_players, first_two, show_cards);
  if winner==1, wins = wins+1;  elseif tiedhands(1)==1, ties = ties+1;
  else losses = losses+1; end;

  taken = takeninit;                          % Prepare for new hand.
end;
wins, losses, ties                            % Print results.