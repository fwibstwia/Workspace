open Ctypes
open Foreign

type linear_form = unit ptr
type ppl_manager = unit ptr
let linear_form:linear_form typ = ptr void
let ppl_manager:ppl_manager typ = ptr void
				      
let init = foreign "init"(ptr void @-> size_t @-> returning ppl_manager)
let copy = foreign "copy"(ppl_manager @-> returning ppl_manager)
let merge = foreign "merge"(ppl_manager @-> ppl_manager @-> returning bool)
let set_affine_form_image = foreign "setAffineFormImage"(ppl_manager @-> int @-> linear_form @-> returning void)
let get_linear_form_constant = foreign "getLinearFormConstant"(ppl_manager @-> float @-> returning linear_form)
let get_linear_form_variable = foreign "getLinearFormVariable"(ppl_manager @-> int @-> returning linear_form)
let get_linear_form_plus = foreign "getLinearFormPlus"(ppl_manager @-> linear_form @-> linear_form @-> returning linear_form)
let get_linear_form_minus = foreign "getLinearFormMinus"(ppl_manager @-> linear_form @-> linear_form @-> returning linear_form)
let add_constraint = foreign "addConstraint"(ppl_manager @-> linear_form @-> linear_form @-> returning void)
let get_constraint_pretty = foreign "getConstraintPretty"(ppl_manager @-> returning string)
