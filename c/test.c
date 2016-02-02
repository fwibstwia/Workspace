#include <cstdlib>
#include <vector>

int main(){
  std::vector<unsigned char> value;
  value.resize(32);
  char* s = "0";
  float v = strtof(s, NULL);
  char *p = reinterpret_cast<char*>(&v);
  std::copy(p, p + sizeof(float), value.begin());
}
