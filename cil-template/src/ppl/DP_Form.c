#include <iomanip>
#include <fenv.h>
#include <iostream>

#include "DP_Form.h"

using namespace std;
using namespace Parma_Polyhedra_Library;

void DP_Form::convertToLC(PPL_Manager *manager, FP_Interval_Abstract_Store &inv_store,
			  Linear_Form<FP_Interval> &lower,
			  Linear_Form<FP_Interval> &upper, int affine_image_vid){
  mpq_class error;
  int n = 0;
  vector<pair<vector<float>, vector<int> > >::iterator it = lk.begin();
  while(it != lk.end()){
    MPQ_Interval ck_bound, vk_bound;
    int vk_id = -1;
    error += convertLktoLinearForm(manager, inv_store, (*it).first, (*it).second, ck_bound, &vk_id, affine_image_vid);
    if(vk_id == -1){//inhomogeneous term
      convertLktoLinearConstraint(ck_bound, lower, upper);
    }else{
      Variable vk = *(manager -> varIdMap[vk_id]);
      FP_Interval vk_fp_bound = inv_store.get_interval(vk);
      vk_bound.lower() = vk_fp_bound.lower();
      vk_bound.upper() = vk_fp_bound.upper();
      convertLktoLinearConstraint(ck_bound, vk_bound, vk, lower, upper);
    }

    n ++;
    it ++;    
  }
  
  error = error * getGammaN(n);
  MPQ_Interval error_bound;
  error_bound.lower() = - error;
  error_bound.upper() = error;
  convertLktoLinearConstraint(error_bound, lower, upper);
}

mpq_class DP_Form::convertLktoLinearForm(PPL_Manager *manager,
					 FP_Interval_Abstract_Store &inv_store,
					 const vector<float> &lk_cons,
					 const vector<int> &lk_var_ids,
					 MPQ_Interval &ck_bound,
					 int *vk_id, int affine_image_vid){
  //caculate the error bound for multiplication, without considering underflow
  int n = 0;
  mpq_class error(1);
  ck_bound.lower() = 1;
  ck_bound.upper() = 1;
  
  vector<float>::const_iterator cons_it = lk_cons.begin();
  MPQ_Interval inv_c;
  while(cons_it != lk_cons.end()){
    mpq_class v (*cons_it);
    inv_c.lower() = v;
    inv_c.upper() = v;
    ck_bound = MPQInvMult(ck_bound, inv_c);
    error = error * getMaxAbsoluteBound(inv_c);
    n ++;
    cons_it ++;
  }

  bool find_vk = false;
  MPQ_Interval inv_v;  
  vector<int>::const_iterator it = lk_var_ids.begin();
  while(it != lk_var_ids.end()){
    if(*it != affine_image_vid && !find_vk){
      *vk_id = *it;
      find_vk = true;
      Variable v = *(manager -> varIdMap[*it]);
      FP_Interval v_bound = inv_store.get_interval(v);
      inv_v.lower() = v_bound.lower();
      inv_v.upper() = v_bound.upper();
      error = error * getMaxAbsoluteBound(inv_v);
    }else{
      Variable v = *(manager -> varIdMap[*it]);
      FP_Interval v_bound = inv_store.get_interval(v);
      inv_v.lower() = v_bound.lower();
      inv_v.upper() = v_bound.upper();
      ck_bound = MPQInvMult(ck_bound, inv_v);
      error = error * getMaxAbsoluteBound(inv_v);
    }
    it ++;
    n ++;
  }

  error = error * (1 + getGammaN(n));
  return error;  
}

MPQ_Interval DP_Form::MPQInvMult(const MPQ_Interval &op1, const MPQ_Interval &op2){
  mpq_class vl_lower (op1.lower());
  mpq_class vl_upper (op1.upper());
  mpq_class vr_lower (op2.lower());
  mpq_class vr_upper (op2.upper());
  mpq_class lower = vl_lower * vr_lower, upper = vl_lower * vr_lower;
  
  mpq_class vll_vru = vl_lower * vr_upper;
  if(vll_vru > upper){
    upper = vll_vru;
  }else if (vll_vru < lower){
    lower = vll_vru;
  }
  
  mpq_class vlu_vrl = vl_upper * vr_lower;
  if(vlu_vrl > upper){
    upper = vlu_vrl;
  }else if(vlu_vrl < lower){
    lower = vlu_vrl;
  }
  
  mpq_class vlu_vru = vl_upper * vr_upper;
  if(vlu_vru > upper){
    upper = vlu_vru;
  }else if(vlu_vru < lower){
    lower = vlu_vru;
  }

  MPQ_Interval r;
  r.lower() = lower;
  r.upper() = upper;
  return r;  
}

mpq_class DP_Form::getGammaN(int n){
  mpq_class num_limit = numeric_limits<float>::denorm_min();
  return (n-1) * num_limit /(1-(n-1)*num_limit);
}

mpq_class DP_Form::getMaxAbsoluteBound(const MPQ_Interval &inv){
  mpq_class mpq_lower (abs(inv.lower()));
  mpq_class mpq_upper (abs(inv.upper()));
  if(mpq_lower > mpq_upper)
    return mpq_lower;
  else
    return mpq_upper;
}

void DP_Form::convertLktoLinearConstraint(MPQ_Interval &ck_bound, MPQ_Interval &vk_bound, Variable &vk,
  Linear_Form<FP_Interval> &lf_lower, Linear_Form<FP_Interval> &lf_upper){
  Linear_Form<FP_Interval> lf_vk(vk);
  
  if(vk_bound.lower() >= 0){
    FP_Interval ak = convertMpqToFPInterval(ck_bound.lower(), MPFR_RNDD);
    FP_Interval bk = convertMpqToFPInterval(ck_bound.upper(), MPFR_RNDU);    
    lf_lower += ak*lf_vk;
    lf_upper += bk*lf_vk;
  }else if(vk_bound.upper() <= 0){
    FP_Interval ak = convertMpqToFPInterval(ck_bound.lower(),
					    MPFR_RNDD);
    FP_Interval bk = convertMpqToFPInterval(ck_bound.upper(), MPFR_RNDU);    
    lf_lower += bk*lf_vk;
    lf_upper += ak*lf_vk;
  }else{
    mpq_class ak (ck_bound.lower());
    mpq_class bk (ck_bound.upper());
    mpq_class vk_lower (vk_bound.lower());
    mpq_class vk_upper (vk_bound.upper());
    mpq_class c = (ak*vk_upper - bk*vk_lower)/(vk_upper - vk_lower);
    mpq_class d = (bk*vk_upper - ak*vk_lower)/(vk_upper - vk_lower);
    mpq_class c_endp = bk * vk_lower;
    mpq_class d_endp = bk * vk_upper;

    FP_Interval inv_c = convertMpqToFPInterval(c, MPFR_RNDD);
    FP_Interval inv_c_endp = convertMpqToFPInterval(c_endp, MPFR_RNDD);
    FP_Interval inv_vk_lower = convertMpqToFPInterval(vk_bound.lower(), MPFR_RNDD);
    Linear_Form<FP_Interval> lf_vk_lower(inv_vk_lower), lf_c_endp(inv_c_endp); 
    lf_lower += inv_c*(lf_vk - lf_vk_lower) + lf_c_endp;
    
    FP_Interval inv_d = convertMpqToFPInterval(d, MPFR_RNDD);
    FP_Interval inv_d_endp = convertMpqToFPInterval(d_endp, MPFR_RNDU);
    FP_Interval inv_vk_upper = convertMpqToFPInterval(vk_bound.upper(), MPFR_RNDU);
    Linear_Form<FP_Interval> lf_vk_upper(inv_vk_upper), lf_d_endp(inv_d_endp); 
    lf_upper += inv_d*(lf_vk - lf_vk_upper) + lf_d_endp;
  }
}

void DP_Form::convertLktoLinearConstraint(MPQ_Interval &ck_bound,
					  Linear_Form<FP_Interval> &lf_lower,
					  Linear_Form<FP_Interval> &lf_upper){
  FP_Interval inv_ck_lower = convertMpqToFPInterval(ck_bound.lower(), MPFR_RNDD);
  FP_Interval inv_ck_upper = convertMpqToFPInterval(ck_bound.upper(), MPFR_RNDU);

  Linear_Form<FP_Interval> lf_ck_lower(inv_ck_lower), lf_ck_upper(inv_ck_upper);
   
  lf_lower += lf_ck_lower;
  lf_upper += lf_ck_upper;
}

FP_Interval DP_Form::convertMpqToFPInterval(mpq_class c, mpfr_rnd_t round_dir){
  mpfr_t mpf_c;
  mpfr_init2 (mpf_c, 24);

  mpfr_set_q(mpf_c, c.get_mpq_t(), round_dir);
  float f_c = mpfr_get_d(mpf_c, round_dir);

  FP_Interval inv_c;
  inv_c.lower() = f_c;
  inv_c.upper() = f_c;

  return inv_c;
}

DP_Form operator+ (const DP_Form& op1, const DP_Form& op2){
  vector<pair<vector<float>, vector<int> > >::const_iterator it = (op2.lk).begin();
  DP_Form r(op1);
  while(it != (op2.lk).end()){
    (r.lk).push_back(*it);
    it ++;
  }
  return r;
}

DP_Form operator- (const DP_Form& op1, const DP_Form& op2){
  vector<float> cons;
  cons.push_back(-1);
  pair<vector<float>, vector<int> > p(cons, vector<int>());
  DP_Form neg_dp = p * op2;
  return op1 + neg_dp;
}

DP_Form operator* (const pair<vector<float>, vector<int> > &p,  const DP_Form& op2){
  DP_Form r(op2);
  vector<pair<vector<float>, vector<int> > >::iterator it = (r.lk).begin();
  while(it != (r.lk).end()){
    vector<float>::const_iterator f_it = (p.first).begin();
    while(f_it != (p.first).end()){
      (*it).first.push_back(*f_it);
      f_it ++;
    }
    vector<int>::const_iterator v_it = (p.second).begin();
    while(v_it != (p.second).end()){
      (*it).second.push_back(*v_it);
      v_it ++;
    }

    it ++;
  }
  return r;
}

