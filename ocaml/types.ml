(* polymorphism *)

let swapInt((x: int), (y: int)): int*int = (y,x)
and swapFloat((x: float), (y: float)): float*float = (y,x)
and swapString((x: string), (y: string)): string*string = (y,x)

let swapIntFloat((x: int), (y: float)): float*int = (y,x)
and swapFloatInt((x: float), (y: int)): int*float = (y,x)

let swap ((x: 'a), (y: 'b)): 'b * 'a = (y,x)

let swap (x,y) = (y, x)

(* defining alpha lists, list_ with constructor Cons_ *)

type 'a list_ = Nil_ | Cons_ of ('a * 'a list_)

let il : int list_ = Cons_(1,Cons_(2,Cons_(3,Nil_)))    (* [1,2,3] *)

let fl : float list_ = Cons_(3.14,Cons_(2.17,Nil_))     (* [3.14,2.17] *)

let sl : string list_ = Cons_("foo",Cons_("bar",Nil_)) (* ["foo","bar"] *)

let sil : (string*int) list_ =
    Cons_(("foo",1),Cons_(("bar",2),Nil_))  (* [("foo",1), ("bar",2)] *)

let isEmpty(lst: 'a list_): bool =
  match lst with
      Nil_ -> true
    | _ -> false

(* return the length of the list *)
let rec length(lst: 'a list_): int =
  match lst with
      Nil_ -> 0
    | Cons_(_, t) -> length(t)+1

(* append two lists:  append([a,b,c],[d,e,f]) = [a,b,c,d,e,f] *)
let rec append((x: 'a list_), (y: 'a list_)): 'a list_ =
  match x with
      Nil_ -> y
    | Cons_(h,t) -> Cons_(h, append(t, y))

let il2 = append(il,il)
let il3 = append(il2,il2)
let il4 = append(il3,il3)

let sl2 = append(sl,sl)
let sl3 = append(sl2,sl2)

(* reverse the list:  reverse([a,b,c,d]) = [d,c,b,a] *)
let rec reverse(x: 'a list_): 'a list_ =
  match x with
      Nil_ -> Nil_
    | Cons_(h,t) -> append(reverse(t), Cons_(h,Nil_))

let il5 = reverse(il4)
let sl4 = reverse(sl3)

(* apply the function f to each element, return new list:
 *    map f [a,b,c] = [f(a),f(b),f(c)] *)
let rec map (f: 'a->'b) (x: 'a list_): 'b list_ =
  match x with
      Nil_ -> Nil_
    | Cons_(h,t) -> Cons_(f(h), map f t)

let sl2 = map string_of_int il2

(* apply the function f to each element and accumulator, return accumulator
 *    reduce f [a,b,c] r = f a (f b (f c r))
 * also called fold_right *)
let rec reduce (f:'a -> 'b -> 'b) (lst: 'a list_) (b:'b): 'b =
    match lst with
      Nil_ -> b
    | Cons_(hd,tl) -> f hd (reduce f tl b)

let sm = reduce (fun x y -> x+y) il5 0

let st = reduce (fun x y  -> string_of_int(x) ^ " " ^ y) il ""

(* insert sep between each element of x:
 *    separate(s,[a,b,c,d]) = [a,s,b,s,c,s,d] *)
let rec separate((sep: 'a), (x: 'a list_)) =
  match x with
      Nil_ -> Nil_
    | Cons_(h,Nil_) -> x
    | Cons_(h,t) -> Cons_(h, Cons_(sep, separate(sep,t)))


(* alpha tree *)

type 'a tree = Leaf | Node of ('a tree) * 'a * ('a tree)

type 'a tree = Leaf | Node of ('a node)
      and 'a node = {left: 'a tree; value: 'a; right: 'a tree}
