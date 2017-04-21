import Data.List
data NestedList a = Elem a | List [NestedList a]

flatten :: NestedList a -> [a]
flatten (Elem a) = [a]
flatten (List k) = concatMap flatten k

compress :: (Eq a) => [a] -> [a]
compress l = foldr duplicateP [(last l)] l where
  duplicateP x acc = if (x == head acc) then acc
    else x:acc
encode :: [Char] -> [(Int, Char)]
encode l = [(length x, head x) | x <- group l]

data Encoding a b = Multiple a b | Single b deriving Show
encodeModified :: (Eq a) => [a] -> [Encoding Int a]
encodeModified l = map encodef (group l) where
  encodef xl = if (length xl == 1) then Single (head xl)
               else Multiple (length xl) (head xl)

dupli :: [a] -> [a]
dupli = concat . map (\x -> [x,x])

insertAt :: a -> [a] -> Int -> [a]
insertAt c s i = let (x,y) = splitAt (i-1) s in
                   x ++ [c] ++ y

rangeInt :: Int -> Int -> [Int]
rangeInt l u = drop (l-1) $ take u $ iterate (+1) 1

suffixes :: [a] -> [[a]]
suffixes [] = [[]]
suffixes (x:xs) = (x:head s):s where
  s = suffixes xs

coprime :: Int -> Int -> Bool
coprime a b= 1 == (gcd a b)

totient :: Int -> Int
totient 1 = 1
totient a = length $ filter (== True) $ map (coprime a) [1..(a-1)]

table :: Int -> ([Bool] -> Bool) -> [[Bool]]
table c f = do a <- foldl (\acc e -> [a ++ [b] | a <- acc, b <- e]) [[True], [False]]
                    (replicate (c-1) [True, False]);
               return (a ++ [f a]);

data Tree a = Empty | Branch a (Tree a) (Tree a)
            deriving (Show, Eq)

leaf x = Branch x Empty Empty

cbalTree :: Int -> [Tree Char]
cbalTree 0 = [Empty]
cbalTree 1 = [leaf 'x']
cbalTree c = if (c-1) `mod` 2 == 0 then
               let t = cbalTree ((c-1) `div` 2) in
                 [Branch 'x' a b | a <- t, b <-t]
             else
               let t1 = cbalTree ((c-1) `div` 2)
                   t2 = cbalTree ((c-1) - (c-1) `div` 2)
               in
                 [Branch 'x' a b | a <- t1, b <-t2] ++ [Branch 'x' b a | a <-t1, b<-t2]
                 
mirror :: Tree a -> Tree a -> Bool
mirror Empty Empty = True
mirror (Branch _ t1 t2) (Branch _ t3 t4) = foldl (&&) True [mirror t1 t4, mirror t2 t3]
mirror _ _ = False

symmetric :: Tree a -> Bool
symmetric Empty = True
symmetric (Branch _ t1 t2) = mirror t1 t2

insertTree :: Tree Int -> Int -> Tree Int
insertTree Empty c = Branch c Empty Empty
insertTree (Branch e t1 t2) c = if c < e then
                                  Branch e (insertTree t1 c) t2
                                else
                                  Branch e t1 (insertTree t2 c)

construct :: [Int] -> Tree Int
construct l = foldl insertTree Empty l


symCbalTrees :: Int -> [Tree Char]
symCbalTrees = (filter symmetric) . cbalTree
