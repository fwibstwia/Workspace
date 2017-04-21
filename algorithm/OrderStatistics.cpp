#include <cstdlib>
#include <iostream>

using namespace std;

int partition(int *A, int p, int r){
  int x = A[r];
  int i = p - 1;
  for (int j = p; j < r; j ++){
    if(A[j] <= x){
      int temp = A[i + 1];
      A[i + 1] = A[j];
      A[j] = temp;
      i ++;
    }
  }
  int temp = A[i + 1];
  A[i + 1] = A[r];
  A[r] = temp;
  return i + 1;
}

int randomized_partition(int *A, int p, int r){
  int i = rand() % (r - p + 1) + p;
  int temp = A[r];
  A[r] = A[i];
  A[i] = temp;
  return partition (A, p, r);
}


int randomized_select(int *A, int p, int r, int i){
  if(p == r)
    return A[p];
  int q = randomized_partition(A, p, r);
  int k = q - p + 1;
  if(i == k)
    return A[q];
  else if (i < k)
    randomized_select(A, p, q - 1, i);
  else
    randomized_select(A, q + 1, r, i - k);
}

int main(){
  int A[5] = {2, 4, 6, 3, 7};
  int r = randomized_select(A, 0, 4, 2);
  cout << r << endl;  
}
