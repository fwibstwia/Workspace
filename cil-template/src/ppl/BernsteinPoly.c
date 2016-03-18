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

  //convert the polynomial to bernstein basis
  vector<int> i_degree;
  i_degree.resize(dimLen);
  convertToBernsteinBasis(0, i_degree);

  cout << "after convertion" << endl;
}

void BernsteinPoly::convertToBernsteinBasis(int dim_index, vector<int> &i_degree){
  if(dim_index == dimLen){    
    mpq_class ci = getBernsteinCoff(i_degree);    
    //Here we use the recursion order
    // 0,0,0 -> 0,0,1 -> 0,0,2 -> 0,1,0
    bernsteinCoffs.push_back(ci);     
  } else {
    for(int i = 0; i <= n_degree[dim_index]; i ++){
      i_degree[dim_index] = i;
      convertToBernsteinBasis(dim_index + 1, i_degree);
    }
  }
}

NNC_Polyhedron BernsteinPoly::getApproxPolyhedron(){

  Generator_System gs;
  vector<mpq_class> bernsteinCoffsEDegree;

  vector<int> e_degree, n_e_degree;
  e_degree.resize(dimLen);
  n_e_degree.resize(dimLen);

  for(int i = 0; i < dimLen; i ++){
    e_degree[i] = 1; // increase the degree by one
    n_e_degree[i] = n_degree[i] + e_degree[i];
  }
  
  vector<int> k_degree;
  k_degree.resize(dimLen);
  
  degreeElevation(0, k_degree, e_degree, bernsteinCoffsEDegree);
  cout << "after elevation" << endl;
  
  int coff_index = 0;
  buildControlPoints(gs, k_degree, n_e_degree,
		     bernsteinCoffsEDegree,
		     0, coff_index);  
 
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
  buildControlPoints - construct bernstein polyhedron using bernstein coefficients
  n_e_degree - the bernstein polynomial degrees after elevation of e degrees;
  coff_index - int reference, used to find the bernstein coefficient of k_degree,
               the order in bernstenCoffs is the recursion order.
 */
void BernsteinPoly::buildControlPoints(Generator_System &gs,
				       vector<int> &k_degree,
				       vector<int> &n_e_degree,
				       vector<mpq_class> &bernsteinCoffsEDegree,
				       int dim_index,
				       int &coff_index){
  if(dim_index == dimLen){
    mpq_class ci = bernsteinCoffsEDegree[coff_index];

    mpz_class denominator = ci.get_den();

    for(int i = 0; i < dimLen; i++){
      if(n_e_degree[i] != 0){
	denominator = denominator*n_e_degree[i];
      }
    }
  
    mpq_class ci_int = ci * denominator;
    ci_int.canonicalize();
    assert(ci_int.get_den() == 1);
    mpz_class ci_z = ci_int.get_num(); 
  
    Linear_Expression le;
  
    for(int j = 0; j < dimLen; j ++){
      mpq_class kj_nj (k_degree[j], n_e_degree[j]); // ij/nj
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
    gs.insert(point(le, denominator));

    coff_index ++;
    return;
  } else {
    for(int i = 0; i <= n_e_degree[dim_index]; i++){
      k_degree[dim_index] = i;
      buildControlPoints(gs, k_degree, n_e_degree,
			 bernsteinCoffsEDegree, dim_index + 1, coff_index);
    }
    
    return;
  }
}
/*
  degreeElevation - increase the degree of bernstein polynomials; 
                    thus increase the precision of the approximation
  e_degree - store the degrees to increase for each dimension
  bernsteinCoffsEDegree - stores the bernstein coefficients after elevation
 */
void BernsteinPoly::degreeElevation(int dim_index,
		     vector<int> &k_degree,
		     vector<int> &e_degree,
		     vector<mpq_class> &bernsteinCoffsEDegree){
  if(dim_index == dimLen){
    vector<int> l_degree;
    l_degree.resize(dimLen);
    bernsteinCoffsEDegree.push_back(degreeElevationLSet(0, l_degree, k_degree, e_degree));
  } else {
    for(int i = 0; i <= e_degree[dim_index] + n_degree[dim_index]; i ++){
      k_degree[dim_index] = i;
      degreeElevation(dim_index + 1, k_degree, e_degree, bernsteinCoffsEDegree);
    }
  }
}

/*
 degreeElevationLSet - calculate the sum of bernstein coefficients in the LSet.
 */
mpq_class BernsteinPoly::degreeElevationLSet(int dim_index,
			 vector<int> &l_degree,
			 vector<int> &k_degree,
			 vector<int> &e_degree){
  cout << "enterElevationLSet" << endl;
  
  mpq_class r(0);
  if(dim_index == dimLen){
    mpq_class ci = bernsteinCoffs[getLDegreeCoffIndex(l_degree)];
    if(ci != 0){
      r = ci * getBinCoffCiElevate(l_degree, k_degree, e_degree);
    }
    return r;  
  } else {
    int max_k_e = k_degree[dim_index] - e_degree[dim_index]; //max(0, K-E)
    if(max_k_e < 0){
      max_k_e = 0;
    }

    int min_n_k = k_degree[dim_index]; //min(N, K)
    if(min_n_k > n_degree[dim_index]){
      min_n_k = n_degree[dim_index];
    }

    for(int i = max_k_e; i <= min_n_k; i ++){
      l_degree[dim_index] = i;
      r += degreeElevationLSet(dim_index + 1, l_degree, k_degree, e_degree);
    }
    return r;
  }  
}

/*
  getLDegreeCoffIndex - get the index corresponding with the l degree
   [3, 2, 3] 011 = 1 * 4 + 1 * 1 = 6
 */
int BernsteinPoly::getLDegreeCoffIndex(vector<int> &l_degree){

  int index = 0;
  cout << "enterElevationLSetIndex" << endl;
  for(int i = dimLen - 1; i >= 0; i --){
    int place = 1;
    for(int j = i + 1; j < dimLen; j ++){
      place = place * (n_degree[j] + 1);
    }
    
    index = index + l_degree[i]*place;
  }
  
  return index;
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
      r = ci * getBinCoffCi(i_degree, k_degree);
    }
    return r;
  }

  for(int i = 0; i <= k_degree[dim_index]; i++){
    i_degree[dim_index] = i;
    r += getBernsteinCoffHelp(dim_index + 1, i_degree, k_degree);
  }
  return r;
}


/*
  getBinCoffCi - calculate the binomials of ci
 */
mpq_class BernsteinPoly::getBinCoffCi(const vector<int> &i_degree,
				      const vector<int> &k_degree){
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

/*
 getBinCoffCiElevate - calculate the binomials of ci for degree elevation
 */

mpq_class BernsteinPoly::getBinCoffCiElevate(const vector<int> &l_degree, 
					     const vector<int> &k_degree,
					     const vector<int> &e_degree){
  mpq_class r(1);
  for(int j = 0; j < dimLen; j ++){
    mpz_class rop_n_l, rop_e_k_l, rop_n_e_k;
    mpz_class n_e_degree_z(n_degree[j] + e_degree[j]), n_degree_z(n_degree[j]),
      e_degree_z(e_degree[j]);
    
    mpz_bin_ui(rop_n_l.get_mpz_t(), n_degree_z.get_mpz_t(), l_degree[j]);
    mpz_bin_ui(rop_e_k_l.get_mpz_t(), e_degree_z.get_mpz_t(), k_degree[j] - l_degree[j]);
    mpz_bin_ui(rop_n_e_k.get_mpz_t(), n_e_degree_z.get_mpz_t(), k_degree[j]);

    mpq_class q(mpz_class(rop_n_l * rop_e_k_l).get_ui(), mpz_class(rop_n_e_k).get_ui());
    r = r * q;   
  }
  return r;
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
