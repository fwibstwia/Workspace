(* TODO: handle Assignment *)
(* TODO: handle terminal condition *)
(* TODO: handle powerset combine *)
(* TODO: handle the if condition *)
open Cil
open Pretty
open Tututil
open Ppl_ocaml

module L  = List
module E  = Errormsg

module IH = Inthash
module DF = Dataflow

module F = Frontc
module C = Cil

module O = Ciltutoptions


let debug = ref false


(* type oekind = Top | Odd | Even | Bottom *)

type lekind = linear_expression

type varmap = int * (varinfo * lekind)

let id_of_vm   (vm : varmap) : int     = fst vm
let vi_of_vm   (vm : varmap) : varinfo = vm |> snd |> fst
let kind_of_vm (vm : varmap) : lekind  = vm |> snd |> snd

let print_string_if_very_noisy = function str ->
  let less_noisy = "LESS_NOISY"
  in let very_noisy = try Sys.getenv "PPL_VERY_NOISY_TESTS"
    with Not_found -> less_noisy
  in
    if (very_noisy != less_noisy)
    then print_string str;
;;

let print_string_if_noisy = function str ->
  let quiet = "QUIET"
  in let noisy = try Sys.getenv "PPL_NOISY_TESTS"
    with Not_found -> quiet
  in
    if (noisy != quiet)
    then print_string str
    else print_string_if_very_noisy str;
;;

let print_int_if_very_noisy = function num ->
  let less_noisy = "LESS_NOISY"
  in let very_noisy = try Sys.getenv "PPL_VERY_NOISY_TESTS"
    with Not_found -> less_noisy
  in
    if (very_noisy != less_noisy)
    then print_int num;
;;

let print_int_if_noisy = function num ->
  let quiet = "QUIET"
  in let noisy = try Sys.getenv "PPL_NOISY_TESTS"
    with Not_found -> quiet
  in
    if (noisy != quiet)
    then print_int num
    else print_int_if_very_noisy num;
;;

let rec print_linear_expression = function
    Variable v ->
      print_string_if_noisy "V(";
      print_int_if_noisy v;
      print_string_if_noisy ")";
  | Coefficient c ->
      print_int_if_noisy(Z.to_int c)
  | Unary_Minus e ->
      print_string_if_noisy "-(";
      print_linear_expression e;
      print_string_if_noisy ")";
  | Unary_Plus e ->
      print_linear_expression e
  | Plus (e1, e2) ->
      print_string_if_noisy "(";
      print_linear_expression e1;
      print_string_if_noisy " + ";
      print_linear_expression e2;
      print_string_if_noisy ")";
  | Minus (e1, e2) ->
      print_string_if_noisy "(";
      print_linear_expression e1;
      print_string_if_noisy " - ";
      print_linear_expression e2;
      print_string_if_noisy ")";
  | Times (c, e) ->
      print_int_if_noisy(Z.to_int c);
      print_string_if_noisy "*(";
      print_linear_expression e;
      print_string_if_noisy ")";
;;

let string_of_oekind (k : lekind) : string =
    print_linear_expression k

let string_of_varmap (vm : varmap) : string =
  let vi = vi_of_vm vm in
  "("^vi.vname^", "^(vm |> kind_of_vm |> string_of_oekind)^")"


let string_of_varmap_list (vml : varmap list) : string =
  vml
  |> L.map string_of_varmap
  |> String.concat ", "


let varmap_list_pretty () (vml : varmap list) =
  vml |> string_of_varmap_list |> text


let oekind_neg (k : oekind) : oekind =
  match k with
  | Even -> Odd
  | Odd -> Even
  | _ -> k


let varmap_equal (vm1 : varmap) (vm2 : varmap) : bool =
  (id_of_vm vm1) = (id_of_vm vm2) &&
  (kind_of_vm vm1) = (kind_of_vm vm2)


let varmap_list_equal (vml1 : varmap list) (vml2 : varmap list) : bool =
  let sort = L.sort (fun (id1,_) (id2,_) -> compare id1 id2) in
  list_equal varmap_equal (sort vml1) (sort vml2)


let oekind_includes (is_this : oekind) (in_this : oekind) : bool =
  match is_this, in_this with
  | _, Top -> true
  | Bottom, _ -> true
  | _, _ -> false




let power_poly_equal

let power_poly_combine (pp1: Pointset_Powerset_C_Polyhedron) (pp2: Pointset_Powerset_C_Polyhedron) :
      Pointset_Powerset_C_Polyhedron option =


let oekind_combine (k1 : oekind) (k2 : oekind) : oekind =
  match k1, k2 with
  | Top, _ | _, Top | Odd, Even | Even, Odd -> Top
  | Odd, _ | _, Odd -> Odd
  | Even, _ | _, Even -> Even
  | Bottom, Bottom -> Bottom

let varmap_combine (vm1 : varmap) (vm2 : varmap) : varmap option =
  match vm1, vm2 with
  | (id1, _), (id2, _) when id1 <> id2 -> None
  | (id1, (vi1, k1)), (_,(_,k2)) -> Some(id1,(vi1,oekind_combine k1 k2))


(* let varmap_list_replace (vml : varmap list) (vm : varmap) : varmap list = *)
  (* vm :: (L.remove_assoc (id_of_vm vm) vml) *)


let rec construct_linear_of_exp (e : exp) : linear_expression list =
  match e with
  | Const(CInt64(i, _, _)) -> [Coefficient i]
  (* | Lval(Var vi, NoOffset) -> vml |> L.assoc vi.vid |> snd *)
  (* | SizeOf _ | SizeOfE _ | SizeOfStr _ | AlignOf _ | AlignOfE _ -> *)
    (* e |> constFold true |> oekind_of_exp vml *)
  (* | UnOp(uo, e, t) -> oekind_of_unop vml uo e *)
  | BinOp(bo, e1, e2, t) -> construct_linear_of_binop bo e1 e2
  (* | CastE(t, e) -> oekind_of_exp vml e *)
  | _ -> [Coefficient 0]        (* TODO: handle other cases *)


(* Construct unary op *)
(* and construct_linear_of_unop (vml : linear_expression list) (u : unop) (e : exp) : linear_expression list = *)

let construct_linear_combine (op) (l1 : linear_expression list) (l2 : linear_expression list) : linear_expression list =
  L.fold_left (construct_linear_combine_one op l1) l2

let construct_linear_combine_one (op) (l : linear_expression list) (le : linear_expression) : linear_expression list =
  L.map (op le) l

and construct_linear_of_binop (b : binop) (e1 : exp) (e2 : exp) : linear_expression list =
  let l1, l2 = construct_linear_of_exp e1, construct_linear_of_exp e2 in
  match b with
  | PlusA -> begin
    construct_linear_combine Plus l1 l2
    end

  | MinusA -> begin
    construct_linear_combine Minus l1 l2
    end
  (* | Mult -> begin *)
    (* end *)

  (* TODO: Handle more binary operations *)
   | _ -> l1

let power_poly_handle_inst (i : instr) (pp : Pointset_Powerset_C_Polyhedron) : Pointset_Powerset_C_Polyhedron =
  match i with
  | Set((Var vi, NoOffset), e, loc) when not(vi.vglob) && (isIntegralType vi.vtype) ->
     let le_list = construct_linear_of_exp e in
     L.map (fun e ->
            ppl_Pointset_Powerset_C_Polyhedron_add_constraint(Equal (Variable vi.vid, e))) le_list
     (* linearize expression *)

  | Set((Mem _, _), _, _)
  (* | Call _ -> varmap_list_kill vml *)
  | _ -> pp


module PowerPolyDF = struct

  let name = "Polyhedra"
  let debug = debug
  type t = Pointset_Powerset_C_Polyhedron
  let copy powerPoly = powerPoly
  let stmtStartData = IH.create 64
  let pretty = power_poly_pretty
  let computeFirstPredecessor stm powerPoly = powerPoly


  let combinePredecessors (s : stmt) ~(old : t) (ll : t) =
    if power_poly_equal old ll then None else
    Some(power_poly_combine old ll)

  let doInstr (i : instr) (ll : t) =
    let action = power_poly_handle_inst i in
    DF.Post action


  let doStmt stm ll = DF.SDefault

  let doGuard c ll =
  let filterStmt stm = true

end


module PowerPolyFDF = DF.ForwardsDataFlow(PowerPolyDF)


let collectVars (fd : fundec) : int =
  (fd.sformals @ fd.slocals)
  |> L.filter (fun vi -> isIntegralType vi.vtype)
  |> L.length


let computePowerPoly (fd : fundec) : unit =
  Cfg.clearCFGinfo fd;
  ignore(Cfg.cfgFun fd);
  let first_stmt = L.hd fd.sbody.bstmts in
  let var_length = collectVars fd in
  let powerPoly = ppl_new_Pointset_Powerset_C_Polyhedron_from_space_dimension var_length in
  IH.clear DF.stmtStartData;
  IH.add PolyhedraDF.stmtStartData first_stmt.sid powerPoly;
  PowerPolyFDF.compute [first_stmt]


let getOddEvens (sid : int) : varmap list option =
  try Some(IH.find OddEvenDF.stmtStartData sid)
  with Not_found -> None


let instrOddEvens (il : instr list) (vml : varmap list) : varmap list list =
  let proc_one hil i =
    match hil with
    | [] -> (varmap_list_handle_inst i vml) :: hil
    | vml':: rst as l -> (varmap_list_handle_inst i vml') :: l
  in
  il |> L.fold_left proc_one [vml]
     |> L.tl
     |> L.rev


class vmlVisitorClass = object(self)
  inherit nopCilVisitor

  val mutable sid           = -1
  val mutable state_list    = []
  val mutable current_state = None

  method vstmt stm =
    sid <- stm.sid;
    begin match getOddEvens sid with
    | None -> current_state <- None
    | Some vml -> begin
      match stm.skind with
      | Instr il ->
        current_state <- None;
        state_list <- instrOddEvens il vml
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
    | None -> getOddEvens sid
    | Some vml -> Some vml

end


class varUseReporterClass = object(self)
  inherit vmlVisitorClass as super

  method vvrbl (vi : varinfo) =
    match self#get_cur_vml () with
    | None -> SkipChildren
    | Some vml -> begin
      if L.mem_assoc vi.vid vml then begin
        let vm = (vi.vid, L.assoc vi.vid vml) in
        E.log "%a: %a\n" d_loc (!currentLoc) varmap_list_pretty [vm]
      end;
      SkipChildren
    end

end


let evenOddAnalysis (fd : fundec) (loc : location) : unit =
  computeOddEven fd;
  let vis = ((new varUseReporterClass) :> nopCilVisitor) in
  ignore(visitCilFunction vis fd)


let tut3 (f : file) : unit =
  iterGlobals f (onlyFunctions evenOddAnalysis)

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
