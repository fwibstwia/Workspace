Require Import Fcore.
Require Import Fcalc_ops.
Require Import Reals.
Require Import Arith.
Require Import Gappa_tactic.
Require Import Omega.
Section coq_basic.

Theorem neg' : forall P Q:Prop, P->~P->Q.
Proof.
 intros P Q p H.
 induction H.
 admit.
Qed.

Theorem conj' : forall P Q:Prop, P->Q->P/\Q.
Proof.
  split.
  assumption.
  assumption.
Qed.

Theorem disj' : forall P Q:Prop, P->P\/Q.
Proof.
  left. 
  assumption.
Qed.

Inductive month : Set :=
| January | February | March     | April   | May      | June 
| July    | August   | September | October | November | December.

Check month_ind.

Record plane : Set := point {abscissa : Z; ordinate : Z}.

Check plane_ind.

Fixpoint sum(n:nat) : nat :=
 match n with
 |0 => 0
 |(S n') => n' + sum(n')
 end.

Eval compute in sum(10).
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

Open Scope Z_scope.
Theorem omega': forall x y:Z, x > 0 -> 3 * x < 2 * y -> x < y.
Proof.
  intros.
  omega.
Qed.
Close Scope Z_scope.

End coq_basic.

Section flocq_basic.
(** Some basic definitions.
Record float (beta : radix) := Float { Fnum : Z ; Fexp : Z }.

Definition F2R (f : float beta) :=
  (Z2R (Fnum f) * bpow beta (Fexp f))%R.

Definition canonic_exp x :=
  fexp (ln_beta beta x).

Definition canonic (f : float beta) :=
  Fexp f = canonic_exp (F2R f).

Definition scaled_mantissa x :=
  (x * bpow (- canonic_exp x))%R.

Definition generic_format (x : R) :=
  x = F2R (Float beta (Ztrunc (scaled_mantissa x)) (canonic_exp x)). 

Record radix := { radix_val :> Z ; radix_prop : Zle_bool 2 radix_val = true }.
Definition radix2 := (Build_radix 2 (refl_equal true)).

Definition round x :=
  F2R (Float beta (rnd (scaled_mantissa x)) (canonic_exp x)).
*)
Open Scope R_scope.

Definition rnd := rounding_float rndZR 53 (-1074).
Definition format :=
  generic_format radix2 (FLT_exp (-1074) 53).
Check rnd.
Goal
  forall a b : R,
  format a -> format b ->
  52 / 16 <= a <= 53 / 16 ->
  22 / 16 <= b <= 30 / 16 ->
  format (a - b).
Proof.
  intros a b Ha Hb Ia Ib.
  refine (sym_eq (_ : rnd (a - b) = a - b)).
  revert Ia Ib.
  replace a with (rnd a).
  replace b with (rnd b).
  unfold rnd ; gappa.
Qed.

Section float_comm.

Theorem rnd_comm: forall a b : R, format a -> format b -> rnd(a+b) = rnd (b+a).
Proof.
   intros.
   apply (f_equal (A:=R)).
   apply Rplus_comm.
Qed.

Goal forall a b c : R, format a -> format b -> format c ->
              rnd (a + (rnd (b + c))) = rnd (a + rnd (c + b)).
Proof.
   auto with *.
Qed.

Goal forall a b c d e f g: R, format a -> format b -> format c  -> format d  -> format e  -> format f  -> format g
                                              ->  rnd (a + rnd(rnd (b + c) + rnd(d + e))) = 
                                                  rnd (a + rnd(rnd (c + b) + rnd(e + d))).
Proof.
   intros.
   (* rewrite H.*)
   apply f_equal.
   apply f_equal.
   apply f_equal.
   assert (comm: rnd(b+c) = rnd(c+b)).
  
   apply rnd_comm.
   exact H0.
   exact H1.
   rewrite -> comm.
   apply f_equal.
   apply rnd_comm.
   trivial.
   trivial.
Qed.


   
(* (= (+ (+ (+ a1 a2) a3) a4) (+ (+ a1 a2) (+ a3 a4))) *)
(* Goal forall a b c d: R, format a -> format b -> format c -> format d ->
rnd(rnd(rnd(a + b)+c)+d) = rnd(rnd(a+b) + rnd(c+d)). *)
End float_comm.
End flocq_basic.