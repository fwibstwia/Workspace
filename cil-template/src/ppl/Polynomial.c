#include <iomanip>
#include <fenv.h>
#include <iostream>

#include "Polynomial.h"
#include "BernsteinPoly.h"

using namespace std;
using namespace Parma_Polyhedra_Library;

/*
  Using polyhedron to approximate expression: A = B*C + D
 */
NNC_Polyhedron Polynomial::polyhedronApprox(FP_Interval_Abstract_Store &inv_store, int affine_image_dim){

  int n = 0;
  mpq_class error;
  vector<Monomial>::iterator it = monomial_list.begin();
  while(it != monomial_list.end()){
    error += normalizeAndGetErrorBound(inv_store, (*it));
    n ++;
    it ++;
  }
  
  BernsteinPoly bp(inv_store, *this, dimLen);
  NNC_Polyhedron nnc_p = bp.getApproxPolyhedron();
  Variable f(dimLen), v(affine_image_dim);
  nnc_p.add_constraint(v - f == 0);
  nnc_p.unconstrain(f);
  Variables_Set vs(f);
  nnc_p.remove_space_dimensions(vs);
  return nnc_p;
  //error = error * getGammaN(n);
  //MPQ_Interval error_bound;
  // error_bound.lower() = - error;
  //  error_bound.upper() = error;
}

/*
  get the error bound for each monomials of the polynomials
 */
mpq_class Polynomial::normalizeAndGetErrorBound(FP_Interval_Abstract_Store &inv_store, Monomial &m){
  //caculate the error bound for multiplication, without considering underflow
 
  int n = 0;
  mpq_class error(1);
  mpq_class coeff_merger(1);

  vector<mpq_class>::const_iterator cons_it = (m.coefficients).begin();
  MPQ_Interval inv_c;
  while(cons_it != (m.coefficients).end()){
    error = error * abs(*cons_it);
    coeff_merger = coeff_merger * (*cons_it);
    n ++;
    cons_it ++;
  }
  (m.coefficients).clear();
  
  if(m.coefficients.size() == 0){
    m.coefficients.push_back(coeff_merger);
  }else{
    (m.coefficients)[0] = coeff_merger; //merge all the constants multiplication
  }
    
  MPQ_Interval inv_v;
  for(int j = 0; j < m.getSpaceDims(); j ++){
    for(int i = 0; i < (m.m_degree)[j]; i ++){
      Variable v(j);
      FP_Interval v_bound = inv_store.get_interval(v);
      inv_v.lower() = v_bound.lower();
      inv_v.upper() = v_bound.upper();
      error = error * getMaxAbsoluteBound(inv_v);
    }
  }

  error = error * (1 + getGammaN(n));
  return error; 
  
}

/*
 Given an interval, get the absolute maximum value of the end point
 */
mpq_class Polynomial::getMaxAbsoluteBound(const MPQ_Interval &inv){
  mpq_class mpq_lower (abs(inv.lower()));
  mpq_class mpq_upper (abs(inv.upper()));
  if(mpq_lower > mpq_upper)
    return mpq_lower;
  else
    return mpq_upper;
}

mpq_class Polynomial::getGammaN(int n){
  mpq_class num_limit = numeric_limits<float>::denorm_min();
  return (n-1) * num_limit /(1-(n-1)*num_limit);
}


Polynomial operator+ (const Polynomial &op1, const Polynomial &op2){
  vector<Monomial>::const_iterator it = (op2.monomial_list).begin();
  Polynomial r(op1);
  while(it != (op2.monomial_list).end()){
    (r.monomial_list).push_back(*it);
    it ++;
  }
  return r;
}

Polynomial operator- (const Polynomial &op1, const Polynomial &op2){
  Monomial m (op1.dimLen);
  (m.coefficients).push_back(-1);
  Polynomial neg_dp = m * op2;
  
  return op1 + neg_dp;
}

Polynomial operator* (const Monomial &m,  const Polynomial &op2){
  Polynomial r(op2);
  vector<Monomial>::iterator it = (r.monomial_list).begin();  
  while(it != (r.monomial_list).end()){
    //for constants
    vector<mpq_class>::const_iterator mpq_it = (m.coefficients).begin();  
    while(mpq_it != (m.coefficients).end()){      
      (*it).coefficients.push_back(*mpq_it);
      mpq_it ++;
    }
    //for variables
    for(int i = 0; i < m.getSpaceDims(); i++){
      (*it).m_degree[i] += m.m_degree[i];
    }    
    it ++;
  }  
  return r;
}
