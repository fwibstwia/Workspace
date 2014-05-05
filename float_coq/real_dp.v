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
