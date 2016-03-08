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

(* branch_dir: record which branch to take, 0 - false branch, 1-true branch, 2-undetermine*)			 
type memState = { intvarmaplist:intVarmap list;  pm:ppl_manager; branch_dir: int}

type expState = IntValue of int | DPForm of dp_form

let id_of_vm   (vm : intVarmap) : int  = fst vm
let val_of_vm   (vm : intVarmap) : int = snd vm

let debug = ref false
							    
let string_pretty () (s: string) =
  s |> text

let power_poly_pretty () (memst : memState) =
  get_constraint_pretty memst.pm |> text
  
let rec construct_linear_of_exp (e : exp) (memst : memState) : expState =
  match e with
  | Const(CInt64(i, _, _)) -> IntValue (Int64.to_int i)
  | Const(CReal(f, _, _)) -> DPForm (get_dp_form_constant (memst.pm) f)
  | Lval(Var vi, NoOffset) -> 
     if isIntegralType vi.vtype then
       begin
	 IntValue(L.assoc vi.vid memst.intvarmaplist)
       end
     else
       DPForm(get_dp_form_variable memst.pm vi.vid)
  (* | SizeOf _ | SizeOfE _ | SizeOfStr _ | AlignOf _ | AlignOfE _ -> *)
    (* e |> constFold true |> oekind_of_exp vml *)
  (* | UnOp(uo, e, t) -> construct_linear_of_unop uo e memst *)
  | BinOp(bo, e1, e2, t) -> construct_linear_of_binop bo e1 e2 memst
  | UnOp(uo, e1, t) -> construct_linear_of_unop uo e1 memst  	      	
  | CastE(t, e1) -> construct_linear_of_exp e1 memst
  | _ -> E.error "Unsupported Operator %a" Cil.d_exp e; IntValue 0
								 
and construct_linear_of_unop (uo : unop) (e1 : exp) (memst : memState) : expState =
  let l1 = construct_linear_of_exp e1 memst in
  match uo with
  |Neg -> begin
      match l1 with
      |IntValue v1 -> IntValue (-v1)
      |DPForm lf1 ->  DPForm (get_dp_form_unary_minus memst.pm lf1)
    end
  |_ -> E.error "unsupported unary operation"; l1
 
    
and construct_linear_of_binop (b : binop) (e1 : exp) (e2 : exp) (memst : memState) : expState=
  let l1, l2 = construct_linear_of_exp e1 memst, construct_linear_of_exp e2 memst in

  match b with
  | PlusA -> begin
      match l1, l2 with
      |IntValue v1, IntValue v2 -> IntValue (v1 + v2)
      |DPForm lf1, DPForm lf2 -> DPForm(get_dp_form_plus memst.pm lf1 lf2)
      |_, _ -> E.error "mixed type exp"; IntValue(0)
    end

  | MinusA -> begin
      match l1, l2 with
      |IntValue v1, IntValue v2 -> IntValue (v1 - v2)
      |DPForm lf1, DPForm lf2 -> DPForm(get_dp_form_minus memst.pm lf1 lf2)
      |_, _ -> E.error "mixed type exp"; IntValue(0)
    end

  |Mult -> begin
      match l1, l2 with
      |IntValue v1, IntValue v2 -> IntValue (v1 * v2)
      |DPForm lf1, DPForm lf2 -> DPForm(get_dp_form_times memst.pm lf1 lf2)
      |_, _ -> E.error "mixed type exp"; IntValue(0)
    end
  (* | Times of Z.t * linear_expression *)
  | _ -> E.error "unsupported binop"; l1

(* eval_exp_constraint: 0 - false; 1-true; 2-undecide *)
and eval_exp_constraint (b : binop) (e1 : exp) (e2 : exp) (memst: memState) : memState =
  let l1, l2 = construct_linear_of_exp e1 memst, construct_linear_of_exp e2 memst in
  match b with
  | Lt -> begin
      match l1, l2 with
      |IntValue v1, IntValue v2 -> begin
	  let br_d = if(v1 < v2) then 0 else 1 in
	  let updated_memst = {intvarmaplist = memst.intvarmaplist; pm = memst.pm; branch_dir = br_d} in
	  updated_memst
	end
      |DPForm lf1, DPForm lf2 ->begin
          ppl_eval_equal_constraint memst.pm lf1 lf2;
	  memst
	end
      |_ , _ -> E.error "mixed type exp"; memst
    end
  | _ -> E.error "unsupported condition"; memst

and get_cond_bad_state(c : exp) (memst: memState) : memState =					    
  match c with
  | BinOp(bo, e1, e2, t) -> eval_exp_constraint bo e1 e2 memst
  | _ -> E.error "unsupported condition"; memst

						
let varmap_list_replace (vml : intVarmap list ) (vid: int) (val_e : int): intVarmap list =
  (vid, val_e) :: ( L.remove_assoc vid vml)
	  

let power_poly_handle_inst (i : instr) (memst : memState) : memState =
  match i with
  | Set((Var vi, NoOffset), e, loc) when not(vi.vglob) &&
                                           isArithmeticType vi.vtype  ->
       begin
	 let es = construct_linear_of_exp e memst in
	 match es with
	 |IntValue v -> begin
	     let updated_vml = varmap_list_replace memst.intvarmaplist vi.vid v in
	     let updated_memst = {intvarmaplist = updated_vml; pm  = memst.pm; branch_dir = memst.branch_dir} in
	     updated_memst
	   end
	 |DPForm lf -> begin
	     set_affine_form_image memst.pm vi.vid lf;
	     memst
	   end
       end
  | Set((Mem _, _), _, _)
  | Call _ 
  | _ -> memst

(*TODO: consider what to do after refine success, should not continue to fold right*)	   
let refine_inst_bad_state(i: instr) (bad_memst : memState) : memState =
  match i with
  | Set((Var vi, NoOffset), e, loc) when not(vi.vglob) &&
                                           isArithmeticType vi.vtype  ->
     begin
       E.error "refine inst bad state";
       let es = construct_linear_of_exp e bad_memst in
       match es with
       |IntValue v -> bad_memst
       |DPForm lf -> begin
	   if (ppl_refine_bad_state bad_memst.pm vi.vid lf) then E.error "success";
	   bad_memst
	 end
     end
	 
  | _ -> bad_memst

(* for each instr in the state refine the bad state in the rev order; 
   otherwise return the original one *)	   
let refine_stmt_bad_state(s: stmt) (bad_memst : memState) : memState =
  match s.skind with
  | Instr il ->
       E.error "stmt bad";
       L.fold_right refine_inst_bad_state il bad_memst
  | _ -> bad_memst
	   	   
let handle_cond_stmt (s : stmt) (memst: memState) : memState =
  match s.skind with
  |If(exp, _, _, _) ->

    begin
      let pred_s = L.hd s.preds in (*hack: multiple previous stmt*)
      let bad_memst = get_cond_bad_state exp memst in
      if bad_memst.branch_dir > 1 then (* if <=1, we already know which branch to take *)
	refine_stmt_bad_state pred_s bad_memst
      else
	bad_memst
    end
  |_ -> memst

let eval_cond_exp(e : exp) (memst: memState) : int =
  match e with
  | UnOp(LNot, BinOp(bo, e1, e2, _), t) ->
     if memst.branch_dir == 0 then 1 else 0
  | BinOp(bo, e1, e2, t) ->
     if memst.branch_dir == 1 then 1 else 0						
  | _ -> E.error "unsupported condition"; 0	  

module PowerPolyDF = struct
  let name = "PowerPolyhedra"
  let debug = debug
  type t = memState
  (* let copy memst = memst *)
  let copy memst = {intvarmaplist = memst.intvarmaplist; pm = copy_pm(memst.pm); branch_dir = memst.branch_dir}
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

  (*check if the stmt is if condition, run check stable module *)	    
  let doStmt stm memst =
    handle_cond_stmt stm memst;
    DF.SDefault

  let doGuard (c : exp) (memst : t) =
    let memst_copy = copy memst in
    let dir = eval_cond_exp c memst_copy in 
    if dir < 1 then DF.GUnreachable
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
  let memst = {intvarmaplist = intvmlist; pm = powerManager; branch_dir = 2} in
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
	E.log "%a: %a\n" d_loc (!currentLoc) power_poly_pretty data;
        DoChildren
    with Failure "hd" -> DoChildren

  method get_cur_vml () =
    match current_state with
    | None -> getMemState sid
    | Some memst -> Some memst

end

let powerPolyAnalysis (fd : fundec) (loc : location) : unit =
  computePowerPoly fd;
  let vis = ((new vmlVisitorClass) :> nopCilVisitor) in
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
