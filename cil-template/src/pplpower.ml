open Ctypes
open Foreign

type dp_form = unit ptr
type ppl_manager = unit ptr
let dp_form:dp_form typ = ptr void
let ppl_manager:ppl_manager typ = ptr void
				      
let init = foreign "init"(ptr void @-> size_t @-> returning ppl_manager)
let copy = foreign "copy"(ppl_manager @-> returning ppl_manager)
let merge = foreign "merge"(ppl_manager @-> ppl_manager @-> returning bool)
let set_affine_form_image = foreign "setAffineFormImage"(ppl_manager @-> int @-> dp_form @-> returning void)
let get_dp_form_constant = foreign "getPolynomialConstant"(ppl_manager @-> float @-> returning dp_form)
let get_dp_form_variable = foreign "getPolynomialVariable"(ppl_manager @-> int @-> returning dp_form)
let get_dp_form_plus = foreign "getPolynomialPlus"(ppl_manager @-> dp_form @-> dp_form @-> returning dp_form)
let get_dp_form_minus = foreign "getPolynomialMinus"(ppl_manager @-> dp_form @-> dp_form @-> returning dp_form)
let get_dp_form_unary_minus = foreign "getPolynomialUnaryMinus"(ppl_manager @-> dp_form @-> returning dp_form)
let get_dp_form_times = foreign "getPolynomialTimes"(ppl_manager @-> dp_form @-> dp_form @-> returning dp_form)				
let add_constraint = foreign "addConstraint"(ppl_manager @-> dp_form @-> dp_form @-> returning void)
let get_constraint_pretty = foreign "getConstraintPretty"(ppl_manager @-> returning string)
