#include <ceed.h>

int main(int argc, char **argv) {
  Ceed ceed;
  CeedVector x;
  CeedInt n;
  CeedScalar a[10], value = 3.0;
  const CeedScalar *b;

  CeedInit(argv[1], &ceed);
  n = 10;
  CeedVectorCreate(ceed, n, &x);
  for (CeedInt i=0; i<n; i++) a[i] = 10 + i;
  CeedVectorSetArray(x, CEED_MEM_HOST, CEED_USE_POINTER, a);
  CeedVectorGetArrayRead(x, CEED_MEM_HOST, &b);
  for (CeedInt i=0; i<n; i++) {
    if (b[i] != 10+i)
      return CeedError(ceed, (int)i, "Error reading array b[%d] = %f",i,
                       (double)b[i]);
  }
  CeedVectorRestoreArrayRead(x, &b);

  CeedVectorSetValue(x, value);
  CeedVectorGetArrayRead(x, CEED_MEM_HOST, &b);
  for (CeedInt i=0; i<n; i++) {
    if (b[i] != value)
      return CeedError(ceed, (int)i, "Error reading array b[%d] = %f",i,
                       (double)b[i]);
  }
  CeedVectorRestoreArrayRead(x, &b);
  
  CeedVectorDestroy(&x);
  CeedDestroy(&ceed);
  return 0;
}
