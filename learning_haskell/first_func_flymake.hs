doubleMe :: Int -> Int
doubleMe x = x + x
boomBang :: [Int] -> [[Char]]
boomBang xs = [if x < 10 then "BOOM" else "Bang" | x<-xs, odd x]

removeNonUppercase :: [Char] -> [Char]
removeNonUppercase xs = [ x | x<-xs, x `elem` ['A'..'Z']]

length' :: (Num b) => [a] -> b
length' [] = 0
length' (_:xs) = 1 + length' xs

sum' :: (Num a) => [a] -> a 
sum' [] = 0
sum' (x:xs) = x + sum' xs

capital :: String -> String
capital [] = "empty string"
capital all_s@(x:_) = "the first letter of " ++ all_s ++ " is " ++ [x]

bmiTell :: (RealFloat a) => a -> a -> String  
bmiTell weight height  
    | bmi <= 18.5 = "You're underweight, you emo, you!"  
    | bmi <= 25.0 = "You're supposedly normal. Pffft, I bet you're ugly!"  
    | bmi <= 30.0 = "You're fat! Lose some weight, fatty!"  
    | otherwise   = "You're a whale, congratulations!"  
  where bmi = weight / height ^ 2
        

max' :: (Ord a) => a -> a -> a
max' a b
  |a <= b = b
  |otherwise = a

maximum' :: (Ord a) => [a] -> a
maximum' [] = error "maximum of empty list"
maximum' [x] = x
maximum' (h:xs) 
  |h > maxTail = h
  |otherwise = maxTail
  where maxTail = maximum' xs

largestDivisible :: (Integral a) => a
largestDivisible = head (filter p [100000,99999..])
                   where p x = x `mod` 3829 == 0

collatz_chain :: (Integral a) => a -> [a]
collatz_chain 1 = [1]
collatz_chain n 
  | even n = n:collatz_chain (n `div` 2)
  | odd n = n:collatz_chain (n*3 + 1)

myLast :: [a] -> a
myLast = foldl1 (\acc x -> x)

