#ifndef DP_FORM
#define DP_FORM
#include <vector>
#include <cmath>
#include <mpfr.h>

#include "C_Expr_defs.h"
#include "PPL_Manager.h"

using namespace std;
using namespace Parma_Polyhedra_Library;

/* DP_Form: a_0 * a_1 * b_1 + a_2 * b_2 + ... + a_n * b_n 
   Lk:a_k * b_k
   parenthesis free
*/
class DP_Form{
 public:
  void convertToLC(PPL_Manager *manager, FP_Interval_Abstract_Store &inv_store,
		   Linear_Form<FP_Interval> &lower, Linear_Form<FP_Interval> &upper,
		   int affine_image_vid); //intervalize DP_Form

  pair<vector<float>, vector<int> > getLk(){
    return *(lk.begin());
  }
  
  DP_Form(float c){
    vector<float> clist;
    clist.push_back(c);
    pair<vector<float>, vector<int> > p(clist, vector<int>());
    lk.push_back(p);
  }

  DP_Form(int vid){
    vector<int> vidlist;
    vidlist.push_back(vid);
    pair<vector<float>, vector<int> > p(vector<float>(), vidlist);
    lk.push_back(p);
  }
  
 DP_Form(const DP_Form &df):lk(df.lk){       }

 private:
  vector<pair<vector<float>, vector<int> > > lk;

  mpq_class convertLktoLinearForm(PPL_Manager *manager, FP_Interval_Abstract_Store &inv_store,
				  const vector<float> &lk_cons, const vector<int> &lk_var_ids,
				  MPQ_Interval &ck_bound, int *vk_id, int affine_image_vid);

  MPQ_Interval MPQInvMult(const MPQ_Interval &op1, const MPQ_Interval &op2);

  mpq_class getGammaN(int n);

  mpq_class getMaxAbsoluteBound(const MPQ_Interval &inv);

  void convertLktoLinearConstraint(MPQ_Interval &ck_bound, MPQ_Interval &vk_bound, Variable &vk,
		 Linear_Form<FP_Interval> &lf_lower, Linear_Form<FP_Interval> &lf_upper);

  void convertLktoLinearConstraint(MPQ_Interval &ck_bound, Linear_Form<FP_Interval> &lf_lower,
		 Linear_Form<FP_Interval> &lf_upper);

  FP_Interval convertMpqToFPInterval(mpq_class c, mpfr_rnd_t round_dir);
  
  friend DP_Form operator+ (const DP_Form& op1, const DP_Form& op2);
  friend DP_Form operator- (const DP_Form& op1, const DP_Form& op2);
  friend DP_Form operator* (const pair<vector<float>, vector<int> > &p, const DP_Form& op2);
};


DP_Form operator+ (const DP_Form& op1, const DP_Form& op2);
DP_Form operator- (const DP_Form& op1, const DP_Form& op2);
DP_Form operator* (const pair<vector<float>, vector<int> > &p, const DP_Form& op2);


#endif
