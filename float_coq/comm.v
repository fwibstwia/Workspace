Require Import List.
Require Import Arith.

Inductive tree : Set :=
  | Leaf : nat -> tree
  | Node : tree -> tree -> list nat -> tree.

Notation t := tree.

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

Fixpoint tree_nat (tr : t) : nat :=
 match tr with
 | Node t1 t2 _ => tree_nat t1 + tree_nat
 | Leaf n => n
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

Theorem comm_aux_valid : forall tr:t, tree_nat (sort_tree tr) = tree_nat tr.
Proof.
 intro.
 induction tr.
 auto.
 unfold sort_tree.
 fold sort_tree.
 destruct (pre_list_compare (pre_list (sort_tree tr1))).
 simpl.
 auto.
 simpl.
 rewrite IHtr1.
 rewrite IHtr2.
 apply plus_comm.
Qed.

Theorem comm_valid : 
  forall tr tr':t, tree_nat (sort_tree tr) = tree_nat (sort_tree tr') ->
              tree_nat tr = tree_nat tr'.
Proof.
  intros.
  induction tr.
  assert (H1: Leaf n = sort_tree (Leaf n)).
  simpl.
  reflexivity.
  induction tr'.
  assert (H2: Leaf n0 = sort_tree (Leaf n0)).
  auto.
  rewrite H1.
  rewrite H2.
  exact H.
  assert (tree_nat (sort_tree (Node tr'1 tr'2 l)) = tree_nat (Node tr'1 tr'2 l)).
  apply comm_aux_valid.  
  rewrite <- H0.
  rewrite H1.
  exact H.
  assert (tree_nat (sort_tree (Node tr1 tr2 l)) = tree_nat (Node tr1 tr2 l)).
  apply comm_aux_valid.
  assert (tree_nat (sort_tree tr') = tree_nat tr').
  apply comm_aux_valid.
  rewrite <- H0.
  rewrite <- H1.
  exact H.
Qed.

Theorem comm_test' :
 (1 + 2) + (3 + 4) = (2 + 1) + (4 + 3).
Proof.
 change
  (tree_nat (Node (Node (Leaf 1) (Leaf 2) nil) (Node (Leaf 3) (Leaf 4) nil) nil) = 
  tree_nat (Node (Node (Leaf 2) (Leaf 1) nil) (Node (Leaf 4) (Leaf 3) nil) nil)).
  apply comm_valid; auto.
Qed.

Theorem comm_test'' :
 (1 + 2) + (3 + 4) = (4 + 3) + (2 + 1).
Proof.
 change
 (tree_nat (Node (Node (Leaf 1) (Leaf 2) nil) (Node (Leaf 3) (Leaf 4) nil) nil) = 
  tree_nat (Node (Node (Leaf 4) (Leaf 3) nil) (Node (Leaf 2) (Leaf 1) nil) nil)).
 apply comm_valid; auto.
Qed.

Theorem comm_test''' :
 (4 + (5 + 6)) + 7 = 7 + (4 + (5 + 6)).
Proof.
 change 
  (tree_nat (Node (Node (Leaf 4) (Node (Leaf 5) (Leaf 6) nil) nil) (Leaf 7) nil) =
   tree_nat (Node (Leaf 7) (Node (Leaf 4) (Node (Leaf 5) (Leaf 6) nil) nil) nil)).
 apply comm_valid; auto.
Qed.

