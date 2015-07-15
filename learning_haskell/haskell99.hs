{-# LANGUAGE TemplateHaskell #-}
-- |
import Control.Monad
import Control.Applicative
import Data.List

myButLast :: [a] -> a
myButLast  = head . tail . reverse

elementat :: [a] -> Int -> a
elementat  = (!!)

myLength :: [a] -> Int
myLength = foldr (\_ c -> c + 1) 0

myReverse :: [a] -> [a]
myReverse = foldl (\xs x -> x:xs) []

isPalindrome :: (Eq a) => [a] -> Bool
isPalindrome xs  = Control.Monad.liftM2 (==) id reverse xs

data NestedList a = Elem a | List [NestedList a]

flatten :: NestedList a -> [a]
flatten (Elem x) = [x]
flatten (List nl) = foldr (\nl l -> flatten nl ++ l) [] nl

compress :: (Eq a) => [a] -> [a]
compress  xs = foldr (\x  l -> if x == head l
                                 then l
                                 else x:l) [last xs] xs

pack :: (Eq a) => [a] -> [[a]]
pack xs = foldr (\x l-> if x == head (head l)
                           then (x:head l):tail l
                           else [x]:l) [[last xs]] (init xs)

encode :: (Eq a) => [a] -> [(Int, a)]
encode = map (\x -> (length x, head x)) . pack

data MSEncode a = Single a | Multiple Int a deriving Show

encodeModified :: (Eq a) => [a] -> [MSEncode a]
encodeModified = map (\x -> if length x <= 1
                               then Single (head x)
                               else Multiple (length x) (head x)) . pack

decodeModified :: (Eq a) => [MSEncode a] -> [a]
decodeModified = concatMap deCode where
                 deCode (Single x1) = [x1]
                 deCode (Multiple i x1) = replicate i x1
insertAt :: a -> [a] -> Int -> [a]
insertAt x xs n = (take (n - 1) xs) ++ (x:(drop (n-1) xs))

isPrime :: Int -> Bool
isPrime n = all id $ map (\x -> (not $ n `mod` x == 0)) [2..n-1]

myGCD :: Int -> Int -> Int
myGCD m n
  |n == 0 = m
  |m < n = myGCD n m
  |otherwise = myGCD n (m `mod` n)

table :: (Bool -> Bool -> Bool) -> [(Bool, Bool, Bool)]
table f = [(x, y, f x y) |x <- [True, False], y <- [True, False]]

huffman :: [(Char, Integer)] -> [(Char, String)]
huffman xs  =  sortBy sortfC $ (fst . head . huffmanHelp . transHuff) xs where
        sortfC (c1, _) (c2, _) = compare c1 c2

transHuff :: [(Char, Integer)] -> [([(Char, String)], Integer)]
transHuff = map (\(x, n) -> ([(x, "")], n))

huffmanHelp :: [([(Char, String)], Integer)] -> [([(Char, String)], Integer)]
huffmanHelp [x] = [x]
huffmanHelp xs =  huffmanHelp . mergeHuff . (sortBy sortfI) $ xs  where
               sortfI (_, m) (_, n) = compare m n

mergeHuff :: [([(Char, String)], Integer)] -> [([(Char, String)], Integer)]
mergeHuff xs = (mergeHelp (xs!!0) (xs!!1)):drop 2 xs where
          mergeHelp (x, n) (y,m) = ((appendHuff x '0') ++ (appendHuff y '1'), n + m)

appendHuff :: [(Char, String)] -> Char -> [(Char, String)]
appendHuff xs code = map (\(c, s) -> (c, code:s)) xs
