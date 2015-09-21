(* TODO: handle Assignment *)
(* TODO: handle powerset combine *)
(* TODO: handle the if condition *)
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

type varmap = int * int 	(* map vid to dimension *)
		      
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
  let s = [| "" |] in
  let i = ref 0 in
  while not (ppl_Pointset_Powerset_C_Polyhedron_iterator_equals_iterator iter end_iter) do
    let poly = ppl_Pointset_Powerset_C_Polyhedron_get_disjunct iter in 
    s.(!i) <- string_of_poly poly;
    ppl_Pointset_Powerset_C_Polyhedron_increment_iterator iter;
    i := !i + 1							  
  done;
  String.concat "###########################################" (Array.to_list s)

let power_poly_pretty () (memst : memState) =
  string_of_power_poly memst.pp |> text
let le_pretty () (le : linear_expression) =
  string_of_le le |> text

(* let varmap_list_replace (vml : varmap list) (vm : varmap) : varmap list = *)
  (* vm :: (L.remove_assoc (id_of_vm vm) vml) *)


let rec construct_linear_of_exp (e : exp) (memst : memState) : linear_expression =
  match e with
  | Const(CInt64(i, _, _)) -> Coefficient (Z.from_int 1)
  | Lval(Var vi, NoOffset) -> let var_dim = L.assoc vi.vid memst.varmaplist in
      E.log "%a\n" le_pretty (Variable var_dim);			      
      Variable var_dim
  (* | SizeOf _ | SizeOfE _ | SizeOfStr _ | AlignOf _ | AlignOfE _ -> *)
    (* e |> constFold true |> oekind_of_exp vml *)
  (* | UnOp(uo, e, t) -> oekind_of_unop vml uo e *)
  | BinOp(bo, e1, e2, t) -> construct_linear_of_binop bo e1 e2 memst
  (* | CastE(t, e) -> oekind_of_exp vml e *)
  | _ -> Coefficient (Z.from_int 0)        (* TODO: handle other cases *)


(* Construct unary op *)
(* and construct_linear_of_unop (vml : linear_expression list) (u : unop) (e : exp) : linear_expression list = *)

and construct_linear_of_binop (b : binop) (e1 : exp) (e2 : exp) (memst : memState) : linear_expression =
  let l1, l2 = construct_linear_of_exp e1 memst, construct_linear_of_exp e2 memst in
  match b with
  | PlusA -> begin
    Plus (l1, l2)
    end

  | MinusA -> begin
    Minus (l1, l2)
    end
  (* | Mult -> begin *)
    (* end *)

  (* TODO: Handle more binary operations *)
   | _ -> l1

let power_poly_handle_inst (i : instr) (memst : memState) : memState =
  match i with
  | Set((Var vi, NoOffset), e, loc) when not(vi.vglob) && (isIntegralType vi.vtype) ->
     let le  = construct_linear_of_exp e memst in
     let dim = L.assoc vi.vid memst.varmaplist in
     ppl_Pointset_Powerset_C_Polyhedron_add_constraint memst.pp (Equal (Variable dim, le));
     memst;

     (* linearize expression *)

  | Set((Mem _, _), _, _)
  (* | Call _ -> varmap_list_kill vml *)
  | _ -> memst

let convertExpToConstraint(c : exp) (memst: memState) : linear_constraint = 
  (* TODO convert linear constraint *)
  match c with 
  UnOp (LNot, _, _) -> Less_Or_Equal (Variable 1, Variable 2)
  | _ -> Less_Or_Equal (Variable 0, Variable 1)
	    
module PowerPolyDF = struct
  let name = "PowerPolyhedra"
  let debug = debug
  type t = memState
  let copy memst = {varmaplist = memst.varmaplist; pp = ppl_new_Pointset_Powerset_C_Polyhedron_from_Pointset_Powerset_C_Polyhedron memst.pp}
  let stmtStartData = IH.create 64
  let pretty = power_poly_pretty
  let computeFirstPredecessor stm memst = memst

  let combinePredecessors (s : stmt) ~(old : t) (newMemst : t) =
    if ppl_Pointset_Powerset_C_Polyhedron_geometrically_equals_Pointset_Powerset_C_Polyhedron old.pp newMemst.pp then None else
      begin
	ppl_Pointset_Powerset_C_Polyhedron_concatenate_assign old.pp newMemst.pp;
	Some(old)
      end

  let doInstr (i : instr) (memst : t) =
    let action = power_poly_handle_inst i  in
    DF.Post action


  let doStmt stm memst = DF.SDefault

  let doGuard (c : exp) (memst : t) =
    let condConstraint = convertExpToConstraint c memst in
    E.error "%a" Cil.d_exp c;
     ppl_Pointset_Powerset_C_Polyhedron_refine_with_constraint memst.pp condConstraint;
     DF.GDefault				      
  let filterStmt stm = true

end


module PowerPolyFDF = DF.ForwardsDataFlow(PowerPolyDF)

let rec mapVarToIndex (i:int) (l) : varmap list =
  match l with
  |[] -> []
  |x::rl -> (x.vid, i)::mapVarToIndex (i + 1) rl


let collectVars (fd : fundec) : varmap list=
  (fd.sformals @ fd.slocals)
  |> L.filter (fun vi -> isIntegralType vi.vtype)
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
