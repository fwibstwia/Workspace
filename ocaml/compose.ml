open List

let compose ((f, g):(int -> int) * (int -> int)) (x : int) : int =
  f (g x)

let rec ntimes ((f, n) : (int -> int) * int) =
  if n = 0
  then (fun (x : int) -> x)
  else compose (f, ntimes(f, n - 1))

exception Error

let sqrt (x :float) : float =
  if x < 0.0 then raise Error
  else sqrt x

let map_test (f: 'a -> 'b) (l: 'a list) : 'b list =
  fold_right (fun x r -> (f x) :: r) l []
