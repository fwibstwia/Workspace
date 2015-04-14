#include <vector>
#include <sstream>
#include <cstdlib>
#include <math.h>
#include <iomanip>
#include <string>
#include <exception>      // std::exception

#include <cmath>
#include <immintrin.h>

#include "z3++.h"
#include "Reorder.h"
#include "Mandelbrot.h"

using namespace z3;
using namespace std;

string getBound(const float v, const int dir)
{
  float boundf;
  std::ostringstream out;
  boundf = nextafterf(v, v + dir);
  out << fixed << setprecision(9) << boundf;
  return out.str();
}

float getModelVal(context &c, model &m, expr &r){
 char *stopString;
 z3::expr rl = m.get_const_interp(r.decl());
 Z3_string sl = Z3_get_numeral_decimal_string(c, rl,50);
 return strtof(sl, &stopString);
}

double getModelVal_d(context &c, model &m, expr &r){
 char *stopString;
 z3::expr rl = m.get_const_interp(r.decl());
 Z3_string sl = Z3_get_numeral_decimal_string(c, rl,50);
 return strtod(sl, &stopString);
}

expr getBlockClause(float v, expr &e, context &c){
  string nf_s = getBound(v, 1);
  string pf_s = getBound(v, -1);
  expr nf_e = c.real_val(nf_s.c_str());
  expr pf_e = c.real_val(pf_s.c_str());
  return e < pf_e || e > nf_e;
}

double mandelbrot_fma(double x0, double y0, double windowPosX, double windowPosY, double stepSize){
  __m128d a, b, c, r;
  a[0] = x0;
  a[1] = y0;
  b[0] = windowPosX;
  b[1] = -windowPosY;
  c[0] = stepSize;
  c[1] = stepSize;
  r = _mm_fmadd_pd(b, c, a);
  
  double stepPosX = r[0];
  double stepPosY = r[1];

  double stepPosX_sqr = stepPosX * stepPosX;
  double stepPosY_sqr = stepPosY * stepPosY;

  double x =  stepPosX_sqr - stepPosY_sqr + stepPosX;
  double y =  2 * stepPosX * stepPosY + stepPosY;

  double x_sqr = x*x;
  double y_sqr = y*y;

  return x_sqr + y_sqr;
}


void mandelbrot(){

  bool terminate = false; 
  int  iter_count = 0;


  context c;
  expr x = c.real_const("x");
  expr y = c.real_const("y");
  expr stepPosX = c.real_const("stepPosX");
  expr stepPosY = c.real_const("stepPosY");
  expr x0 = c.real_const("x0");
  expr y0 = c.real_const("y0");
  expr windowPosX = c.real_const("windowPosX");
  expr windowPosY = c.real_const("windowPosY");
  expr stepSize = c.real_const("stepSize");

  solver s(c);
  //params p(c);
  //p.set("shuffle_vars", true);

  //p.set("random_freq", 1.0); 
  //p.set("random_seed", (unsigned)1024) ;
  //s.set(p);
  
  s.add(stepPosX == x0 + windowPosX * stepSize);
  s.add(stepPosY == y0 - windowPosY * stepSize);
  s.add(y == 2 * stepPosX * stepPosY + stepPosY);
  s.add(x == stepPosX * stepPosX - stepPosY * stepPosY + stepPosX);
  s.add(100.0 == x*x + y*y);
  s.add(x0 > 0);
  s.add(y0 > 0);
  s.add(stepSize > 0);
  while(!terminate){
    s.check();
    model m(c, s.get_model());
    double x0_v = getModelVal_d(c, m, x0);
    double y0_v = getModelVal_d(c, m, y0);
    double windowPosX_v = getModelVal_d(c, m, windowPosX);
    double windowPosY_v = getModelVal_d(c, m, windowPosY);
    double stepSize_v = getModelVal_d(c, m, stepSize);

    cout << "var_v: " << setprecision(17) 
	 << x0_v << " " 
	 << y0_v << " "
	 << windowPosX_v << " " 
	 << windowPosY_v << " " 
	 << stepSize_v << " " 
	 << endl; 

    double fmar = mandelbrot_fma(x0_v, y0_v, windowPosX_v, windowPosY_v, stepSize_v);
    double nonfmar = mandelbrot_nonfma(x0_v, y0_v, windowPosX_v, windowPosY_v, stepSize_v);
    
    cout << setprecision(17) << fmar << " " << nonfmar << endl;
    if(fmar != nonfmar){
      break;
    }

    s.add(getBlockClause(x0_v, x0, c));
    s.add(getBlockClause(y0_v, y0, c));
    s.add(getBlockClause(windowPosX_v, windowPosX, c));
    s.add(getBlockClause(windowPosY_v, windowPosY, c));
    s.add(getBlockClause(stepSize_v, stepSize, c));
  }
}

void ray_tracing(){

  bool terminate = false; 
  int  iter_count = 0;

  context c;
  //c.set("nlsat.shuffle_vars", true);
  //c.set("nlsat.factor", false);
  //c.set("nlsat.randomize", false);
  //c.set("sat.random_freq", "0.1");

  Reorder ro(FE_TONEAREST);

  expr rx = c.real_const("rx");
  expr ry = c.real_const("ry");
  expr rz = c.real_const("rz");
  expr sx = c.real_const("sx");
  expr sy = c.real_const("sy");
  expr sz = c.real_const("sz");
  expr radius = c.real_const("radius");
  expr A = c.real_const("A");
  expr B = c.real_const("A");
  expr C = c.real_const("C");
  expr D = c.real_const("D");
  expr two = c.real_val("-2.0");
  expr four = c.real_val("4.0");



  solver s(c);
  
  params p(c);
  p.set("shuffle_vars", true);

  //p.set("random_freq", 1.0); 
  //p.set("random_seed", (unsigned)1024) ;
  s.set(p);
  
  s.add(0.125 == (two*(sx*rx+sy*ry+sz*rz))*(two*(sx*rx+sy*ry+sz*rz)) - four * (rx*rx+ry*ry+rz*rz) * (sx*sx+sy*sy+sz*sz - radius));
  s.add(rx > 9.720005035400391);
  s.add(rx < 9.820005416870117);
  while(!terminate){
    vector<float> opl;
    vector<float> opr;
    vector<float> ops;
    float max = 0.0f, min = 0.0f, fmaMax = 0.0f, fmaMin = 0.0f;
    s.check();
    model m(c, s.get_model());
    cout << m;

    float rx_v = getModelVal(c, m, rx);
    float ry_v = getModelVal(c, m, ry);
    float rz_v = getModelVal(c, m, rz);
    float sx_v = getModelVal(c, m, sx);
    float sy_v = getModelVal(c, m, sy);
    float sz_v = getModelVal(c, m, sz);
    float radius_v = getModelVal(c, m, radius);
   cout << "var_v: " << setprecision(9) 
	      << rx_v << " " 
	      << ry_v << " "
	      << rz_v << " " 
	      << sx_v << " " 
	      << sy_v << " " 
	      << sz_v << "  "
	      << radius_v << " " 
	      << endl; 

    opl.push_back(rx_v);
    opr.push_back(rx_v);
    ops.push_back(rx_v * rx_v);

    opl.push_back(ry_v);
    opr.push_back(ry_v);
    ops.push_back(ry_v * ry_v);

    opl.push_back(rz_v);
    opr.push_back(rz_v);
    ops.push_back(rz_v * rz_v);

    fmaMax = ro.getFMAMax(opl, opr);
    fmaMin = ro.getFMAMin(opl, opr);
    max = ro.getPlusMax(ops);
    min = ro.getPlusMin(ops);

    if(fmaMax != fmaMin || max != min || fmaMax != max){
      break;
    }

    opl.clear();
    opr.clear();
    ops.clear();

    opl.push_back(rx_v);
    opr.push_back(sx_v);
    ops.push_back(rx_v * sx_v);

    opl.push_back(ry_v);
    opr.push_back(sy_v);
    ops.push_back(ry_v * sy_v);

    opl.push_back(rz_v);
    opr.push_back(sz_v);
    ops.push_back(rz_v * sz_v);

    fmaMax = ro.getFMAMax(opl, opr);
    fmaMin = ro.getFMAMin(opl, opr);
    max = ro.getPlusMax(ops);
    min = ro.getPlusMin(ops);

    if(fmaMax != fmaMin || max != min || fmaMax != max){
      break;
    }

    opl.clear();
    opr.clear();
    ops.clear();

    opl.push_back(sx_v);
    opr.push_back(sx_v);
    ops.push_back(sx_v * sx_v);

    opl.push_back(sy_v);
    opr.push_back(sy_v);
    ops.push_back(sy_v * sy_v);

    opl.push_back(sz_v);
    opr.push_back(sz_v);
    ops.push_back(sz_v * sz_v);

    fmaMax = ro.getFMAMax(opl, opr);
    fmaMin = ro.getFMAMin(opl, opr);
    max = ro.getPlusMax(ops);
    min = ro.getPlusMin(ops);

    if(fmaMax != fmaMin || max != min || fmaMax != max){
      break;
    }

    s.add(getBlockClause(rx_v, rx, c));
    s.add(getBlockClause(ry_v, ry, c));
    s.add(getBlockClause(rz_v, rz, c));
    s.add(getBlockClause(sx_v, sx, c));
    s.add(getBlockClause(sy_v, sy, c));
    s.add(getBlockClause(sz_v, sz, c));
    s.add(getBlockClause(radius_v, radius, c));
    
    iter_count ++;
  }

  cout << iter_count << endl;
}

void dot_product(char *cond) {
  bool terminate = false; 
  int  iter_count = 0;

  context c;
 
  Reorder ro(FE_TONEAREST);

  expr x = c.real_const("x");
  expr y = c.real_const("y");
  expr z = c.real_const("z");
  expr x1 = c.real_const("x1");
  expr y1 = c.real_const("y1");
  expr z1 = c.real_const("z1");
  expr v = c.real_val(cond);                 
  solver s(c);
 
  s.add(x*x + y*y + z*z  + x1*x1 + y1*y1 + z1*z1 == v);                    
  while(!terminate){
    vector<float> opl;
    vector<float> opr;
    vector<float> ops;
    float max = 0.0f, min = 0.0f, fmaMax = 0.0f, fmaMin = 0.0f;

    s.check();
    model m(c, s.get_model());
    float x_v = getModelVal(c, m, x);
    opl.push_back(x_v);
    opr.push_back(x_v);
    ops.push_back(x_v * x_v);
    float y_v = getModelVal(c, m, y);
    opl.push_back(y_v);
    opr.push_back(y_v);
    ops.push_back(y_v * y_v);
    float z_v = getModelVal(c, m, z);
    opl.push_back(z_v);
    opr.push_back(z_v);
    ops.push_back(z_v * z_v);

    float x1_v = getModelVal(c, m, x1);
    opl.push_back(x1_v);
    opr.push_back(x1_v);
    ops.push_back(x1_v * x1_v);
    float y1_v = getModelVal(c, m, y1);
    opl.push_back(y1_v);
    opr.push_back(y1_v);
    ops.push_back(y1_v * y1_v);
    float z1_v= getModelVal(c, m, z1);
    opl.push_back(z1_v);
    opr.push_back(z1_v);
    ops.push_back(z1_v * z1_v);

    std::cout << "value: " << std::setprecision(9) 
	      << x_v << " " 
	      << y_v << " "
	      << z_v << " " 
	      << x1_v << " " 
	      << y1_v << " " 
	      << z1_v << " " 
	      << std::endl;   

    z3::expr x_v_expr = m.get_const_interp(x.decl());

    s.add(getBlockClause(x_v, x, c));
    s.add(getBlockClause(y_v, y, c));
    s.add(getBlockClause(z_v, z, c));
    s.add(getBlockClause(x1_v, x1, c));
    s.add(getBlockClause(y1_v, y1, c));
    s.add(getBlockClause(z1_v, z1, c));

    /*
    string nf_s = getBound(x_v, 1);
    string pf_s = getBound(x_v, -1);

    expr nf_e = c.real_val(nf_s.c_str());
    expr pf_e = c.real_val(pf_s.c_str());
    s.add(x < pf_e || x > nf_e);

    z3::expr y_v_expr = m.get_const_interp(y.decl());
    s.add(y != y_v_expr);
    z3::expr z_v_expr = m.get_const_interp(z.decl());
    s.add(z != z_v_expr);

    z3::expr x1_v_expr = m.get_const_interp(x1.decl());
    s.add(x1 != x1_v_expr);
    z3::expr y1_v_expr = m.get_const_interp(y1.decl());
    s.add(y1 != y1_v_expr);
    z3::expr z1_v_expr = m.get_const_interp(z1.decl());
    s.add(z1 != z1_v_expr);
    */ 

    fmaMax = ro.getFMAMax(opl, opr);
    fmaMin = ro.getFMAMin(opl, opr);
    max = ro.getPlusMax(ops);
    min = ro.getPlusMin(ops);

    
    std::cout << "extreme: " << std::setprecision(9) 
	      << fmaMax << " " 
	      << fmaMin << "  "
	      << max << " " 
	      << min << " " 
	      << std::endl;    
    

    if(fmaMax != fmaMin || max != min || fmaMax != max){
      terminate = true;
    }

    iter_count ++;
  }

  cout << iter_count << endl;
}

void sum(char *cond){
  bool terminate = false; 
  int  iter_count = 0;
  string nf_s;
  string pf_s;

  context c;
  Reorder ro(FE_TONEAREST);

  expr x = c.real_const("x");
  expr y = c.real_const("y");
  expr z = c.real_const("z");
  expr x1 = c.real_const("x1");
  expr y1 = c.real_const("y1");
  expr z1 = c.real_const("z1");
  expr v = c.real_val(cond);                 
  solver s(c);
 
  s.add(x + y + z + x1 + y1 + z1== v);                    
  while(!terminate){
    vector<float> ops;
    float max = 0.0f, min = 0.0f;

    s.check();
    model m(c, s.get_model());
    float x_v = getModelVal(c, m, x);
    ops.push_back(x_v);
    float y_v = getModelVal(c, m, y);
    ops.push_back(y_v);
    float z_v = getModelVal(c, m, z);
    ops.push_back(z_v);
    float x1_v = getModelVal(c, m, x1);
    ops.push_back(x1_v);
    float y1_v = getModelVal(c, m, y1);
    ops.push_back(y1_v);
    float z1_v = getModelVal(c, m, z1);
    ops.push_back(z1_v);
    
    
    cout << "var_v: " << setprecision(9) 
	      << x_v << " " 
	      << y_v << " "
	      << z_v << " " 
	      << x1_v << " " 
	      << y1_v << " " 
	      << z1_v << " " 
	      << endl; 
    

    nf_s = getBound(x_v, 1);
    pf_s = getBound(x_v, -1);
    expr nf_x = c.real_val(nf_s.c_str());
    expr pf_x = c.real_val(pf_s.c_str());
    s.add(x < pf_x || x > nf_x);

    nf_s = getBound(y_v, 1);
    pf_s = getBound(y_v, -1);
    expr nf_y = c.real_val(nf_s.c_str());
    expr pf_y = c.real_val(pf_s.c_str());
    s.add(y < pf_y || y > nf_y);

    nf_s = getBound(z_v, 1);
    pf_s = getBound(z_v, -1);
    expr nf_z = c.real_val(nf_s.c_str());
    expr pf_z = c.real_val(pf_s.c_str());
    s.add(z < pf_z || z > nf_z);

    nf_s = getBound(x1_v, 1);
    pf_s = getBound(x1_v, -1);
    expr nf_x1 = c.real_val(nf_s.c_str());
    expr pf_x1 = c.real_val(pf_s.c_str());
    s.add(x1 < pf_x1 || x1 > nf_x1);

    nf_s = getBound(y1_v, 1);
    pf_s = getBound(y1_v, -1);
    expr nf_y1 = c.real_val(nf_s.c_str());
    expr pf_y1 = c.real_val(pf_s.c_str());
    s.add(y1 < pf_y1 || y1 > nf_y1);

    nf_s = getBound(z1_v, 1);
    pf_s = getBound(z1_v, -1);
    expr nf_z1 = c.real_val(nf_s.c_str());
    expr pf_z1 = c.real_val(pf_s.c_str());
    s.add(z1 < pf_z1 || z1 > nf_z1);


     
    max = ro.getPlusMax(ops);
    min = ro.getPlusMin(ops);

    std::cout << "extreme: " << std::setprecision(9) 
	      << max << " " 
	      << min << " " 
	      << std::endl;     

    if(max != min){
      terminate = true;
    }

    iter_count ++;
  }

  cout << iter_count << endl;

}



int main() {
  clock_t begin, end;
  begin = clock();
  mandelbrot();
  //ray_tracing();
  end = clock();
  cout << "time: " << double(end - begin)/CLOCKS_PER_SEC << endl;
  /*
  begin = clock();
  sum("5.001");
  end = clock();
  cout << "time: " << double(end - begin)/CLOCKS_PER_SEC << endl;
  
  begin = clock();
  sum("10");
  end = clock();
  cout << "time: " << double(end - begin)/CLOCKS_PER_SEC << endl;
  
  begin = clock();
  sum("12.37");
  end = clock();
  cout << "time: " << double(end - begin)/CLOCKS_PER_SEC << end;

  begin = clock();
  sum("50.001");
  end = clock();
  cout << "time: " << double(end - begin)/CLOCKS_PER_SEC << endl;
 
  begin = clock();
  sum("100.031");
  end = clock();
  cout << "time: " << double(end - begin)/CLOCKS_PER_SEC << endl;
   
  begin = clock();
  sum("500.001");
  end = clock();
  cout << "time: " << double(end - begin)/CLOCKS_PER_SEC << endl;
   
  begin = clock();
  dot_product("5.001");
  end = clock();
  cout << "time: " << double(end - begin)/CLOCKS_PER_SEC << endl;
 
  begin = clock();
    dot_product("10");
  end = clock();
  cout << "time: " << double(end - begin)/CLOCKS_PER_SEC << endl;
  
  begin = clock();
    dot_product("12.37");
  end = clock();
  cout << "time: " << double(end - begin)/CLOCKS_PER_SEC << end;
  
  begin = clock();
    dot_product("50.001");
  end = clock();
  cout << "time: " << double(end - begin)/CLOCKS_PER_SEC << endl;
 
  begin = clock();
    dot_product("100.031");
  end = clock();
  cout << "time: " << double(end - begin)/CLOCKS_PER_SEC << endl;
  
  begin = clock();
    dot_product("500.001");
  end = clock();
  cout << "time: " << double(end - begin)/CLOCKS_PER_SEC << endl;
  
  */
  std::cout << "\n";
  std::cout << "done\n";
  return 0;
}
