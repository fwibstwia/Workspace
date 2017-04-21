{-# OPTIONS -W -fglasgow-exts #-}
module WordNumbers1 where

import Prelude hiding ((+), (*), sum, product, Monoid, (*>), (<*))
import qualified Prelude as P
import Data.List (genericSplitAt, genericLength)

infixl 6 +
infixl 7 *

class Monoid a where
  zero:: a
  (+) :: a -> a -> a

instance Monoid [a] where
  zero = []
  (+) = (++)

class (Monoid a) => Seminearring a where
  one :: a
  (*) :: a -> a -> a

instance Seminearring [[a]] where
  one = [[]]
  xss * yss = [ xs ++ ys | xs <- xss, ys <- yss]

class Character a where
  char :: Char -> a

instance Character Char where
  char = id

instance (Character a) => Character [a] where
  char c = [char c]

product :: (Seminearring a) => [a] -> a
product = foldr (*) one

string :: (Seminearring a, Character a) => String -> a
string = product . map char

sum :: (Monoid a) => [a] -> a
sum = foldr (+) zero

strings :: (Seminearring a, Character a) => String -> a
strings = sum . map string . words

ten1, ten2, ten3, ten6, ten9 :: (Seminearring a, Character a) => a
ten1 = strings "one two three four five six seven eight nine"
ten2 = ten1
     + strings "ten eleven twelve"
     + (strings "thir four" + prefixes) * string "teen"
     + (strings "twen thir for" + prefixes) * string "ty" * (one + ten1)
    where prefixes = strings "fif six seven eigh nine"
ten3 = ten2 + ten1 * string "hundred" * (one + ten2)
ten6 = ten3 + ten3 * string "thousand" * (one + ten3)
ten9 = ten6 + ten3 * string "million" * (one + ten6)

newtype Nat a = Nat a deriving (Eq, Ord)

type Count = Nat Integer

instance (Show a) => Show (Nat a) where
  show (Nat x) = show x

instance (Num a) => Monoid (Nat a) where
  zero = Nat 0
  Nat a + Nat b = Nat (a P.+ b)

instance (Num a) => Seminearring (Nat a) where
  one = Nat 1
  Nat a * Nat b = Nat (a P.* b)

instance (Num a) => Character (Nat a) where
  char _ = one

class (Seminearring r, Monoid m) => Module r m where
  (*>) :: r -> m -> m
  (*>) = flip (<*)
  (<*) :: m -> r -> m
  (<*) = flip (*>)

data Deriv r m = Deriv r m deriving (Eq, Show)

instance (Monoid r, Monoid m) => Monoid (Deriv r m) where
  zero = Deriv zero zero
  Deriv c1 m1 + Deriv c2 m2 = Deriv (c1 + c2) (m1 + m2)

instance (Module r m) => Seminearring (Deriv r m) where
  one = Deriv one zero
  Deriv c1 m1 * Deriv c2 m2 = Deriv (c1 * c2) (c1 *> m2 + m1 <* c2)

instance (Character r, Character m) => Character (Deriv r m) where
  char c = Deriv (char c) (char c)

newtype Wrap s a = Wrap a deriving (Eq, Ord)

instance (Monoid a) => Monoid (Wrap s a) where
  zero = Wrap zero
  Wrap a + Wrap b = Wrap (a + b)

instance (Show a) => Show (Wrap s a) where
  show (Wrap x) = show x

instance (Seminearring a) => Seminearring (Wrap s a) where
  one = Wrap one
  Wrap a * Wrap b = Wrap (a * b)

instance (Seminearring r) => Module r (Wrap s r) where
  r *> Wrap m = Wrap (r * m)
  Wrap m <* r = Wrap (m * r)

data V
type Volume = Wrap V (Nat Integer)

instance Character Volume where char _ = one

data Binary m = Binary m (Maybe (Binary m, Binary m))

instance (Character m) => Character (Binary m) where
  char c = Binary (char c) Nothing

instance (Monoid m) => Monoid (Binary m) where
  zero = Binary zero Nothing
  b1@(Binary m1 _) + b2@(Binary m2 _) = Binary (m1 + m2) (Just (b1, b2))

instance (Seminearring m) => Seminearring (Binary m) where
  one = Binary one Nothing
  b1@(Binary m1 c1) * b2@(Binary m2 c2) =
    Binary (m1 * m2) (case c1 of
                        Just (b11, b12) -> Just (b11 * b2, b12 * b2)
                        Nothing -> case c2 of
                                     Nothing -> Nothing
                                     Just (b21, b22) -> Just (b1 * b21, b1 * b22))

type Measure = ([String], Deriv Count Volume)

instance (Monoid a,  Monoid b) => Monoid (a, b) where
  zero = (zero, zero)
  (a1, b1) + (a2, b2) = (a1 + a2, b1 + b2)

instance (Seminearring a, Seminearring b) => Seminearring (a, b) where
  one = (one, one)
  (a1, b1) * (a2, b2) = (a1*a2, b1*b2)

instance (Character r1, Character r2) => Character (r1, r2) where
  char c = (char c, char c)

volume :: Deriv Count Volume -> Integer
volume (Deriv _ (Wrap (Nat v))) = v

search :: Binary Measure -> Integer -> Measure
search (Binary m Nothing) _ = m
search (Binary _ (Just (c1@(Binary (_, skip) _), c2))) i
  | i' < 0 = search c1 i
  |otherwise = let (s, m) = search c2 i' in (s, skip + m)
  where i' = i - volume skip

answer n = (before, it, after)
  where
    target = pred n
    ([string], d) = search ten9 target
    end = volume d
    (before, it:after) = genericSplitAt local string
    local = genericLength string - (end - target)
