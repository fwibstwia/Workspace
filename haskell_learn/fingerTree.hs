--{-# LANGUAGE TemplateHaskell #-}
-- b:set -XOverloadedLists -XTypeFamilies -- 
import GHC.Exts (IsList(..))

data Node a = Branch3 a a a
            | Branch2 a a
            deriving Show

instance IsList (Node a) where
  type Item (Node a) = a

  toList (Branch3 x y z) = [x, y, z]
  toList (Branch2 x y) = [x, y]

  fromList [x, y, z] = Branch3 x y z
  fromList [x, y] = Branch2 x y
  fromList _ = error "Node must contain two or three elements"

data Affix a = One a
             | Two a a
             | Three a a a
             | Four a a a a
             deriving Show

instance IsList (Affix a) where
  type Item (Affix a) = a

  toList (One x) = [x]
  toList (Two x y) = [x, y]
  toList (Three x y z) = [x, y, z]
  toList (Four x y z w) = [x, y, z, w]

  fromList [x] = One x
  fromList [x, y] = Two x y
  fromList [x, y, z] = Three x y z
  fromList [x, y, z, w] = Four x y z w
  fromList _ = error "Affix must have one to four elements"

affixPrepend :: a -> Affix a -> Affix a
affixPrepend x = fromList . (x :) . toList

affixAppend :: a -> Affix a -> Affix a
affixAppend x = fromList . (++ [x]) . toList

data FingerTree a
  = Empty
  | Single a
  | Deep {
      prefix :: Affix a,
      deeper :: FingerTree (Node a),
      suffix :: Affix a
      }
  deriving Show

layer3 :: FingerTree a
layer3 = Empty

layer2 :: FingerTree (Node Char)
layer2 = Deep prefix layer3 suffix
  where
    prefix = [Branch2 'i' 's', Branch2 'i' 's']
    suffix = [Branch3 'n' 'o' 't', Branch2 'a' 't']

layer1 :: FingerTree Char
layer1 = Deep prefix layer2 suffix
  where
    prefix = ['t', 'h']
    suffix = ['r', 'e', 'e']

exampleTree::FingerTree Char
exampleTree = layer1

infixr 5 <|
(<|)::a -> FingerTree a -> FingerTree a

x <| Empty = Single x

x <| Single y = Deep [x] Empty [y]

x <| Deep [a, b, c, d] deeper suffix = Deep [x, a] (node <| deeper) suffix
  where
    node = Branch3 b c d

x <| tree = tree{ prefix = affixPrepend x $ prefix tree}

infixl 5 |>

(|>) :: FingerTree a -> a -> FingerTree a

Empty |> x = Single x

Single y |> x = Deep [y] Empty [x]

Deep prefix deeper [a, b, c, d] |> x = Deep prefix (deeper |> node) [d, x]
  where
    node = Branch3 a b c

tree |> x = tree {suffix = affixAppend x $ suffix tree}

empty :: FingerTree a
empty = Empty
