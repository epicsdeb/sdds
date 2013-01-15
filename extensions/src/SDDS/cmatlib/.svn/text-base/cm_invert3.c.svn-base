/* routine: cm_invert()
 * purpose: invert a complex matrix
 * usage: cm_invert(A, B) ==>  A=INV(B); A and B must point to matrix
 * 	  structures of the same size.
 * Michael Borland, 1986 (after CITLIB routine MATINV), Louis Emery 1992.
 */
#include "complex.h"
#include "cmatlib.h"
#include "mdb.h"
#define MAXIMUM_EXPONENT 126

int cm_invert(CMATRIX *A, CMATRIX *B)         /* A=inv(B) */
{
  register long i, j, k, l, m, n;
  static long *ipivot=NULL, **index=NULL, *ind_l;
  static COMPLEX *pivot=NULL, piv, t;
  static double swap, *tmp, abs_amax; 
  static long row, col, max_n=0;
  static double *ar_j, *ai_j, *ar_col, *ai_col, *ar_m, *ai_m;
  double mod_a_col_l;
  long exponent_mod_a_col_l;
  long exponent_t, exponent_piv;
  COMPLEX c;

  if (!A)
    bomb("NULL CMATRIX (A) passed to cm_invert", NULL);
  if (!B)
    bomb("NULL CMATRIX (B) passed to cm_invert", NULL);
  if (!A->ar || !A->ai)
    bomb("NULL CMATRIX data (A) in cm_invert", NULL);
  if (!B->ar || !B->ai)
    bomb("NULL CMATRIX data (A) in cm_invert", NULL);

  if ((n=A->n)!=A->m || A->m!=B->n || B->n!=B->m) {
    fprintf(stderr, "CMATRIX size mismatch (cm_invert)\n");
    return(0);
  }

  if (max_n<n) {
    ipivot = trealloc(ipivot, sizeof(*ipivot)*n);
    pivot  = trealloc(pivot, sizeof(*pivot)*n);
    if (!index || !max_n)
      index = (long**)zarray_2d(sizeof(**index), n, 2);
    else
      index = (long**)resize_zarray_2d(sizeof(**index), max_n, 2, (void**)index, n, 2);
    max_n = n;
  }
  if (!ipivot)
    bomb("pivot index array is NULL (cm_invert)", NULL);
  if (!index)
    bomb("index array is NULL (cm_invert)", NULL);
  if (!pivot)
    bomb("pivot array is NULL (cm_invert)", NULL);

  cm_copy(A, B);
  for (j=0; j<n; j++) 
    ipivot[j] = 0;

  for (i=0; i<n; i++) {
    /* search for pivot element */
    abs_amax = 0;
    for (j=0; j<n; j++) {
      if (!(ar_j = A->ar[j]) || !(ai_j = A->ai[j])) {
        fprintf(stderr, "row %ld of CMATRIX A is NULL (cm_invert)", j);
        abort();
      }
      if (ipivot[j]!=1) {
        for (k=0; k<n; k++) {
		    if (ipivot[k]>1)
            return(0);
		    if (ipivot[k]!=1 && abs_amax<sqrt(sqr(ar_j[k])+sqr(ai_j[k]))) {
            col = k;
            row = j;
            abs_amax =  sqrt(sqr(ar_j[k])+sqr(ai_j[k]));
          }
        }
		}
    }
    if (abs_amax == 0.)
      return(0);
    ipivot[col] = ipivot[col] + 1;
    /* interchange rows to put pivot element on diagonal */
    if (row!=col) {
      tmp       = A->ar[row];
      A->ar[row] = A->ar[col];
      A->ar[col] = tmp;
      tmp       = A->ai[row];
      A->ai[row] = A->ai[col];
      A->ai[col] = tmp;
    }
    if (!index[i])
      bomb("pivot index array has NULL element (cm_invert)", NULL);
    index[i][0] = row;
    index[i][1] = col;
    pivot[i] = cassign(A->ar[col][col],A->ai[col][col]);
    /* divide pivot row by pivot element */
    A->ar[col][col] = 1.0;
    A->ai[col][col] = 0.0;
    ar_col = A->ar[col];
    ai_col = A->ai[col];
    piv = pivot[i];
    frexp( cmod(piv), (int*) &exponent_piv ); /* frexp also return the unneeded mantissa */
    for (l=0; l<n; l++) {
      mod_a_col_l = cmod(cassign(ar_col[l],ai_col[l]));
      frexp( mod_a_col_l, (int*) &exponent_mod_a_col_l);
      if (mod_a_col_l!=0 && (exponent_mod_a_col_l-exponent_piv)> MAXIMUM_EXPONENT) {
        printf("error: floating overflow in cm_invert (pivot too small).\n");
        return(0);
      }
      c=cdiv(cassign(ar_col[l],ai_col[l]),piv);
      ar_col[l]=c.r;
      ai_col[l]=c.i;
    }
    /* reduce non-pivot rows */
    for (m=0; m<n; m++) {
      if (m!=col) {
        ar_m = A->ar[m];
        ai_m = A->ai[m];
        t = cassign(ar_m[col],ai_m[col]);
        ar_m[col] = 0.;
        ai_m[col] = 0.;
        if (t.r!=0 || t.i!=0)
          frexp(cmod(t),(int*)&exponent_t);
        else
          exponent_t = 0;
        for (l=0; l<n; l++) {
          mod_a_col_l = cmod(cassign(ar_col[l],ai_col[l]));
          frexp( mod_a_col_l,(int*)&exponent_mod_a_col_l);
          /*                  if ((a_col[l]!=0?(long)log10(FABS(a_col[l])+0.5):0)+log10_t > 37) {    */
          if ((mod_a_col_l!=0?exponent_mod_a_col_l:0)+exponent_t > MAXIMUM_EXPONENT) {
            printf("error: floating-point overflow in cm_invert().\n");
            return(0);
          }
          /*		    a_m[l] -= a_col[l]*t;    */
          c=csub(cassign(ar_m[l],ai_m[l]),cmul(cassign(ar_col[l],ai_col[l]),t));
          ar_m[l]=c.r;
          ai_m[l]=c.i;
        }
		}
    }
  }

  /* interchange columns */
  for (i=0; i<n; i++) {
    l = n - i - 1;
    ind_l = index[l];
    if (ind_l[0] == ind_l[1])
      continue;
    if ((row = ind_l[0])<0 || row>n) 
      bomb("row out of range in column interchange (cm_invert)", NULL);
    if ((col = ind_l[1])<0 || col>n)
      bomb("column out of range in column interchange (cm_invert)", NULL);
    for (j=0; j<n; j++) {
      ar_j = A->ar[j];
      swap = ar_j[row];
      ar_j[row] = ar_j[col];
      ar_j[col] = swap;
      ai_j = A->ai[j];
      swap = ai_j[row];
      ai_j[row] = ai_j[col];
      ai_j[col] = swap;
    }
  }
  return(1);
}
