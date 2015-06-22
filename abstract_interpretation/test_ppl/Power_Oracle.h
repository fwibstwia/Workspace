#include <ppl.hh>

struct C_Expr;

struct Floating_Real_Open_Interval_Info_Policy {
  const_bool_nodef(store_special, false);
  const_bool_nodef(store_open, true);
  const_bool_nodef(cache_empty, true);
  const_bool_nodef(cache_singleton, true);
  const_bool_nodef(cache_normalized, false);
  const_int_nodef(next_bit, 0);
  const_bool_nodef(may_be_empty, true);
  const_bool_nodef(may_contain_infinity, false);
  const_bool_nodef(check_empty_result, false);
  const_bool_nodef(check_inexact, false);
};

typedef Interval_Info_Bitset<unsigned int,
                             Floating_Real_Open_Interval_Info_Policy> Floating_Real_Open_Interval_Info;

typedef Interval<mpq_class,
                 Floating_Real_Open_Interval_Info> FP_Interval;

typedef Box<FP_Interval> FP_Interval_Abstract_Store;

