#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#
# $Log: Makefile,v $
# Revision 1.6  2011/01/12 16:20:37  soliday
# Added the poly subdirectory.
#
# Revision 1.5  2010/08/31 15:38:28  soliday
# Added the matrix and block subsections of the gsl library.
#
# Revision 1.4  2010/06/30 15:43:57  soliday
# This version should work now.
#
# Revision 1.3  2010/06/29 20:48:38  soliday
# Added a bunch of files per Shang's request.
#
# Revision 1.2  2009/04/28 21:01:30  soliday
# Moved to child directory
#
# Revision 1.1  2009/04/24 18:22:02  soliday
# Imported the gsl-1.12 library into CVS.
#
#
#

# The full gsl library contains many more files than we have here.
# For our purposes we only need the files from the sys, err, 
# complex, and specfunc subdirectories. Those files are included
# here.


TOP=../../..
include $(TOP)/configure/CONFIG

CMPLR = 

#RATIONAL = purify
#RATIONAL = quantify
#RATIONAL = purecov
ifdef RATIONAL
HOST_OPT = NO
CC := $(RATIONAL) -best-effort $(CC)
CCC := $(RATIONAL) -best-effort $(CCC)
endif


LIBRARY = gsl
SHARED_LIBRARIES = NO

# private gsl hearder files
INC += gsl/gsl_blas.h
INC += gsl/gsl_blas_types.h
INC += gsl/gsl_block_char.h
INC += gsl/gsl_block_complex_double.h
INC += gsl/gsl_block_complex_float.h
INC += gsl/gsl_block_complex_long_double.h
INC += gsl/gsl_block_double.h
INC += gsl/gsl_block_float.h
INC += gsl/gsl_block.h
INC += gsl/gsl_block_int.h
INC += gsl/gsl_block_long_double.h
INC += gsl/gsl_block_long.h
INC += gsl/gsl_block_short.h
INC += gsl/gsl_block_uchar.h
INC += gsl/gsl_block_uint.h
INC += gsl/gsl_block_ulong.h
INC += gsl/gsl_block_ushort.h
INC += gsl/gsl_bspline.h
INC += gsl/gsl_cblas.h
INC += gsl/gsl_cdf.h
INC += gsl/gsl_chebyshev.h
INC += gsl/gsl_check_range.h
INC += gsl/gsl_combination.h
INC += gsl/gsl_complex.h
INC += gsl/gsl_complex_math.h
INC += gsl/gsl_const_cgs.h
INC += gsl/gsl_const_cgsm.h
INC += gsl/gsl_const.h
INC += gsl/gsl_const_mksa.h
INC += gsl/gsl_const_mks.h
INC += gsl/gsl_const_num.h
INC += gsl/gsl_deriv.h
INC += gsl/gsl_dft_complex_float.h
INC += gsl/gsl_dft_complex.h
INC += gsl/gsl_dht.h
INC += gsl/gsl_diff.h
INC += gsl/gsl_eigen.h
INC += gsl/gsl_errno.h
INC += gsl/gsl_fft_complex_float.h
INC += gsl/gsl_fft_complex.h
INC += gsl/gsl_fft.h
INC += gsl/gsl_fft_halfcomplex_float.h
INC += gsl/gsl_fft_halfcomplex.h
INC += gsl/gsl_fft_real_float.h
INC += gsl/gsl_fft_real.h
INC += gsl/gsl_fit.h
INC += gsl/gsl_heapsort.h
INC += gsl/gsl_histogram2d.h
INC += gsl/gsl_histogram.h
INC += gsl/gsl_ieee_utils.h
INC += gsl/gsl_inline.h
INC += gsl/gsl_integration.h
INC += gsl/gsl_interp.h
INC += gsl/gsl_linalg.h
INC += gsl/gsl_machine.h
INC += gsl/gsl_math.h
INC += gsl/gsl_matrix_char.h
INC += gsl/gsl_matrix_complex_double.h
INC += gsl/gsl_matrix_complex_float.h
INC += gsl/gsl_matrix_complex_long_double.h
INC += gsl/gsl_matrix_double.h
INC += gsl/gsl_matrix_float.h
INC += gsl/gsl_matrix.h
INC += gsl/gsl_matrix_int.h
INC += gsl/gsl_matrix_long_double.h
INC += gsl/gsl_matrix_long.h
INC += gsl/gsl_matrix_short.h
INC += gsl/gsl_matrix_uchar.h
INC += gsl/gsl_matrix_uint.h
INC += gsl/gsl_matrix_ulong.h
INC += gsl/gsl_matrix_ushort.h
INC += gsl/gsl_message.h
INC += gsl/gsl_min.h
INC += gsl/gsl_minmax.h
INC += gsl/gsl_mode.h
INC += gsl/gsl_monte.h
INC += gsl/gsl_monte_miser.h
INC += gsl/gsl_monte_plain.h
INC += gsl/gsl_monte_vegas.h
INC += gsl/gsl_multifit.h
INC += gsl/gsl_multifit_nlin.h
INC += gsl/gsl_multimin.h
INC += gsl/gsl_multiroots.h
INC += gsl/gsl_nan.h
INC += gsl/gsl_ntuple.h
INC += gsl/gsl_odeiv.h
INC += gsl/gsl_permutation.h
INC += gsl/gsl_permute_char.h
INC += gsl/gsl_permute_complex_double.h
INC += gsl/gsl_permute_complex_float.h
INC += gsl/gsl_permute_complex_long_double.h
INC += gsl/gsl_permute_double.h
INC += gsl/gsl_permute_float.h
INC += gsl/gsl_permute.h
INC += gsl/gsl_permute_int.h
INC += gsl/gsl_permute_long_double.h
INC += gsl/gsl_permute_long.h
INC += gsl/gsl_permute_short.h
INC += gsl/gsl_permute_uchar.h
INC += gsl/gsl_permute_uint.h
INC += gsl/gsl_permute_ulong.h
INC += gsl/gsl_permute_ushort.h
INC += gsl/gsl_permute_vector_char.h
INC += gsl/gsl_permute_vector_complex_double.h
INC += gsl/gsl_permute_vector_complex_float.h
INC += gsl/gsl_permute_vector_complex_long_double.h
INC += gsl/gsl_permute_vector_double.h
INC += gsl/gsl_permute_vector_float.h
INC += gsl/gsl_permute_vector.h
INC += gsl/gsl_permute_vector_int.h
INC += gsl/gsl_permute_vector_long_double.h
INC += gsl/gsl_permute_vector_long.h
INC += gsl/gsl_permute_vector_short.h
INC += gsl/gsl_permute_vector_uchar.h
INC += gsl/gsl_permute_vector_uint.h
INC += gsl/gsl_permute_vector_ulong.h
INC += gsl/gsl_permute_vector_ushort.h
INC += gsl/gsl_poly.h
INC += gsl/gsl_pow_int.h
INC += gsl/gsl_precision.h
INC += gsl/gsl_qrng.h
INC += gsl/gsl_randist.h
INC += gsl/gsl_rng.h
INC += gsl/gsl_roots.h
INC += gsl/gsl_sf_airy.h
INC += gsl/gsl_sf_bessel.h
INC += gsl/gsl_sf_clausen.h
INC += gsl/gsl_sf_coulomb.h
INC += gsl/gsl_sf_coupling.h
INC += gsl/gsl_sf_dawson.h
INC += gsl/gsl_sf_debye.h
INC += gsl/gsl_sf_dilog.h
INC += gsl/gsl_sf_elementary.h
INC += gsl/gsl_sf_ellint.h
INC += gsl/gsl_sf_elljac.h
INC += gsl/gsl_sf_erf.h
INC += gsl/gsl_sf_exp.h
INC += gsl/gsl_sf_expint.h
INC += gsl/gsl_sf_fermi_dirac.h
INC += gsl/gsl_sf_gamma.h
INC += gsl/gsl_sf_gegenbauer.h
INC += gsl/gsl_sf.h
INC += gsl/gsl_sf_hyperg.h
INC += gsl/gsl_sf_laguerre.h
INC += gsl/gsl_sf_lambert.h
INC += gsl/gsl_sf_legendre.h
INC += gsl/gsl_sf_log.h
INC += gsl/gsl_sf_mathieu.h
INC += gsl/gsl_sf_pow_int.h
INC += gsl/gsl_sf_psi.h
INC += gsl/gsl_sf_result.h
INC += gsl/gsl_sf_synchrotron.h
INC += gsl/gsl_sf_transport.h
INC += gsl/gsl_sf_trig.h
INC += gsl/gsl_sf_zeta.h
INC += gsl/gsl_siman.h
INC += gsl/gsl_sort_char.h
INC += gsl/gsl_sort_double.h
INC += gsl/gsl_sort_float.h
INC += gsl/gsl_sort.h
INC += gsl/gsl_sort_int.h
INC += gsl/gsl_sort_long_double.h
INC += gsl/gsl_sort_long.h
INC += gsl/gsl_sort_short.h
INC += gsl/gsl_sort_uchar.h
INC += gsl/gsl_sort_uint.h
INC += gsl/gsl_sort_ulong.h
INC += gsl/gsl_sort_ushort.h
INC += gsl/gsl_sort_vector_char.h
INC += gsl/gsl_sort_vector_double.h
INC += gsl/gsl_sort_vector_float.h
INC += gsl/gsl_sort_vector.h
INC += gsl/gsl_sort_vector_int.h
INC += gsl/gsl_sort_vector_long_double.h
INC += gsl/gsl_sort_vector_long.h
INC += gsl/gsl_sort_vector_short.h
INC += gsl/gsl_sort_vector_uchar.h
INC += gsl/gsl_sort_vector_uint.h
INC += gsl/gsl_sort_vector_ulong.h
INC += gsl/gsl_sort_vector_ushort.h
INC += gsl/gsl_specfunc.h
INC += gsl/gsl_spline.h
INC += gsl/gsl_statistics_char.h
INC += gsl/gsl_statistics_double.h
INC += gsl/gsl_statistics_float.h
INC += gsl/gsl_statistics.h
INC += gsl/gsl_statistics_int.h
INC += gsl/gsl_statistics_long_double.h
INC += gsl/gsl_statistics_long.h
INC += gsl/gsl_statistics_short.h
INC += gsl/gsl_statistics_uchar.h
INC += gsl/gsl_statistics_uint.h
INC += gsl/gsl_statistics_ulong.h
INC += gsl/gsl_statistics_ushort.h
INC += gsl/gsl_sum.h
INC += gsl/gsl_sys.h
INC += gsl/gsl_test.h
INC += gsl/gsl_types.h
INC += gsl/gsl_vector_char.h
INC += gsl/gsl_vector_complex_double.h
INC += gsl/gsl_vector_complex_float.h
INC += gsl/gsl_vector_complex.h
INC += gsl/gsl_vector_complex_long_double.h
INC += gsl/gsl_vector_double.h
INC += gsl/gsl_vector_float.h
INC += gsl/gsl_vector.h
INC += gsl/gsl_vector_int.h
INC += gsl/gsl_vector_long_double.h
INC += gsl/gsl_vector_long.h
INC += gsl/gsl_vector_short.h
INC += gsl/gsl_vector_uchar.h
INC += gsl/gsl_vector_uint.h
INC += gsl/gsl_vector_ulong.h
INC += gsl/gsl_vector_ushort.h
INC += gsl/gsl_version.h
INC += gsl/gsl_wavelet2d.h
INC += gsl/gsl_wavelet.h

GSL=$(TOP)/src/SDDS/gsl

SRC_DIRS = $(GSL)/sys $(GSL)/err $(GSL)/complex $(GSL)/specfunc $(GSL)/interpolation $(GSL)/linalg $(GSL)/vector $(GSL)/matrix $(GSL)/block $(GSL)/poly

SYSSRCS = sys_minmax.c prec.c hypot.c log1p.c expm1.c coerce.c invhyp.c pow_int_sys.c infnan.c fdiv.c fcmp.c ldfrexp.c

ERRSRCS = error.c stream.c message.c strerror.c

COMPLEXSRCS = math.c complex_inline.c

SPECFUNCSRCS = airy.c airy_der.c airy_zero.c atanint.c bessel.c bessel_I0.c bessel_I1.c bessel_In.c bessel_Inu.c bessel_J0.c bessel_J1.c bessel_Jn.c bessel_Jnu.c bessel_K0.c bessel_K1.c bessel_Kn.c bessel_Knu.c bessel_Y0.c bessel_Y1.c bessel_Yn.c bessel_Ynu.c bessel_amp_phase.c bessel_i.c bessel_j.c bessel_k.c bessel_olver.c bessel_temme.c bessel_y.c bessel_zero.c bessel_sequence.c beta.c beta_inc.c clausen.c coulomb.c coupling.c coulomb_bound.c dawson.c debye.c dilog.c elementary.c ellint.c elljac.c erfc.c exp.c expint.c expint3.c fermi_dirac.c gegenbauer.c gamma.c gamma_inc.c hyperg_0F1.c hyperg_2F0.c hyperg_1F1.c hyperg_2F1.c hyperg_U.c hyperg.c laguerre.c lambert.c legendre_H3d.c legendre_Qn.c legendre_con.c legendre_poly.c log.c mathieu_angfunc.c mathieu_charv.c mathieu_coeff.c mathieu_radfunc.c mathieu_workspace.c poch.c pow_int_specfunc.c psi.c result.c shint.c sinint.c synchrotron.c transport.c trig.c zeta.c

INTERPOLATION = accel.c akima.c cspline.c interp.c linear.c spline.c poly.c interpolation_inline.c

LINALG = multiply.c exponential.c tridiag.c lu.c luc.c hh.c qr.c qrpt.c lq.c ptlq.c svd.c householder.c householdercomplex.c hessenberg.c hesstri.c cholesky.c choleskyc.c symmtd.c hermtd.c bidiag.c balance.c balancemat.c

VECTOR = vector_copy.c vector_file.c vector_init.c vector_minmax.c vector_oper.c vector_prop.c reim.c subvector.c vector_swap.c vector.c vector_view.c

VECTOR = vector_copy.c vector_file.c vector_init.c vector_minmax.c vector_prop.c reim.c subvector.c vector_swap.c vector.c vector_view.c

MATRIX = matrix_init.c matrix.c matrix_file.c rowcol.c matrix_swap.c matrix_copy.c matrix_minmax.c matrix_prop.c matrix_oper.c getset.c matrix_view.c submatrix.c

MATRIX = matrix_init.c matrix.c matrix_file.c matrix_swap.c matrix_copy.c matrix_minmax.c matrix_prop.c getset.c matrix_view.c matrix_oper.c

BLOCK = block_init.c block_file.c block.c

POLY = dd.c eval.c solve_quadratic.c solve_cubic.c zsolve_quadratic.c zsolve_cubic.c zsolve.c zsolve_init.c

gsl_SRCS = $(SYSSRCS) $(ERRSRCS) $(COMPLEXSRCS) $(SPECFUNCSRCS) $(INTERPOLATION) $(LINALG) $(VECTOR) $(MATRIX) $(BLOCK) $(POLY)

include $(TOP)/configure/RULES

