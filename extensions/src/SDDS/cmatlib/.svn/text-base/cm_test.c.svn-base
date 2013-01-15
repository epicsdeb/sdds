/* test program */
#include "mdb.h"
#include "complex.h"
#include "cmatlib.h"

void cm_get(CMATRIX *A);
void cm_rand(CMATRIX *A, double lo, double hi);

int main()
{
  CMATRIX *A, *B, *C;
  long n;
  COMPLEX detA, detB, detC, product;

  random_1(-1);

  while (n = query_long("dimension of arrays", 3L)) {
    cm_alloc(&A, n, n);
    cm_alloc(&B, n, n);
    cm_alloc(&C, n, n);

    cm_rand(A, 0.1L, 1.0L);    
    /*        cm_get(A); */
    if (n<10)
      cm_show(A, "%9.6le + i %9.6le  ", "CMATRIX:\n", stdout);
    detA=cm_det(A);
    printf("complex determinant             : %.16le + i %.16le\n", detA.r, detA.i);

    cm_invert(B, A);
    if (n<10)
      cm_show(B, "%9.6le + i %9.6le  ", "inverse:\n", stdout);
    detB=cm_det(B);
    printf("complex determinant of inverse: %.16le + i %.16le\n", detB.r,detB.i);
    product=cmul(detA,detB);
    printf("complex product of determinants: %.16le + i %.16le\n", product.r,product.i);

    cm_mult(C, A, B);
    if (n<10)
      cm_show(C, "%23.16le + i %23.16le  ", "product\n", stdout);
    detC=cm_det(C);
    printf("determinant of product: %.16le + i %.16le\n",detC.r,detC.i);
    cm_free(&A);
    cm_free(&B);
    cm_free(&C);
  }
  exit(0);
}

void cm_get(CMATRIX *A)
{
  register long i, j;
  char s[132];
  for (i=0; i<A->n; i++) {
    printf("row %ld:\n", i);
    for (j=0; j<A->m; j++) {
      printf("\t%ld: ", j);
      gets(s);
      sscanf(s, "%lf %lf", A->ar[i]+j, A->ai[i]+j);
    }
  }
}


void cm_rand(CMATRIX *A, double lo, double hi ) {
  register long i,j,n,m;
  register double range;
  double *a_i;
  
  n=A->n;
  m=A->m;
  range = hi-lo;
  for (i=0; i<n; i++) {
    a_i = A->ar[i];
    for (j=0; j<n; j++) 
      a_i[j] = lo + random_1(0)*range;
    a_i = A->ai[i];
    for (j=0; j<n; j++) 
      a_i[j] = lo + random_1(0)*range;
  }
}
