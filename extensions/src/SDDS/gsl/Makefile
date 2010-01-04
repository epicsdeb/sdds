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

GSL=$(TOP)/src/SDDS/gsl

SRC_DIRS = $(GSL)/sys $(GSL)/err $(GSL)/complex $(GSL)/specfunc

SYSSRCS = minmax.c prec.c hypot.c log1p.c expm1.c coerce.c invhyp.c pow_int_sys.c infnan.c fdiv.c fcmp.c ldfrexp.c

ERRSRCS = error.c stream.c message.c strerror.c

COMPLEXSRCS = math.c inline.c

SPECFUNCSRCS = airy.c airy_der.c airy_zero.c atanint.c bessel.c bessel_I0.c bessel_I1.c bessel_In.c bessel_Inu.c bessel_J0.c bessel_J1.c bessel_Jn.c bessel_Jnu.c bessel_K0.c bessel_K1.c bessel_Kn.c bessel_Knu.c bessel_Y0.c bessel_Y1.c bessel_Yn.c bessel_Ynu.c bessel_amp_phase.c bessel_i.c bessel_j.c bessel_k.c bessel_olver.c bessel_temme.c bessel_y.c bessel_zero.c bessel_sequence.c beta.c beta_inc.c clausen.c coulomb.c coupling.c coulomb_bound.c dawson.c debye.c dilog.c elementary.c ellint.c elljac.c erfc.c exp.c expint.c expint3.c fermi_dirac.c gegenbauer.c gamma.c gamma_inc.c hyperg_0F1.c hyperg_2F0.c hyperg_1F1.c hyperg_2F1.c hyperg_U.c hyperg.c laguerre.c lambert.c legendre_H3d.c legendre_Qn.c legendre_con.c legendre_poly.c log.c mathieu_angfunc.c mathieu_charv.c mathieu_coeff.c mathieu_radfunc.c mathieu_workspace.c poch.c pow_int_specfunc.c psi.c result.c shint.c sinint.c synchrotron.c transport.c trig.c zeta.c

gsl_SRCS = $(SYSSRCS) $(ERRSRCS) $(COMPLEXSRCS) $(SPECFUNCSRCS)

include $(TOP)/configure/RULES

