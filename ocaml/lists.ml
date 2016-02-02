(* polymorphic lists *)

type 'a list_ = Nil | Cons of 'a * 'a list_

(* is the list empty? *)
let is_empty (lst : 'a list_) : bool = 
  match lst with
  | Nil-> true
  | _ -> false

(* length of the list *)
let rec length (lst : 'a list_) : int = 
  match lst with
  | Nil-> 0
  | Cons (_, rest) -> 1 + length rest

(* append [a; b; c] [d; e; f] = [a; b; c; d; e; f] *)
let rec append (x : 'a list_) (y : 'a list_) : 'a list_ = 
  match x with
  | Nil-> y
  | Cons (h, t) -> Cons (h, append t y)

(* [1; 2; 3] *)
let il = Cons (1, Cons (2, Cons (3, Nil)))
let il2 = append il il
let il4 = append il2 il2
let il8 = append il4 il4
(* ["a"; "b"; "c"] *)
let sl = Cons ("a", Cons ("b", Cons ("c", Nil)))
let sl2 = append sl sl
let sl4 = append sl2 sl2

(* reverse the list:  reverse [1; 2; 3; 4] = [4; 3; 2; 1] *)
let rec reverse (x : 'a list_) : 'a list_ = 
  match x with
  | Nil-> Nil
  | Cons (h, t) -> append (reverse t) (Cons (h, Nil))

let il4r = reverse il4
let sl4r = reverse sl4

(* apply the function f to each element of x
 * map f [a; b; c] = [f a; f b; f c] *)
let rec map (f : 'a -> 'b) (x : 'a list_) : 'b list_ = 
  match x with
  | Nil-> Nil
  | Cons (h, t) -> Cons (f h, map f t)

let mil4 = map string_of_int il4

(* insert sep between each element of x: 
 * separate s [a; b; c; d] = [a; s; b; s; c; s; d] *)
let rec separate (sep : 'a) (x : 'a list_) : 'a list_ = 
  match x with
  | Nil-> Nil
  | Cons (h, Nil) -> x
  | Cons (h, t) -> Cons (h, Cons (sep, separate sep t))

let s0il4 = separate 0 il4
