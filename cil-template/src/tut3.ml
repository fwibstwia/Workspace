open Cil
open Pretty
open Printf
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
(* Assumption: do not consider expressions with mixed int and float types*)
type varmap = int * int 	(* map float vid to dimension or map int vid to value *)

type memState = { varmaplist:varmap list; pp:pointset_powerset_c_polyhedron}

let id_of_vm   (vm : varmap) : int  = fst vm
let dim_of_vm   (vm : varmap) : int = snd vm

let debug = ref false

let string_pretty () (s: string) =
  s |> text

let rec string_of_le = function
    Variable v -> "V(" ^ string_of_int v ^ ")"
  | Coefficient c ->
      string_of_int (Z.to_int c)
  | Unary_Minus e ->
      "-(" ^ string_of_le e ^ ")"
  | Unary_Plus e ->
      string_of_le e
  | Plus (e1, e2) ->
     "(" ^ string_of_le e1 ^ " + " ^
      string_of_le e2 ^ ")"
  | Minus (e1, e2) ->
     "(" ^ string_of_le e1 ^ " - " ^
      string_of_le e2 ^ ")"
  | Times (c, e) ->
     string_of_int (Z.to_int c) ^"*(" ^ string_of_le e ^ " ) "
  | _ -> "error"

let rec string_of_lc = function
    Less_Than (le1, le2) ->
      string_of_le le1 ^ " < " ^
	string_of_le le2 ^ ", "
  | Less_Or_Equal (le1, le2) ->
     string_of_le le1 ^ " <= " ^
       string_of_le le2 ^ ", "
  | Equal (le1, le2) ->
     string_of_le le1 ^ " == " ^
      string_of_le le2 ^ ", "
  | Greater_Than (le1, le2) ->
     string_of_le le1 ^ " > " ^
       string_of_le le2 ^ ", "
  | Greater_Or_Equal (le1, le2) ->
     string_of_le le1 ^ " >= " ^
       string_of_le le2 ^ ", "

let string_of_poly poly = L.fold_left (fun s c -> s ^ string_of_lc c) ""  (ppl_Polyhedron_get_constraints poly)

let string_of_power_poly  pp =
  let iter = ppl_Pointset_Powerset_C_Polyhedron_begin_iterator pp in
  let end_iter = ppl_Pointset_Powerset_C_Polyhedron_end_iterator pp in
  let s = Array.make 2 "" in
  let i = ref 0 in
  while not (ppl_Pointset_Powerset_C_Polyhedron_iterator_equals_iterator iter end_iter) do
    let poly = ppl_Pointset_Powerset_C_Polyhedron_get_disjunct iter in
    Array.set s (!i) (string_of_poly poly);
    ppl_Pointset_Powerset_C_Polyhedron_increment_iterator iter;
    i := !i + 1
  done;
  String.concat "##" (Array.to_list s)

let power_poly_pretty () (memst : memState) =
  string_of_power_poly memst.pp |> text

let poly_pretty () (poly : polyhedron) =
  string_of_poly poly |> text

let le_pretty () (le : linear_expression) =
  string_of_le le |> text

(* let varmap_list_replace (vml : varmap list) (vm : varmap) : varmap list = *)
  (* vm :: (L.remove_assoc (id_of_vm vm) vml) *)
let rec cal_integer_exp(e : exp) (memst : memState) : int =
  match e with
  | Const(CInt64(i, _, _)) -> Int64.to_int i
  | Lval(Var vi, NoOffset) -> let var_val = L.assoc vi.vid memst.varmaplist in
			      var_val
  | BinOp(bo, e1, e2, t) -> cal_binop bo e1 e2 memst
  | _ -> E.error "Unsupported Operator %a" Cil.d_exp e; 0

and cal_binop(b: binop) (e1 : exp) (e2 : exp) (memst : memState) : int =
  let v1, v2 = cal_integer_exp e1 memst, cal_integer_exp e2 memst in
  match b with
  |PlusA -> v1 + v2
  |MinusA -> v1 - v2

let rec construct_linear_of_exp (e : exp) (memst : memState) : linear_expression =
  match e with
  | Const(CInt64(i, _, _)) -> Coefficient (Z.from_int (Int64.to_int i))
  | Const(CReal(f, _, _)) -> Coefficient (Z.from_float f)
  | Lval(Var vi, NoOffset) -> let var_dim = L.assoc vi.vid memst.varmaplist in
      Variable var_dim
  (* | SizeOf _ | SizeOfE _ | SizeOfStr _ | AlignOf _ | AlignOfE _ -> *)
    (* e |> constFold true |> oekind_of_exp vml *)
  (* | UnOp(uo, e, t) -> construct_linear_of_unop uo e memst *)
  | BinOp(bo, e1, e2, t) -> construct_linear_of_binop bo e1 e2 memst
  | CastE(t, e1) -> construct_linear_of_exp e1 memst
  | _ -> E.error "Unsupported Operator %a" Cil.d_exp e; Variable 0

							      
and construct_linear_of_binop (b : binop) (e1 : exp) (e2 : exp) (memst : memState) : linear_expression =
  let l1, l2 = construct_linear_of_exp e1 memst, construct_linear_of_exp e2 memst in
  match b with
  | PlusA -> begin
    Plus (l1, l2)
    end

  | MinusA -> begin
    Minus (l1, l2)
    end
  (* | Times of Z.t * linear_expression *)


  | _ -> E.error "unsupported binop"; l1

and convert_binop_to_constraint (b : binop) (e1 : exp) (e2 : exp) (memst: memState) : linear_constraint =
  let l1, l2 = construct_linear_of_exp e1 memst, construct_linear_of_exp e2 memst in
  match b with
  | Lt -> begin
      Less_Than (l1, l2)
    end
  | Gt -> begin
      Greater_Than (l1, l2)
    end
  | Le -> begin
      Less_Or_Equal (l1, l2)
    end
  | Ge -> begin
      Greater_Or_Equal (l1, l2)
    end
  | Eq-> begin
      Equal (l1, l2)
    end
  | _ -> E.error "unsupported condition"; Less_Than (l1, l2)

and convert_exp_to_constraint(e : exp) (memst: memState) (need_neg : bool) : linear_constraint =
  match e with
  | UnOp(LNot, e1, t) -> convert_exp_to_constraint e1 memst true
  | BinOp(bo, e1, e2, t) -> if need_neg then convert_binop_to_constraint bo e2 e1 memst else
                              convert_binop_to_constraint bo e1 e2 memst
  | _ -> E.error "unsupported condition"; Equal (Variable 0, Variable 1)

						
let varmap_list_replace (vml : varmap list ) (vid: int) (val_e : int): varmap list =
  (vid, val_e) :: ( L.remove_assoc vid vml)
	  
let power_poly_handle_inst (i : instr) (memst : memState) : memState =
  match i with
  | Set((Var vi, NoOffset), e, loc) when not(vi.vglob) &&
                                           isArithmeticType vi.vtype &&
                                             not(isIntegralType vi.vtype) ->
     
     let lexp  = construct_linear_of_exp e memst in
     let dim = L.assoc vi.vid memst.varmaplist in

     ppl_Pointset_Powerset_C_Polyhedron_affine_image memst.pp dim lexp (Z.from_int 1);
     memst;
  
     (* linearize expression *)
  | Set((Var vi, NoOffset), e, loc) when not(vi.vglob)
					 && isIntegralType vi.vtype ->
     let val_e = cal_integer_exp e memst in
     let updated_vml = varmap_list_replace memst.varmaplist vi.vid val_e in
     let updated_memst = {varmaplist = updated_vml; pp = memst.pp} in
     updated_memst

| Set((Mem _, _), _, _)
  (* | Call _ -> varmap_list_kill vml *)
  | _ -> memst
	   
let cal_integer_birel (b : binop) (e1 : exp) (e2 : exp) (memst : memState) : bool =
  let v1, v2 = cal_integer_exp e1 memst, cal_integer_exp e2 memst in
  match b with
  | Lt -> v1 < v2
  | Gt -> v1 > v2
  | Le -> v1 <= v2
  | Ge -> v1 >= v2
  | Eq -> v1 = v2
  | Ne -> v1 <> v2
	 
let cal_integer_rel (c : exp) (memst : memState) : bool =    
  match c with
  | BinOp(binop, e1, e2, _) -> cal_integer_birel binop e1 e2 memst     
  | UnOp(LNot, BinOp(binop, e1, e2, _), t) -> not(cal_integer_birel binop e1 e2 memst)

					      
let combine_power_set (pp1 : pointset_powerset_c_polyhedron) (pp2 : pointset_powerset_c_polyhedron) : unit =
  let iter = ppl_Pointset_Powerset_C_Polyhedron_begin_iterator pp2 in
  let end_iter = ppl_Pointset_Powerset_C_Polyhedron_end_iterator pp2 in
  while not (ppl_Pointset_Powerset_C_Polyhedron_iterator_equals_iterator iter end_iter) do
    let poly = ppl_Pointset_Powerset_C_Polyhedron_get_disjunct iter in
    ppl_Pointset_Powerset_C_Polyhedron_add_disjunct pp1 poly;
    ppl_Pointset_Powerset_C_Polyhedron_increment_iterator iter
  done;


module PowerPolyDF = struct
  let name = "PowerPolyhedra"
  let debug = debug
  type t = memState
  (* let copy memst = memst *)
  let copy memst = {varmaplist = memst.varmaplist; pp = ppl_new_Pointset_Powerset_C_Polyhedron_from_Pointset_Powerset_C_Polyhedron memst.pp}
  let stmtStartData = IH.create 64
  let pretty = power_poly_pretty
  let computeFirstPredecessor stm memst = memst

  let combinePredecessors (s : stmt) ~(old : t) (newMemst : t) =
    if ppl_Pointset_Powerset_C_Polyhedron_geometrically_equals_Pointset_Powerset_C_Polyhedron old.pp newMemst.pp then None else
      begin
        let iter = ppl_Pointset_Powerset_C_Polyhedron_begin_iterator newMemst.pp in
        let end_iter = ppl_Pointset_Powerset_C_Polyhedron_end_iterator newMemst.pp in
        while not (ppl_Pointset_Powerset_C_Polyhedron_iterator_equals_iterator iter end_iter) do
          let poly = ppl_Pointset_Powerset_C_Polyhedron_get_disjunct iter in
          ppl_Pointset_Powerset_C_Polyhedron_add_disjunct old.pp poly;
          ppl_Pointset_Powerset_C_Polyhedron_increment_iterator iter
        done;
        E.error "%a" power_poly_pretty old;
	Some(old)
      end

  let doInstr (i : instr) (memst : t) =
    let action = power_poly_handle_inst i  in
    DF.Post action


  let doStmt stm memst = DF.SDefault

  let doGuard (c : exp) (memst : t) =
    if true then begin
      E.error "visit while";
      if cal_integer_rel c memst then
	let memst_copy = copy memst in
	DF.GUse memst_copy
      else
	DF.GUnreachable
      end
    else
      let condConstraint = convert_exp_to_constraint c memst false in
      match c with
      | UnOp(LNot, e1, t) ->
	 begin
           let memst_copy = copy memst in
           ppl_Pointset_Powerset_C_Polyhedron_add_constraint memst_copy.pp condConstraint;
           DF.GUse memst_copy
	 end
      | _ ->
	 begin
           let memst_copy = copy memst in
           ppl_Pointset_Powerset_C_Polyhedron_add_constraint memst_copy.pp condConstraint;
           DF.GUse memst_copy
	 end
  let filterStmt stm = true

end


module PowerPolyFDF = DF.ForwardsDataFlow(PowerPolyDF)

let rec mapVarToIndex (i:int) (l) : varmap list =
  match l with
  |[] -> []
  |x::rl -> if isIntegralType x.vtype then
	      (x.vid, 0)::mapVarToIndex i rl (*map the integer variable to value 0*)
	    else
	      (x.vid, i)::mapVarToIndex (i + 1) rl (* map the float variable to dimension *)


let collectVars (fd : fundec) : varmap list=
  (fd.sformals @ fd.slocals)
  |> L.filter (fun vi -> isArithmeticType vi.vtype)
  |> mapVarToIndex 0


let computePowerPoly (fd : fundec) : unit =
  Cfg.clearCFGinfo fd;
  ignore(Cfg.cfgFun fd);
  let first_stmt = L.hd fd.sbody.bstmts in
  let varmaplist = collectVars fd in
  let var_length = L.length varmaplist in
  let poly = ppl_new_C_Polyhedron_from_space_dimension var_length Universe in
  let powerPoly = ppl_new_Pointset_Powerset_C_Polyhedron_from_C_Polyhedron poly in
  let memst = {varmaplist = varmaplist; pp = powerPoly} in
  IH.clear PowerPolyDF.stmtStartData;
  IH.add PowerPolyDF.stmtStartData first_stmt.sid memst;
  PowerPolyFDF.compute [first_stmt]


let getMemState (sid : int) : memState option =
  try Some(IH.find PowerPolyDF.stmtStartData sid)
  with Not_found -> None


let instrPowerPoly (il : instr list) (memst : memState) : memState list =
  let proc_one hil i =
    match hil with
    | [] -> (power_poly_handle_inst i memst) :: hil
    | memst':: rst as l -> (power_poly_handle_inst i memst') :: l
  in
  il |> L.fold_left proc_one [memst]
     |> L.tl
     |> L.rev


class vmlVisitorClass = object(self)
  inherit nopCilVisitor

  val mutable sid           = -1
  val mutable state_list    = []
  val mutable current_state = None

  method vstmt stm =
    sid <- stm.sid;
    begin match getMemState sid with
    | None -> current_state <- None
    | Some memst -> begin
      match stm.skind with
      | Instr il ->
        current_state <- None;
        state_list <- instrPowerPoly il memst
      | _ -> current_state <- None
    end end;
    DoChildren

  method vinst i =
    try let data = L.hd state_list in
        current_state <- Some(data);
        state_list <- L.tl state_list;
        DoChildren
    with Failure "hd" -> DoChildren

  method get_cur_vml () =
    match current_state with
    | None -> getMemState sid
    | Some memst -> Some memst

end


class varUseReporterClass = object(self)
  inherit vmlVisitorClass as super

  method vvrbl (vi : varinfo) =
    match self#get_cur_vml () with
    | None -> SkipChildren
    | Some memst -> begin
        E.log "%a: %a\n" d_loc (!currentLoc) power_poly_pretty memst
      end;
      SkipChildren
end


let powerPolyAnalysis (fd : fundec) (loc : location) : unit =
  computePowerPoly fd;
  let vis = ((new varUseReporterClass) :> nopCilVisitor) in
  ignore(visitCilFunction vis fd)


let tut3 (f : file) : unit =
  iterGlobals f (onlyFunctions powerPolyAnalysis)

let parseOneFile (fname: string) : C.file =
  let cabs, cil = F.parse_with_cabs fname () in
  Rmtmps.removeUnusedTemps cil;
  cil

let outputFile (f : C.file) : unit =
  if !O.outFile <> "" then
    try
      let c = open_out !O.outFile in

      C.print_CIL_Input := false;
      Stats.time "printCIL"
        (C.dumpFile (!C.printerForMaincil) c !O.outFile) f;
      close_out c
    with _ ->
      E.s (E.error "Couldn't open file %s" !O.outFile)

let processOneFile (cil: C.file) : unit =
  if !(O.enable_tut.(3)) then tut3 cil;
  outputFile cil
;;

let main () =

  C.print_CIL_Input := true;


  C.insertImplicitCasts := false;


  C.lineLength := 100000;


  C.warnTruncate := false;


  E.colorFlag := true;


  Cabs2cil.doCollapseCallCast := true;



  let usageMsg = "Usage: ciltutcc [options] source-files" in
  Arg.parse (O.align ()) Ciloptions.recordFile usageMsg;

  Ciloptions.fileNames := List.rev !Ciloptions.fileNames;
  let files = List.map parseOneFile !Ciloptions.fileNames in
  let one =
    match files with
	  | [] -> E.s (E.error "No file names provided")
    | [o] -> o
    | _ -> Mergecil.merge files "stdout"
  in
  processOneFile one
;;


begin
  try
    main ()
  with
  | F.CabsOnly -> ()
  | E.Error -> ()
end;
exit (if !E.hadErrors then 1 else 0)
