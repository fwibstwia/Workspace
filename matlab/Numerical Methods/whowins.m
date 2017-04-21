function [winner, tiedhands] = whowins(no_players, first_two, show_cards)
%
%  For each player, this function uses function findhand to find his best
%  hand from among his first_two cards and the showcards.  With this
%  information, it then compares the hands and determines which one is
%  the winner.  If there is a tie, it lists all of the hands that tie
%  in the vector tiedhands.
%

winner = 0;  tieflag = 0;  tiedhands = [];

%  Loop over players.

for m=1:no_players

%    Store player's cards in form needed for function findhand.

  for i=1:2,
    cards(i,1) = fix((first_two(i,m)-1)/4) + 2;
    cards(i,2) = first_two(i,m) - 4*(cards(i,1)-2);
  end;
  for i=1:5,
    cards(i+2,1) = fix((show_cards(i)-1)/4) + 2;
    cards(i+2,2) = show_cards(i) - 4*(cards(i+2,1)-2);
  end;

%    Use findhand to find player's best hand and return it and its rank.

  [hand,handrank] = findhand(cards);

%    Determine who has the best hand so far, or if there is a tie.

  if m==1,
    winner = 1;  besthand = hand;  besthandrank = handrank;
  else
    if handrank < besthandrank,
      winner = m; besthand = hand; besthandrank = handrank; tieflag = 0; tiedhands = [];
    end;
    if handrank == besthandrank,
      i = 1;
      while i < 5 & hand(i,1)==besthand(i,1),
        i = i+1;
      end;
      if hand(i,1)==besthand(i,1), 
        if tieflag==0,
          tiedhands = [winner; m];
        else 
          tiedhands = [tiedhands; m];
        end;
        tieflag = 1;
      elseif hand(i,1) > besthand(i,1),
        winner = m; besthand = hand; besthandrank = handrank; tieflag = 0; tiedhands = [];
      end;
    end;
  end;

end;
if tieflag==1, winner = 0; end;       % if there is a tie, do not return a winner.
if tieflag==0, tiedhands = [0]; end;  % if there is a winner, return 0 for tiedhands.
