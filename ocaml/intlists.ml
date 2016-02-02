(* An intlist is either Nil or Cons of an int and a (shorter) intlist *)
type intlist = Nil | Cons of int * intlist

(* Returns the length of lst *)
let rec length (lst : intlist) : int =
  match lst with
  | Nil -> 0
  | Cons (h, t) -> length t + 1

(* is the list empty? *)
let is_empty (lst : intlist) : bool =
  match lst with
  | Nil -> true
  | Cons _ -> false

(* Notice that the match expressions for lists all have the same
 * form -- a case for the empty list (Nil) and a case for a Cons.
 * Also notice that for most functions, the Cons case involves a
 * recursive function call. *)

(* Return the sum of the elements in the list *)
let rec sum (lst : intlist) : int =
  match lst with
  | Nil -> 0
  | Cons (i, t) -> i + sum t

(* Create a string representation of a list *)
let rec to_string (lst : intlist) : string =
  match lst with
  | Nil -> ""
  | Cons (i, Nil) -> string_of_int i
  | Cons (i, Cons (j, t)) ->
      string_of_int i ^ "," ^ to_string (Cons (j, t))

(* Return the head (first element) of the list *)
let head (lst : intlist) : int =
  match lst with
  | Nil -> failwith "empty list"
  | Cons (i, t) -> i

(* Return the tail (rest of the list after the head) *)
let tail (lst : intlist) : intlist =
  match lst with
  | Nil -> failwith "empty list"
  | Cons (i, t) -> t

(* Return the last element of the list (if any) *)
let rec last (lst : intlist) : int =
  match lst with
  | Nil -> failwith "empty list"
  | Cons (i, Nil) -> i
  | Cons (i, t) -> last t

(* Return the nth element of the list (starting from 0) *)
let rec nth (lst : intlist) (n : int) : int =
  match lst with
  | Nil -> failwith "index out of bounds"
  | Cons (i, t) ->
      if n = 0 then i
      else nth t (n - 1)

(* Append two lists:  append [1; 2; 3] [4; 5; 6] = [1; 2; 3; 4; 5; 6] *)
let rec append (l1 : intlist) (l2 : intlist) : intlist =
  match l1 with
  | Nil -> l2
  | Cons (i, t) -> Cons (i, append t l2)

(* Reverse a list:  reverse [1; 2; 3] = [3; 2; 1].
 * First reverse the tail of the list 
 * (e.g., compute reverse [2; 3] = [3; 2]), then
 * append the singleton list [1] to the end to yield [3; 2; 1].
 * This is not the most efficient method. *)
let rec reverse (lst : intlist) : intlist =
  match lst with
  | Nil -> Nil
  | Cons (h, t) -> append (reverse t) (Cons (h , Nil))

(******************************
 * Examples
 ******************************)

(* Here is a way to perform a function on each element
 * of a list.  We apply the function recursively.
 *)

let inc (x : int) : int = x + 1
let square (x : int) : int = x * x

(* Given [i1; i2; ...; in], return [i1+1; i2+1; ...; in+n] *)
let rec addone_to_all (lst : intlist) : intlist = 
  match lst with
  | Nil -> Nil
  | Cons (h, t) -> Cons (inc h, addone_to_all t)

(* Given [i1; i2; ...; in], return [i1*i1; i2*i2; ...; in*in] *)
let rec square_all (lst : intlist) : intlist = 
  match lst with
  | Nil -> Nil
  | Cons (h, t) -> Cons (square h, square_all t)

(* Here is a more general method. *)

(* Given a function f and [i1; ...; in], return [f i1; ...; f in].
 * Notice how we factored out the common parts of addone_to_all
 * and square_all. *)
let rec do_function_to_all (f : int -> int) (lst : intlist) : intlist =
  match lst with
  | Nil -> Nil
  | Cons (h, t) -> Cons (f h, do_function_to_all f t)

let addone_to_all (lst : intlist) : intlist =
  do_function_to_all inc lst

let square_all (lst : intlist) : intlist =
  do_function_to_all square lst

(* Even better: use anonymous functions. *)

let addone_to_all (lst : intlist) : intlist =
  do_function_to_all (fun x -> x + 1) lst

let square_all (lst : intlist) : intlist =
  do_function_to_all (fun x -> x * x) lst

(* Equivalently, we can partially evaluate by applying 
 * do_function_to_all just to the first argument. *)

let addone_to_all : intlist -> intlist =
  do_function_to_all (fun x -> x + 1)

let square_all : intlist -> intlist =
  do_function_to_all (fun x -> x * x)

(* Say we want to compute the sum and product of integers
 * in a list. *)

(* Explicit versions *)
let rec sum (lst : intlist) : int =
  match lst with
  | Nil -> 0
  | Cons (i, t) -> i + sum t

let rec product (lst : intlist) : int =
  match lst with
  | Nil -> 1
  | Cons (h, t) -> h * product t

(* Better: use a general function collapse that takes an
 * operation and an identity element for that operation.
 *)

(* Given f, b, and [i1; i2; ...; in], return f(i1, f(i2, ..., f (in, b))).
 * Again, we factored out the common parts of sum and product. *)
let rec collapse (f : int -> int -> int) (b : int) (lst : intlist) : int =
  match lst with
  | Nil -> b
  | Cons (h, t) -> f h (collapse f b t)

(* Now we can define sum and product in terms of collapse *)
let sum (lst : intlist) : int =
  let add (i1 : int) (i2 : int) : int = i1 + i2 in
  collapse add 0 lst

let product (lst : intlist) : int =
  let mul (i1 : int) (i2 : int) : int = i1 * i2 in
  collapse mul 1 lst

(* Here, we use anonymous functions instead of defining add and mul.
 * After all, what's the point of giving those functions names if all
 * we're going to do is pass them to collapse? *)
let sum (lst : intlist) : int =
  collapse (fun i1 i2 -> i1 + i2) 0 lst

let product (lst : intlist) : int =
  collapse (fun i1 i2 -> i1 * i2) 1 lst

(* Trees of integers *)

type inttree = Empty | Node of node
and node = { value : int; left : inttree; right : inttree }

(* Return true if the tree contains x. *)
let rec search (t : inttree) (x : int) : bool =
  match t with
  | Empty -> false
  | Node {value=v; left=l; right=r} ->
	v = x || search l x || search r x

let tree1 =
  Node {value=2; left=Node {value=1; left=Empty; right=Empty};
                 right=Node {value=3; left=Empty; right=Empty}}

let z = search tree1 3
