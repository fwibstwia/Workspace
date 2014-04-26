Require Import List.
Require Import Arith.
Require Import ZArith.


Inductive tree : Set :=
  | Leaf : nat -> tree
  | Node : tree -> tree -> list nat -> tree.
Notation t := tree.

Section comm_eq.

Variables (A : Set) (f : A -> A -> A).

Hypothesis comm : forall x y : A, f x y = f y x.


Fixpoint tree_A (l:list A)(def:A)(tr:t) : A :=
 match tr with
 | Node t1 t2 _ => f (tree_A l def t1) (tree_A l def t2)
 | Leaf n => nth (n - 1) l def
 end.

Definition pre_list (m : t) : list nat :=
 match m with
 | Leaf n => n::nil
 | Node _ _  pre_list => pre_list
 end.

Fixpoint pre_list_compare (ell elr:list nat) : bool :=
 match ell, elr with
 | a::l', b::r' => match (nat_compare a b) with 
                   | Lt => true
                   | Gt => false
                   | Eq => (pre_list_compare l' r')
                   end
 | nil, _ => true
 | _, nil => false
 end. 

Fixpoint sort_tree (tr : t) : t :=
 match tr with
 | Node t1 t2 _ => let lt := sort_tree t1 in
                   let rt := sort_tree t2 in
                   if pre_list_compare (pre_list lt) (pre_list rt) then
                      Node lt rt (0::nil ++ (pre_list lt) ++ (pre_list rt))
                   else 
                      Node rt lt (0::nil ++ (pre_list rt) ++ (pre_list lt))
 | Leaf n => Leaf n
 end.

Theorem comm_aux_valid : forall (l:list A) (def:A) (tr:t), tree_A l def (sort_tree tr) = tree_A l def tr.
Proof.
 intros.
 induction tr.
 auto.
 unfold sort_tree.
 fold sort_tree.
 destruct (pre_list_compare (pre_list (sort_tree tr1))).
 simpl.
 rewrite IHtr1.
 rewrite IHtr2.
 reflexivity.
 simpl.
 rewrite IHtr1.
 rewrite IHtr2.
 apply comm.
Qed.

Theorem comm_valid : 
  forall (l : list A) (def: A) (tr tr':t), tree_A l def (sort_tree tr) = tree_A l def (sort_tree tr') ->
              tree_A l def tr = tree_A l def tr'.
Proof.
  intros.
  rewrite <- (comm_aux_valid l def tr). 
  rewrite <- (comm_aux_valid l def tr').
  trivial.
Qed.

End comm_eq.

Ltac term_list f l v :=
  match v with
  | (f ?X1 ?X2) =>
    let l1 := term_list f l X2 in term_list f l1 X1
  | ?X1 => constr:(cons X1 l)
  end.

Ltac compute_rank l n v :=
  match l with
  | (cons ?X1 ?X2) =>
    let tl := constr:X2 in
    match constr:(X1 = v) with
    | (?X1 = ?X1) => n
    | _ => compute_rank tl (S n) v
    end
  end.

Ltac model_aux l f v :=
 match v with
 | (f ?X1 ?X2) =>
   let r1 := model_aux l f X1 with r2 := model_aux l f X2 in 
       constr:(Node r1 r2 nil)
 | ?X1 => let n := compute_rank l 1 X1 in constr:(Leaf n) 
 | _ => constr:(Leaf 1)
 end.

Ltac comm_eq A f comm_thm :=
  match goal with
  | [ |- (?X1 = ?X2 :>A) ] =>
    let l := term_list f (nil (A:=A)) X1 in
    let term1 := model_aux l f X1 
    with term2 := model_aux l f X2 in
    (change (tree_A A f l X1 term1 = tree_A A f l X1 term2);
     apply comm_valid with (1 := comm_thm); auto)
  end.
Require Import Reals.
Require Import Fourier.
Require Import Psatz.
Open Scope R_scope.
Theorem ring' :
forall x y z:R, (x + y)*z + (y + x) = x*z + y*z + x + y.
Proof.
 intros.
 ring.
Qed.

Theorem field' :
forall x y z:R, x <> 0 -> (y + z)/x + y + x= y/x + z/x + x + y. 
 intros.
 field.
 exact H.
Qed.

Theorem fourier':
forall x y:R, x < y -> y + 1 >= x - 1.
Proof.
 intros.
 fourier.
Qed.

Theorem lra' :
forall x y:R, y > 0 -> x + y >  10*x/10.
Proof.
 intros.
 fourier.
Qed.

Theorem psatz' :
forall x y:R, x > 0 -> y > 0 -> x^2 + y^2 <= (x + y)^2. 
Proof.
 intros.
 psatz R 2.
Qed.

Theorem r':
forall x100 y z t : R, (x100 + y) * z = x100 * z + y*z.
Proof.
  intros.
  ring.
Theorem comm_test' :
 forall x y z t : Z, ((x + y) + (z + t) = (y + x) + (t + z))%Z.
Proof.
  intros.
  comm_eq Z Zplus Zplus_comm.
Qed.

Theorem comm_test'' :
 forall x y z t: Z, ((x + y) + (z + t) = (z + t) + (y + x))%Z.
Proof.
 intros.
 comm_eq Z Zplus Zplus_comm.
Qed.

Theorem comm_test''' :
 forall x y z t: Z, ((x + (y + z)) + t = t + (x + (y + z)))%Z.
Proof.
 intros.
 comm_eq Z Zplus Zplus_comm.
Qed.
