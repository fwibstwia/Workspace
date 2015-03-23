int general(float a, float b, float c, float d) {
  float r = a + b * c * d;      
  klee_tag_reorderable(&r);            
  if (r > 0)
    return 0;
} 

int main() {
  float a, b, c, d;
  klee_make_symbolic_with_sort(&a, sizeof(a), "a", 0, 32);
  klee_make_symbolic_with_sort(&b, sizeof(b), "b", 0, 32); 
  klee_make_symbolic_with_sort(&c, sizeof(c), "c", 0, 32);
  klee_make_symbolic_with_sort(&d, sizeof(d), "d", 0, 32);
  return general(a, b, c, d);
} 
