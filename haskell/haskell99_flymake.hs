{-# LANGUAGE TemplateHaskell #-}
-- |
import Control.Monad
import Control.Applicative

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
