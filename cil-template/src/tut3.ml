open Cil
open Pretty
open Printf
open Tututil
open Ctypes
open Pplpower

module L  = List
module E  = Errormsg

module IH = Inthash
module DF = Dataflow

module F = Frontc
module C = Cil

module O = Ciltutoptions
(* Assumption: do not consider expressions with mixed int and float types*)
type intVarmap = int * int 	(*map int vid to value *)
		   
type memState = { intvarmaplist:intVarmap list; pm:ppl_manager}

type expState = IntValue of int | LinearForm of linear_form

let id_of_vm   (vm : intVarmap) : int  = fst vm
let val_of_vm   (vm : intVarmap) : int = snd vm

let debug = ref false

let reorderableStr = "reorderable"
let hasReorderableAttrs : attributes -> bool = hasAttribute reorderableStr
							    
let string_pretty () (s: string) =
  s |> text

let power_poly_pretty () (memst : memState) =
  get_constraint_pretty memst.pm |> text

let rec collect_vars(e:exp) (memst: memState):int list =
  match e with
  |Lval(Var vi, NoOffset) -> vi.vid::[]
  |BinOp(bo, e1, e2, t) -> List.append (collect_vars e1 memst) (collect_vars e2 memst)
  |_ -> E.error "Unsupported reorderable expression"; []
							
let set_reorderable_val (vid:int) (pm:ppl_manager) arr : unit =
  let open Unsigned.Size_t in
  let len = of_int (CArray.length arr) in
  let start = to_voidp (CArray.start arr) in
  set_affine_form_image_reorder pm vid start len    
							
let rec construct_linear_of_exp (e : exp) (memst : memState) : expState =
  match e with
  | Const(CInt64(i, _, _)) -> IntValue (Int64.to_int i)
  | Const(CReal(f, _, _)) -> LinearForm (get_linear_form_constant (memst.pm) f)
  | Lval(Var vi, NoOffset) -> 
     if isIntegralType vi.vtype then
       begin
	 IntValue(L.assoc vi.vid memst.intvarmaplist)
       end
     else
       LinearForm(get_linear_form_variable memst.pm vi.vid)
  (* | SizeOf _ | SizeOfE _ | SizeOfStr _ | AlignOf _ | AlignOfE _ -> *)
    (* e |> constFold true |> oekind_of_exp vml *)
  (* | UnOp(uo, e, t) -> construct_linear_of_unop uo e memst *)
  | BinOp(bo, e1, e2, t) -> construct_linear_of_binop bo e1 e2 memst
  | CastE(t, e1) -> construct_linear_of_exp e1 memst
  | _ -> E.error "Unsupported Operator %a" Cil.d_exp e; IntValue 0
  
and construct_linear_of_binop (b : binop) (e1 : exp) (e2 : exp) (memst : memState) : expState=
  let l1, l2 = construct_linear_of_exp e1 memst, construct_linear_of_exp e2 memst in

  match b with
  | PlusA -> begin
      match l1, l2 with
      |IntValue v1, IntValue v2 -> IntValue (v1 + v2)
      |LinearForm lf1, LinearForm lf2 -> LinearForm(get_linear_form_plus memst.pm lf1 lf2)
      |_, _ -> E.error "mixed type exp"; IntValue(0)
    end

  | MinusA -> begin
      match l1, l2 with
      |IntValue v1, IntValue v2 -> IntValue (v1 - v2)
      |LinearForm lf1, LinearForm lf2 -> LinearForm(get_linear_form_minus memst.pm lf1 lf2)
      |_, _ -> E.error "mixed type exp"; IntValue(0)
    end
  (* | Times of Z.t * linear_expression *)
  | _ -> E.error "unsupported binop"; l1

(* eval_exp_constraint: 0 - false; 1-true; 2-undecide *)
and eval_exp_constraint (b : binop) (e1 : exp) (e2 : exp) (memst: memState) : int =
  let l1, l2 = construct_linear_of_exp e1 memst, construct_linear_of_exp e2 memst in
  match b with
  | Lt -> begin
      match l1, l2 with
      |IntValue v1, IntValue v2 -> begin
	  if(v1 < v2) then 0 else 1
	end
      |LinearForm lf1, LinearForm lf2 ->begin
	  add_constraint memst.pm lf1 lf2;
	  2
	end
      |_ , _ -> E.error "mixed type exp"; 0
    end
  | _ -> E.error "unsupported condition"; 0
						    
and convert_exp_constraint(e : exp) (memst: memState) : int =
  match e with
  | UnOp(LNot, BinOp(bo, e1, e2, _), t) -> eval_exp_constraint bo e2 e1 memst
  | BinOp(bo, e1, e2, t) -> eval_exp_constraint bo e1 e2 memst
  | _ -> E.error "unsupported condition"; 0

						
let varmap_list_replace (vml : intVarmap list ) (vid: int) (val_e : int): intVarmap list =
  (vid, val_e) :: ( L.remove_assoc vid vml)
	  

let power_poly_handle_inst (i : instr) (memst : memState) : memState =
  match i with
  | Set((Var vi, NoOffset), e, loc) when not(vi.vglob) &&
                                           isArithmeticType vi.vtype  ->
     if hasReorderableAttrs vi.vattr then
       let varlist = collect_vars e memst in 
       set_reorderable_val vi.vid memst.pm (CArray.of_list int varlist);
       memst
     else
       begin
	 let es = construct_linear_of_exp e memst in
	 match es with
	 |IntValue v -> begin
	     let updated_vml = varmap_list_replace memst.intvarmaplist vi.vid v in
	     let updated_memst = {intvarmaplist = updated_vml; pm  = memst.pm} in
	     updated_memst
	   end
	 |LinearForm lf -> begin
	     set_affine_form_image memst.pm vi.vid lf;
	     memst
	   end
       end
  | Set((Mem _, _), _, _)
  (* | Call _ -> varmap_list_kill vml *)
  | _ -> memst

module PowerPolyDF = struct
  let name = "PowerPolyhedra"
  let debug = debug
  type t = memState
  (* let copy memst = memst *)
  let copy memst = {intvarmaplist = memst.intvarmaplist; pm = copy(memst.pm)}
  let stmtStartData = IH.create 64
  let pretty = power_poly_pretty
  let computeFirstPredecessor stm memst = memst

  let combinePredecessors (s : stmt) ~(old : t) (newMemst : t) =
    let isStable = merge old.pm newMemst.pm in
    if isStable then begin E.log "visit None"; None end
    else
      begin
	Some(newMemst)	    
      end

  let doInstr (i : instr) (memst : t) =
    let action = power_poly_handle_inst i  in
    DF.Post action

  let doStmt stm memst = DF.SDefault

  let doGuard (c : exp) (memst : t) =
    let memst_copy = copy memst in
    let r = convert_exp_constraint c memst_copy in
    if r < 1 then DF.GUnreachable
    else DF.GUse memst_copy
		 
  let filterStmt stm = true

end


module PowerPolyFDF = DF.ForwardsDataFlow(PowerPolyDF)

let rec constructIntVidList (l) : intVarmap list =
  match l with
  |[] -> []
  |x::rl -> (x.vid, 0)::constructIntVidList rl

let collectIntVars (fd : fundec) : intVarmap list=
  (fd.sformals @ fd.slocals)
  |> L.filter (fun vi -> isIntegralType vi.vtype)
  |> constructIntVidList

let rec fetchFloatVidList (l) : int list =
  match l with
  |[] -> []
  |x::rl -> x.vid::fetchFloatVidList rl
					    
let collectFloatVars (fd : fundec) : int list=
  (fd.sformals @ fd.slocals)
  |> L.filter (fun vi -> isArithmeticType vi.vtype &&
                                             not(isIntegralType vi.vtype) )
  |> fetchFloatVidList
    
let initPowerManager arr =
  let open Unsigned.Size_t in
  let len = of_int (CArray.length arr) in
  let start = to_voidp (CArray.start arr) in
  init start len
  
let computePowerPoly (fd : fundec) : unit =
  Cfg.clearCFGinfo fd;
  ignore(Cfg.cfgFun fd);
  let first_stmt = L.hd fd.sbody.bstmts in
  let intvmlist = collectIntVars fd in
  let floatvarlist = collectFloatVars fd in
  let powerManager = initPowerManager (CArray.of_list int floatvarlist) in
  let memst = {intvarmaplist = intvmlist; pm = powerManager} in
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
