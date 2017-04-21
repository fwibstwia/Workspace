chainCPS :: ((a -> r) -> r) -> (a -> ((b -> r) -> r)) -> ((b -> r) -> r)
chainCPS s c = \k -> s $ \a -> c a $ k
