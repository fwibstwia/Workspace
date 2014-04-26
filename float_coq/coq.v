Require Import 
Require Import Arith.

Fixpoint sum(n:nat) : nat :=
 match n with
 |0 => 0
 |(S n') => n' + sum(n')
 end.

Theorem sum_n: forall n, 2 * sum n + n = n * n.
Proof. 
  induction n.
  reflexivity.
  assert (SnSn : S n * S n = n * n + 2 * n + 1).
  ring. 
  rewrite SnSn.
  rewrite <- IHn.
  simpl.
  ring.
Qed.


