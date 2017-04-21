function [hand,handrank] = findhand(cards)
%
%  This function finds the best hand from the given cards and 
%  returns a number from 1 (royal flush) to 10 (nothing), according
%  to where the hand ranks.  It does not distinguish between hands
%  in the same category.
%
%     Royal flush = 1
%     Straight flush = 2
%     Four of a kind = 3
%     Full house = 4
%     Flush = 5
%     Straight = 6
%     Three of a kind = 7
%     Two pair = 8
%     Two of a kind = 9
%     Anything else = 10
%

[n,m] = size(cards);
[sortcards(:,1),indx] = sort(-cards(:,1));
sortcards(:,1) = -sortcards(:,1);
sortcards(:,2) = cards(indx,2);

%  Check for flush.

flush = 0;
clubs = 0; diamonds = 0; hearts = 0; spades = 0;
for i=1:n,
  if sortcards(i,2)==1, clubs = clubs+1; end;
  if sortcards(i,2)==2, diamonds = diamonds+1; end;
  if sortcards(i,2)==3, hearts = hearts+1; end;
  if sortcards(i,2)==4, spades = spades+1; end;
end;
if clubs >=5 | diamonds >=5 | hearts >=5 | spades >=5 ,
  flush = 1;
end;

if flush==1,                            % Flush
  if clubs >=5, suit = 1; end;
  if diamonds >=5, suit = 2; end;
  if hearts >=5, suit = 3; end;
  if spades >=5, suit = 4; end;

  %  Separate out cards of this suit.

  hand = [];
  for i=1:n,
    if sortcards(i,2)==suit, hand=[hand; sortcards(i,:)]; end;
  end;

  %  Check for a straight flush.

  if hand(1,1)==14, hand = [hand;1 hand(1,2)]; end;  % If there is an ace,
                                         % it can count as a 1 as well.
  i1 = 1;  count = 1;
  i = 2;
  while i <= length(hand(:,1)) & count < 5,
    if hand(i,1)==hand(i-1,1)-1, 
      count = count+1;
    else
      i1 = i;  count = 1;
    end;
    i = i+1;
  end;
  if count >= 5,                         %  Straight flush.
    if hand(i1,1)==14, 
      handrank = 1;
    else
      handrank = 2;
    end;
    hand = hand(i1:i1+4,:);
  else                                   %  Ordinary flush.
    handrank = 5;  hand = hand(1:5,:);
  end;
  return

else                                    %  No flush.

  hand = sortcards;

  %  Check for straight.

  if hand(1,1)==14, hand=[hand; 1 hand(1,2)]; end;   % If there is an ace,
                                        %  it can count as a 1 as well.
  i1 = 1;  count = 1;
  i = 2;
  while i <= length(hand(:,1)) & count < 5,
    if hand(i,1)==hand(i-1,1)-1,
      count = count+1;
    else
      if hand(i,1)~=hand(i-1,1),
        i1 = i;  count = 1;
      end;
    end;
    i = i+1;
  end;
  if count >= 5,                         %  Straight.
    handrank = 6;
%    hand = hand(i1:i1+4,:);
    temphand(1,:) = hand(i1,:);
    kount = 1;
    i = i1 + 1;
    while i <= length(hand(:,1)) & kount < 5,
      if hand(i,1)==hand(i-1,1)-1,
        kount = kount+1;
        temphand = [temphand; hand(i,:)];
      end;
      i = i+1;
    end;
    hand = temphand;
    return
  else                                   %  No straight.

    %  Find largest number of a kind.

    i1 = 1;  count = 1;  maxcount = 1;
    i = 2;
    while i <= length(hand(:,1)) & count < 4,
      if hand(i,1)==hand(i-1,1),
        count = count+1;
        if count > maxcount, maxcount = count; end;
      else
        i1 = i;  count = 1;
      end;
      i = i+1;
    end;
    if count == 4,                       %  Four of a kind.
      handrank = 3;
      if i1==1,
        lastcard = 5;
      else
        lastcard = 1;
      end;
      hand = [hand(i1:i1+3,:);hand(lastcard,:)];
      return
    end;

    if maxcount == 3,                  %  Three of a kind.

      %  Check for a full house.

      i1 = 1;  count = 1; 
      i = 2;
      while i <= length(hand(:,1)) & count < 3,
        if hand(i,1)==hand(i-1,1),
          count = count+1;
        else
          i1 = i;  count = 1;
        end;
        i = i+1;
      end;
      triple1 = hand(i1:i1+2,:);
      
      %  In 7 cards, there may be two triples.  If so, take the
      %  higher as the triple and two of the other as the pair.

      ii1 = 1;  count = 1;
      i = 2;
      while i <= length(hand(:,1)) & count < 3,
        if i < i1 | i > i1+2,
          if hand(i,1)==hand(i-1,1),
            count = count+1;
          else
            ii1 = i;  count = 1;
          end;
        end;
        i = i+1;
      end;
      if count==3,
        triple2 = hand(ii1:ii1+2,:);
        if triple1(1,1) > triple2(1,1),
          hand = [triple1; triple2(1:2,:)];
        else
          hand = [triple2; triple1(1:2,:)];
        end;
        handrank = 4;
        return
      end;
                                         % Check for a pair.
      ii1 = 1;  count = 1;
      i = 2;
      while i <= length(hand(:,1)) & count < 2,
        if i < i1 | i > i1+2,
          if hand(i,1)==hand(i-1,1),
            count = count+1;
          else
            ii1 = i;  count = 1;
          end;
        end;
        i = i+1;
      end;
      if count==2,                       % Could have two pairs.
                                         % If so, take the higher one.
        pair1 = hand(ii1:ii1+1,:);
        iii1 = 1;  kount = 1;
        i = 2;
        while i <= length(hand(:,1)),
          if (i < i1 | i > i1+2) & (i < ii1 | i > ii1+1),
            if hand(i,1)==hand(i-1,1),
              kount = kount+1;
            else
              iii1 = i;  kount = 1;
            end;
          end;
          i = i+1;
        end;
        if kount==2,
          pair2 = hand(iii1:iii1+1,:);
          if pair1(1,1) > pair2(1,1),
            hand = [triple1; pair1];
          else
            hand = [triple1; pair2];
          end;
        else                             % Only one pair.
          hand = [triple1; pair1];
        end;
        handrank = 4;
        return
      else                               % No full house; only three of a kind.
        if i1 > 1, 
          fourthcard = hand(1,:);
          if i1 > 2,
            fifthcard = hand(2,:);
          else
            fifthcard = hand(i1+3,:);
          end;
        else
          fourthcard = hand(i1+3,:);
          fifthcard = hand(i1+4,:);
        end;
        handrank = 7;
        hand = [triple1; fourthcard; fifthcard];
        return;
      end;
    end;
    if maxcount==2,                      % Two of a kind.  Check for two pair.

      i1 = 1;  count = 1; 
      i = 2;
      while i <= length(hand(:,1)) & count < 2,
        if hand(i,1)==hand(i-1,1),
          count = count+1;
        else
          i1 = i;  count = 1;
        end;
        i = i+1;
      end;
      pair1 = hand(i1:i1+1,:);
      ii1 = 1;  count = 1;
      i = 2;
      while i <= length(hand(:,1)) & count < 2,
        if i < i1 | i > i1+1,
          if hand(i,1)==hand(i-1,1),
            count = count+1;
          else
            ii1 = i;  count = 1;
          end;
        end;
        i = i+1;
      end;
      if count > 1,                      %  Two pair.  Check for a third pair
                                         %  and take the highest two.
        handrank = 8;
        pair2 = hand(ii1:ii1+1,:);
        iii1 = 1;  kount = 1;
        i = 2;
        while i <= length(hand(:,1)) & kount < 2,
          if (i < i1 | i > i1+1) & (i < ii1 | i > ii1+1),
            if hand(i,1)==hand(i-1,1),
              kount = kount+1;
            else
              iii1 = i;  kount = 1;
            end;
          end;
          i = i+1;
        end;
        if kount > 1,                     %  Three pair.
          pair3 = hand(iii1:iii1+1,:);
          if pair3(1,1) > pair2(1,1) & pair2(1,1) > pair1(1,1), 
            temphand = [pair3; pair2];
            inext = 0;  i = 1;
            while i <= length(hand(:,1)) & inext==0,
              if (i < iii1 | i > iii1+1) & (i < ii1 | i > ii1+1),
                inext = i;
              else
                i = i+1;
              end;
            end;
          end;
          if pair3(1,1) > pair1(1,1) & pair1(1,1) > pair2(1,1),  
            temphand = [pair3; pair1];
            inext = 0;  i = 1;
            while i <= length(hand(:,1)) & inext==0,
              if (i < iii1 | i > iii1+1) & (i < i1 | i > i1+1),
                inext = i;
              else
                i = i+1;
              end;
            end;
          end;
          if pair2(1,1) > pair3(1,1) & pair3(1,1) > pair1(1,1),  
            temphand = [pair2; pair3];
            inext = 0;  i = 1;
            while i <= length(hand(:,1)) & inext==0,
              if (i < ii1 | i > ii1+1) & (i < iii1 | i > iii1+1),
                inext = i;
              else
                i = i+1;
              end;
            end;
          end;
          if pair2(1,1) > pair1(1,1) & pair1(1,1) > pair3(1,1),  
            temphand = [pair2; pair1];
            inext = 0;  i = 1;
            while i <= length(hand(:,1)) & inext==0,
              if (i < ii1 | i > ii1+1) & (i < i1 | i > i1+1),
                inext = i;
              else
                i = i+1;
              end;
            end;
          end;
          if pair1(1,1) > pair3(1,1) & pair3(1,1) > pair2(1,1),  
            temphand = [pair1; pair3];
            inext = 0;  i = 1;
            while i <= length(hand(:,1)) & inext==0,
              if (i < i1 | i > i1+1) & (i < iii1 | i > iii1+1),
                inext = i;
              else
                i = i+1;
              end;
            end;
          end;
          if pair1(1,1) > pair2(1,1) & pair2(1,1) > pair3(1,1),  
            temphand = [pair1; pair2];
            inext = 0;  i = 1;
            while i <= length(hand(:,1)) & inext==0,
              if (i < i1 | i > i1+1) & (i < ii1 | i > ii1+1),
                inext = i;
              else
                i = i+1;
              end;
            end;
          end;
          hand = [temphand; hand(inext,:)];
          return
        else
          if pair1(1,1) > pair2(1,1),
            temphand = [pair1; pair2];
          else
            temphand = [pair2; pair1];
          end;
          inext = 0;  i = 1;
          while i <= length(hand(:,1)) & inext==0,
            if (i < i1 | i > i1+1) & (i < ii1 | i > ii1+1),
              inext = i;
            else
              i = i+1;
            end;
          end;
          hand = [temphand; hand(inext,:)];
          return
        end;
      else                                           %  Only one pair. 
        handrank = 9;
        temphand = hand(i1:i1+1,:);
        if i1 > 1,
          inext1 = 1;
          if i1 > 2,
            inext2 = 2;
            if i1 > 3,
              inext3 = 3;
            else
              inext3 = i1+2;
            end;
          else
            inext2 = i1+2; inext3 = i1+3;
          end;
        else
          inext1 = i1+2; inext2 = i1+3; inext3 = i1+4;
        end;
        hand = [temphand; hand(inext1,:); hand(inext2,:); hand(inext3,:)];
        return
      end;
    else                                           %  Nothing.
      handrank = 10;
      hand = hand(1:5,:);
      return
    end;
  end;
end;
