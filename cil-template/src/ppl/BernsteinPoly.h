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
  NNC_Polyhedron getApproxPolyhedron();

  ~BernsteinPoly(){
    delete root;
  }

 private:
  void convertToBernsteinBasis(int dim_index, vector<int> &i_degree);

  void buildControlPoints(Generator_System &gs,
			  vector<int> &k_degree,
			  vector<int> &n_e_degree,
			  vector<mpq_class> &bernsteinCoffsEDegree,
			  int dim_index,
			  int &coff_index);
  
  void degreeElevation(int dim_index,
		       vector<int> &k_degree,
		       vector<int> &e_degree,
		       vector<mpq_class> &bernsteinCoffsEDegree);

  mpq_class degreeElevationLSet(int dim_index,
				vector<int> &l_degree,
				vector<int> &k_degree,
				vector<int> &e_degree);

  int getLDegreeCoffIndex(vector<int> &l_degree);

  mpq_class getBernsteinCoff(vector<int> &k_degree); //get bernstein coefficient b(n,k)

  mpq_class getBernsteinCoffHelp(int dim_index,
				 vector<int> &i_degree,
				 const vector<int> &k_degree);

  mpq_class getBinCoffCi(const vector<int> &i_degree,
			 const vector<int> &k_degree);
  
  mpq_class getBinCoffCiElevate(const vector<int> &l_degree, 
				const vector<int> &k_degree,
				const vector<int> &e_degree);
  mpq_class getPolyCoff(MonomialNode *root, vector<int> &m_degree);
  
  void buildMonomialTree(MonomialNode *root, Polynomial &poly, vector<int> &n_degree);
  //changes_bases
  void changeBasis(Polynomial &p);
  void changeBasisHelp(int dim_index,
		       MonomialNode *root,
		       vector<int> &k_degree,
		       const vector<int> &n_degree);
  
    
  mpq_class getUnitPCoeff(MonomialNode *root, const vector<int> &k_degree,
			  const vector<int> &n_degree);
  
  mpq_class getUnitPCoeffHelp(MonomialNode *root,
			      int dim_index, vector<int> &i_degree,
			      const vector<int> &k_degree,
			      const vector<int> &n_degree);
  
  Polynomial unit_p; //Polynomial defined in unit interval [0,1]
  MonomialNode *root; //Monomial Tree used for search
  vector<int> n_degree; //the max degree of the polynomial: n;
  FP_Interval_Abstract_Store &inv_store;
  vector<mpq_class> bernsteinCoffs;
  int dimLen; //the dimensions of the polyhedron
};
#endif
