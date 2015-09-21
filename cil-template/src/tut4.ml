open Cil
open Pretty
open Tututil
open Ppl_ocaml
open Gmp

module L  = List
module E  = Errormsg

module IH = Inthash
module DF = Dataflow

module F = Frontc
module C = Cil

module O = Ciltutoptions


let main () =

  C.print_CIL_Input := true;


  C.insertImplicitCasts := false;


  C.lineLength := 100000;


  C.warnTruncate := false;


  E.colorFlag := true;


  Cabs2cil.doCollapseCallCast := true;

  E.error "No file names provided";
  
;;


begin
  try
    main ()
  with
  | F.CabsOnly -> ()
  | E.Error -> ()
end;
exit (if !E.hadErrors then 1 else 0)
