#ifndef BERNSTEIN_POLY
#define BERNSTEIN_POLY

#include <iostream>
#include <vector>
#include <cmath>
#include <ppl.hh>
#include <map>

#include "Polynomial.h"
#include "PPL_Manager.h"

using namespace std;
using namespace Parma_Polyhedra_Library;
using namespace Parma_Polyhedra_Library::IO_Operators;

struct MonomialNode{
  map<int, MonomialNode*> m_node;
  mpq_class coefficient;

  ~MonomialNode(){
    map<int, MonomialNode*>::iterator iter = m_node.begin();
    
    while(iter != m_node.end()){
      MonomialNode *ptr = iter->second;
      delete ptr;
      iter ++;
    }
  }
};

class BernsteinPoly{
 public:
  BernsteinPoly(FP_Interval_Abstract_Store &inv_store, Polynomial &p, int dimLength);
  NNC_Polyhedron getApproxPolyhedron(FP_Interval_Abstract_Store &inv_store);

  ~BernsteinPoly(){
    delete root;
  }

 private:
  //changes_bases
  void changeBasis(FP_Interval_Abstract_Store &inv_store,
				  Polynomial &p);
  void changeBasisHelp(FP_Interval_Abstract_Store &inv_store, int dim_index,
		       MonomialNode *root,
		       vector<int> &k_degree,
		       const vector<int> &n_degree);
  
    
  mpq_class getUnitPCoeff(FP_Interval_Abstract_Store &inv_store,
			  MonomialNode *root, const vector<int> &k_degree, const vector<int> &n_degree);
  
  mpq_class getUnitPCoeffHelp(FP_Interval_Abstract_Store &inv_store,
			      MonomialNode *root,
			      int dim_index, vector<int> &i_degree,
			      const vector<int> &k_degree,
			      const vector<int> &n_degree);
  
  void getApproxPolyhedronHelp(int dim_index, vector<int> &i_degree,
			       vector<int> &n_degree, Generator_System &gs);
  
  void buildMonomialTree(MonomialNode *root, Polynomial &poly, vector<int> &n_degree);


  mpq_class getBernsteinCoff(vector<int> &k_degree); //get bernstein coefficient b(n,k)

  mpq_class getBernsteinCoffHelp(int dim_index, vector<int> &i_degree, const vector<int> &k_degree);

  void calDenominatorNDegree();

  Generator buildControlPoint(vector<int> &k_degree);

  mpq_class getBinomialCoff(const vector<int> &i_degree, const vector<int> &k_degree);
  
  mpq_class getPolyCoff(MonomialNode *root, vector<int> &m_degree);

  mpq_class getBernsteinCoff(vector<int> &i_degree, vector<int> &k_degree);
  
  Polynomial unit_p; //Polynomial defined in unit interval [0,1]
  MonomialNode *root; //Monomial Tree used for search
  vector<int> n_degree; //the max degree of the polynomial: n;

  mpz_class denominator;
  int dimLen; //the dimensions of the polyhedron


};
#endif
