#include <iostream>
#include "BernsteinPoly.h"

using namespace std;
using namespace Parma_Polyhedra_Library;
using namespace Parma_Polyhedra_Library::IO_Operators;

typedef pair<int, MonomialNode*> Pair;

BernsteinPoly::BernsteinPoly(FP_Interval_Abstract_Store &inv_store, Polynomial &p, int dimLength):dimLen(dimLength){
  unit_p.dimLen = dimLength;
  changeBasis(inv_store, p);
  //unit_p = p;  
  n_degree.resize(dimLength, 0);
  root = new MonomialNode();
  buildMonomialTree(root, unit_p, n_degree);
  calDenominatorNDegree();
}

NNC_Polyhedron BernsteinPoly::getApproxPolyhedron(FP_Interval_Abstract_Store &inv_store){

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
    int a = v_bound.lower();
    int b = v_bound.upper();
    nnc_p.affine_image(Variable(j), a + (b-a)*Variable(j));
  }
  cout << "poly is: " << nnc_p << endl;  
  return nnc_p;
}


void BernsteinPoly::changeBasis(FP_Interval_Abstract_Store &inv_store,
			   Polynomial &p){
  vector<int> n_degree, k_degree;
  n_degree.resize(dimLen);
  k_degree.resize(dimLen);
  MonomialNode *root = new MonomialNode();
  buildMonomialTree(root, p, n_degree);
  
  for(int j = 0; j < dimLen; j ++){
    cout << "----------n_degree" << n_degree[j];
  }
  cout << endl;
  
  changeBasisHelp(inv_store, 0, root, k_degree, n_degree);
  delete root;
}

void BernsteinPoly::changeBasisHelp(FP_Interval_Abstract_Store &inv_store, int dim_index,
				     MonomialNode *root,
				    vector<int> &k_degree,
				    const vector<int> &n_degree){
 
  if(dim_index == dimLen){
    Monomial m(dimLen);
    for(int j = 0; j < dimLen; j ++){
      m.m_degree[j] = k_degree[j];
    }
    mpq_class coff = getUnitPCoeff(inv_store, root, k_degree, n_degree);
    cout << "#####enter change basis help " << coff << endl;
    m.coefficients.push_back(coff);
    unit_p.monomial_list.push_back(m);
    
  }else{    
    for(int i = 0; i <= n_degree[dim_index]; i++){
      k_degree[dim_index] = i;
      changeBasisHelp(inv_store, dim_index + 1, root, k_degree, n_degree);
    }    
  }
}

mpq_class BernsteinPoly::getUnitPCoeff(FP_Interval_Abstract_Store &inv_store,
				       MonomialNode *root, const vector<int> &k_degree, const vector<int> &n_degree){
  vector<int> i_degree;
  i_degree.resize(dimLen);
  return getUnitPCoeffHelp(inv_store, root, 0, i_degree, k_degree, n_degree);
}

mpq_class BernsteinPoly::getUnitPCoeffHelp(FP_Interval_Abstract_Store &inv_store,
					 MonomialNode *root,
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
        cout << "enter ci " << i_degree[j] << " " << k_degree[j] << endl;
	mpz_bin_ui(rop_i_k.get_mpz_t(), i_degree_z.get_mpz_t(), k_degree[j]);
        ci = ci * (rop_i_k.get_ui());
	cout << "ci bino: " << ci << endl;
	MPQ_Interval inv_v;
	FP_Interval v_bound = inv_store.get_interval(Variable(j));
	inv_v.lower() = v_bound.lower();
	inv_v.upper() = v_bound.upper();
	
	for(int e = 0; e < k_degree[j]; e ++){
	  ci = ci * (inv_v.upper() - inv_v.lower());  
	}
	cout << "ci inv: " << ci << endl;
	for(int e = 0; e < i_degree[j] - k_degree[j]; e ++){
	  ci = ci * (inv_v.lower());
	}
	cout << "ci lower: " << ci << endl;
      }
      r = ci;
      cout << "exit coef if" << ci << endl;
    }
    return r;
  }
  
  for(int i = k_degree[dim_index]; i <= n_degree[dim_index]; i++){
    i_degree[dim_index] = i;
    r += getUnitPCoeffHelp(inv_store, root, dim_index + 1, i_degree, k_degree, n_degree);
  }
  return r;
}



void BernsteinPoly::getApproxPolyhedronHelp(int dim_index, vector<int> &i_degree, vector<int> &n_degree, Generator_System &gs){
  if(dim_index == dimLen){
    gs.insert(buildControlPoint(i_degree));
  } else{
    for(int i = 0; i <= n_degree[dim_index]; i ++){
      i_degree[dim_index] = i;
      getApproxPolyhedronHelp(dim_index + 1, i_degree, n_degree, gs);
    }
  }
}


void BernsteinPoly::calDenominatorNDegree(){
  denominator = 1;
  for(int i = 0; i < dimLen; i++){
    if(n_degree[i] != 0){
      denominator = denominator*n_degree[i];
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

//TODO: need to think ci is float
Generator BernsteinPoly::buildControlPoint(vector<int> &k_degree){
  mpq_class ci = getBernsteinCoff(k_degree) * denominator;
  ci.canonicalize();
  mpz_class ci_z = ci.get_num(); // denominator should be 1
  
  Linear_Expression le;
  
  for(int j = 0; j < dimLen; j ++){
    mpq_class kj_nj (k_degree[j], n_degree[j]); // ij/nj
    kj_nj *= denominator;
    kj_nj.canonicalize();
    mpz_class kj_nj_z = kj_nj.get_num(); //denominator should be 1
    Variable v(j);
    le = le + kj_nj_z * v;
  }

  Variable v(dimLen);
  le += ci_z * v;
  cout << "point" << point(le, denominator) << endl;
  return point(le, denominator);
}




