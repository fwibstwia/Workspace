muller :: Float -> Float -> Float
muller y z = 108 - (815 - 1500/z)/y

ce :: Float -> Float -> Int -> Float
ce x y 0 = x
ce x y 1 = y
ce x y n = ce y (muller y x) m
           where m = n - 1
  

