import Control.Monad
import Control.Monad.Writer

type  KnightPos = (Int, Int)

binSmalls :: Int -> Int -> Maybe Int
binSmalls a b = if a > 9 then Nothing
                              else Just $ a + b


logPos :: KnightPos -> Writer [KnightPos] KnightPos
logPos pos = writer (pos, [pos])



--moveKnightDir :: KnightPos -> KnightPos -> Writer [KnightPos] KnightPos
--moveKnightDir start end = Writer (end, [start]) 


-- moveKnight :: Writer [KnightPos] KnightPos -> [Writer [KnightPos] KnightPos]
-- moveKnight wpos = do
--  (c', r') <- [(c+2, r-1), (c+2, r+1), (c-2, r-1), (c-2, r+1),
--               (c+1, r-2), (c+1, r+2), (c-1, r-2), (c-1, r+2)]
--  guard (c' `elem` [1..8] && r' `elem` [1..8])
--  return (Writer (c', r') [(c,r)])

  
-- in3 :: KnightPos -> [Writer [KnightPos] KnightPos]
-- in3 (c, r) = do
--   firstMove <- moveKnight (c,r)
--   secondMove <- moveKnight firstMove
--   thirdMove <-  moveKnight secondMove
--   return (thirdMove, [(c,r), firstMove, secondMove, thirdMove])

-- canReachIn3 :: KnightPos -> KnightPos -> [[KnightPos]]
-- canReachIn3 start end = do
--   (pos, move) <- in3 start
--   guard (end == pos)
--   return move
