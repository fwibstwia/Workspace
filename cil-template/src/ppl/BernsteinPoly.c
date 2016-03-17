#include <iostream>
#include <cassert>

#include "BernsteinPoly.h"

using namespace std;
using namespace Parma_Polyhedra_Library;
using namespace Parma_Polyhedra_Library::IO_Operators;

typedef pair<int, MonomialNode*> Pair;

BernsteinPoly::BernsteinPoly(FP_Interval_Abstract_Store &store,
			     Polynomial &p,
			     int dimLength):dimLen(dimLength), inv_store(store){
  unit_p.dimLen = dimLength;
  n_degree.resize(dimLength, 0);
  root = new MonomialNode();
  changeBasis(p);
  buildMonomialTree(root, unit_p, n_degree);
}

NNC_Polyhedron BernsteinPoly::getApproxPolyhedron(){

  Generator_System gs;
  vector<int> i_degree;
  i_degree.resize(dimLen);
  getApproxPolyhedronHelp(0, i_degree, n_degree, gs);

  NNC_Polyhedron nnc_p(gs);


  
  Variables_Set vs;
  for(int j = 0; j < dimLen; j ++){
    if (n_degree[j] == 0){
      vs.insert(Variable(j));
    }
  }
  nnc_p.unconstrain(vs);
  
  for(int j = 0; j < dimLen; j ++){
    if(n_degree[j] == 0){
      continue;
    }
    FP_Interval v_bound = inv_store.get_interval(Variable(j));
    mpq_class a = v_bound.lower();
    mpz_class a_den = a.get_den();
    mpq_class b = v_bound.upper();
    mpz_class b_den = b.get_den();
    mpq_class a_z = a * a_den * b_den;
    mpq_class b_z = b * a_den * b_den;
    a_z.canonicalize();
    b_z.canonicalize();
    mpz_class a_z_num = a_z.get_num();
    mpz_class b_z_num = b_z.get_num();

    nnc_p.generalized_affine_image(Variable(j), EQUAL,
				   a_z_num + (b_z_num-a_z_num)*Variable(j),
				   a_den * b_den);
  }
  
  return nnc_p;
}

/*
 change the domain of the polynomial to [0,1] unit domain
 */
void BernsteinPoly::changeBasis(Polynomial &p){
  vector<int> n_degree, k_degree;
  n_degree.resize(dimLen);
  k_degree.resize(dimLen);
  MonomialNode *root = new MonomialNode();
  buildMonomialTree(root, p, n_degree);
  changeBasisHelp(0, root, k_degree, n_degree);
  delete root;
}

void BernsteinPoly::changeBasisHelp(int dim_index,
				    MonomialNode *root,
				    vector<int> &k_degree,
				    const vector<int> &n_degree){
 
  if(dim_index == dimLen){
    Monomial m(dimLen);
    for(int j = 0; j < dimLen; j ++){
      m.m_degree[j] = k_degree[j];
    }
    mpq_class coff = getUnitPCoeff(root, k_degree, n_degree);
    m.coefficients.push_back(coff);
    unit_p.monomial_list.push_back(m);
    
  }else{    
    for(int i = 0; i <= n_degree[dim_index]; i++){
      k_degree[dim_index] = i;
      changeBasisHelp(dim_index + 1, root, k_degree, n_degree);
    }    
  }
}

mpq_class BernsteinPoly::getUnitPCoeff(MonomialNode *root,
				       const vector<int> &k_degree,
				       const vector<int> &n_degree){
  vector<int> i_degree;
  i_degree.resize(dimLen);
  return getUnitPCoeffHelp(root, 0, i_degree, k_degree, n_degree);
}

mpq_class BernsteinPoly::getUnitPCoeffHelp(MonomialNode *root,
					   int dim_index, vector<int> &i_degree,
					   const vector<int> &k_degree,
					   const vector<int> &n_degree){
  mpq_class r(0);
  if(dim_index == dimLen){
  
    mpq_class ci = getPolyCoff(root, i_degree);
    
    if(ci != 0){
      for(int j = 0; j < dimLen; j ++){
        if(n_degree[j] == 0){
	  continue;
	}
	mpz_class rop_i_k;
	mpz_class i_degree_z(i_degree[j]);

	mpz_bin_ui(rop_i_k.get_mpz_t(), i_degree_z.get_mpz_t(), k_degree[j]);
        ci = ci * (rop_i_k.get_ui());

	MPQ_Interval inv_v;
	FP_Interval v_bound = inv_store.get_interval(Variable(j));
	inv_v.lower() = v_bound.lower();
	inv_v.upper() = v_bound.upper();
	
	for(int e = 0; e < k_degree[j]; e ++){
	  ci = ci * (inv_v.upper() - inv_v.lower());  
	}

	for(int e = 0; e < i_degree[j] - k_degree[j]; e ++){
	  ci = ci * (inv_v.lower());
	}

      }
      r = ci;

    }
    return r;
  }
  
  for(int i = k_degree[dim_index]; i <= n_degree[dim_index]; i++){
    i_degree[dim_index] = i;
    r += getUnitPCoeffHelp(root, dim_index + 1, i_degree, k_degree, n_degree);
  }
  return r;
}



void BernsteinPoly::getApproxPolyhedronHelp(int dim_index, vector<int> &i_degree,
					    vector<int> &n_degree, Generator_System &gs){
  if(dim_index == dimLen){
    gs.insert(buildControlPoint(i_degree));
  } else{
    for(int i = 0; i <= n_degree[dim_index]; i ++){
      i_degree[dim_index] = i;
      getApproxPolyhedronHelp(dim_index + 1, i_degree, n_degree, gs);
    }
  }
}

void BernsteinPoly::buildMonomialTree(MonomialNode *root, Polynomial &poly, vector<int> &n_degree){

  vector<Monomial>::iterator iter = poly.monomial_list.begin();
  MonomialNode *ptr_node;
  
  while(iter != poly.monomial_list.end()){
    //we use the dense variable representation, start from the root
    ptr_node = root;

    for(int i = 0; i < poly.dimLen; i ++){
      //update max degrees of the polynomial
      if(n_degree[i] < iter -> m_degree[i]){
	n_degree[i] = iter -> m_degree[i];
      }
     
      map<int, MonomialNode*>::iterator node_iter = (ptr_node -> m_node).find(iter -> m_degree[i]);
									      
      if(node_iter == (ptr_node -> m_node).end()){
	MonomialNode *new_node = new MonomialNode();
	(ptr_node -> m_node).insert(Pair(iter -> m_degree[i], new_node));
	ptr_node = new_node;
      }else{
	ptr_node = node_iter->second;
      }
    }

    /*the leafnode is the coefficient of the monomial, the coefficient of 
      the unit polynomial has been simplified to single value
     */
    ptr_node -> coefficient = iter -> coefficients[0];

    //go to the next monomial of the polynomial
    iter ++;
  }
}

mpq_class BernsteinPoly::getPolyCoff(MonomialNode *root, vector<int> &m_degree){
  MonomialNode *ptr_node = root;
  for(int i = 0; i < dimLen; i ++){
    map<int, MonomialNode*>::iterator node_iter = (ptr_node -> m_node).find(m_degree[i]);
    if(node_iter == (ptr_node -> m_node).end()){
      return mpq_class(0);
    }else{
      ptr_node = node_iter -> second;
    }
  }
  return ptr_node -> coefficient;
}

mpq_class BernsteinPoly::getBinomialCoff(const vector<int> &i_degree, const vector<int> &k_degree){
  mpq_class r(1);

  for(int j = 0; j < dimLen; j ++){
    //mpz_bin_ui(mpz_t rop, const mpz_t n, unsigned long int k)
    //compute the binomial coefficient n over k and store the result in rop.
    mpz_class rop_k_i, rop_n_i;
    mpz_class k_degree_z(k_degree[j]), n_degree_z(n_degree[j]);
    mpz_bin_ui(rop_k_i.get_mpz_t(), k_degree_z.get_mpz_t(), i_degree[j]);
    mpz_bin_ui(rop_n_i.get_mpz_t(), n_degree_z.get_mpz_t(), i_degree[j]);
    mpq_class ki_div_ni(mpz_class(rop_k_i).get_ui(), mpz_class(rop_n_i).get_ui());
    r = r * ki_div_ni;
  }

  return r;
}

mpq_class BernsteinPoly::getBernsteinCoff(vector<int> &k_degree){
  vector<int> i_degree;
  i_degree.resize(dimLen);
  return getBernsteinCoffHelp(0, i_degree, k_degree);
}

mpq_class BernsteinPoly::getBernsteinCoffHelp(int dim_index, vector<int>  &i_degree, const vector<int> &k_degree){
  mpq_class r(0);
  //cout << "PolyCoff " << i_degree[0] << i_degree[1] << i_degree[2] << i_degree[3] << " " << ci << endl;
  if(dim_index == dimLen){
    mpq_class ci = getPolyCoff(root, i_degree);
    if(ci != 0){
      r = ci * getBinomialCoff(i_degree, k_degree);
    }
    return r;
  }

  for(int i = 0; i <= k_degree[dim_index]; i++){
    i_degree[dim_index] = i;
    r += getBernsteinCoffHelp(dim_index + 1, i_degree, k_degree);
  }
  return r;
}

Generator BernsteinPoly::buildControlPoint(vector<int> &k_degree){
  mpq_class ci = getBernsteinCoff(k_degree);

  mpz_class denominator = ci.get_den();
  for(int i = 0; i < dimLen; i++){
    if(n_degree[i] != 0){
      denominator = denominator*n_degree[i];
    }
  }
  
  mpq_class ci_int = ci * denominator;
  ci_int.canonicalize();
  assert(ci_int.get_den() == 1);
  mpz_class ci_z = ci_int.get_num(); 
  
  Linear_Expression le;
  
  for(int j = 0; j < dimLen; j ++){
    mpq_class kj_nj (k_degree[j], n_degree[j]); // ij/nj
    kj_nj *= denominator;
    kj_nj.canonicalize();
    mpz_class kj_nj_z = kj_nj.get_num(); 
    assert(kj_nj.get_den() == 1);
    Variable v(j);
    le = le + kj_nj_z * v;
  }

  Variable v(dimLen);
  le += ci_z * v;
  //cout << "point" << point(le, denominator) << endl;
  return point(le, denominator);
}




