#ifndef POLYNOMIAL
#define POLYNOMIAL

#include <vector>
#include <ppl.hh>

#include "PPL_Manager.h"

using namespace std;
using namespace Parma_Polyhedra_Library;
using namespace Parma_Polyhedra_Library::IO_Operators;

struct Monomial{
  //variable dense representation 2*3*x^2*y^3
  vector<int> m_degree;
  vector<mpq_class> coefficients;

  Monomial(const int dimLen){
    m_degree.resize(dimLen, 0);

  }

  int getSpaceDims() const{
    return m_degree.size();
  }

Monomial(const Monomial &m):m_degree(m.m_degree),
    coefficients(m.coefficients){}
  
};

class Polynomial{
 public:
  NNC_Polyhedron polyhedronApprox(int affine_image_dim){}
  Polynomial(){}//default constructor
  
 Polynomial(float c, const int dimLength):dimLen(dimLength){
    Monomial m(dimLen);
    (m.coefficients).push_back(c);    
    monomial_list.push_back(m);
  }

 Polynomial(int dim, const int dimLength):dimLen(dimLength){    
    Monomial m(dimLen);    
    (m.m_degree)[dim] ++;
    monomial_list.push_back(m);
  }

 Polynomial(const Polynomial &poly):dimLen(poly.dimLen),
    monomial_list(poly.monomial_list){}

  NNC_Polyhedron polyhedronApprox(FP_Interval_Abstract_Store &inv_store, int affine_image_dim);
  mpq_class normalizeAndGetErrorBound(FP_Interval_Abstract_Store &inv_store, Monomial &m);

  mpq_class getMaxAbsoluteBound(const MPQ_Interval &inv);

  mpq_class getGammaN(int n);

  vector<Monomial> monomial_list;
  int dimLen;


  
  friend Polynomial operator+ (const Polynomial &op1, const Polynomial &op2);
  friend Polynomial operator- (const Polynomial &op1, const Polynomial &op2);
  friend Polynomial operator* (const Monomial &m, const Polynomial &op2);


  //convertoLF
  //getError
};

Polynomial operator+ (const Polynomial &op1, const Polynomial &op2);
Polynomial operator- (const Polynomial &op1, const Polynomial &op2);
Polynomial operator* (const Monomial &m, const Polynomial &op2); //m is const monomial

#endif
