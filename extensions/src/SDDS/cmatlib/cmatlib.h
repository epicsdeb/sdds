/* file: cmatlib.h
 * purpose: definitions for C complex matrix routines 
 *
 * Michael Borland, 1989
 * Louis Emery, 1992
 */
#ifndef _CMATLIB_
#define _CMATLIB_

#include "SDDScomplex.h"

COMPLEX cassign_me(CMATRIX *A, int row, int col);
void cm_error(char *message);
int cm_add(CMATRIX *C, CMATRIX *A, CMATRIX *B);
void cm_alloc(CMATRIX **A, int n, int m);
int cm_copy(CMATRIX *A, CMATRIX *B);
COMPLEX cm_det(CMATRIX *D);
void cm_free(CMATRIX **A);
int cm_invert(CMATRIX *A, CMATRIX *B);
int cm_mult(CMATRIX *C, CMATRIX *A, CMATRIX *B);
int cm_scmul(CMATRIX *B, CMATRIX *A, double a);
void cm_show(CMATRIX *A, char *format, char *label, FILE *fp);
void cm_get(CMATRIX *A);
void cm_rand(CMATRIX *A, double lo, double hi);
int cm_trans(CMATRIX *B, CMATRIX *A);
void cm_zero(CMATRIX *A);
void cm_identity(CMATRIX *A);
int cm_check(CMATRIX *A);

#endif
