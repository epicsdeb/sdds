#
# $Id: Makefile,v 1.4 2010-10-31 19:42:46 soliday Exp $
#
# $Log: not supported by cvs2svn $
# Revision 1.3  2009/09/01 20:39:40  soliday
# Fixed support for Windows using Intel Visual Fortran and added support
# for cygwin-x86.
#
# Revision 1.2  2004/05/03 15:49:25  soliday
# Updated to work with Epics/Base 3.14 build rules.
#
# Revision 1.1  1999/10/21 18:56:18  soliday
# First version
#
#
#
TOP=../../..

ifeq (0, $(words $(notdir $(wildcard $(TOP)/configure))))
include $(TOP)/config/CONFIG_EXTENSIONS
include $(TOP)/config/RULES_ARCHS
else
include $(TOP)/configure/CONFIG
include $(TOP)/src/SDDS/fortran/Makefile.OAG
include $(TOP)/configure/RULES

ifeq ($(T_A),cygwin-x86)
LINKDIR =/$(EPICS_EXTENSIONS_LIB)
FC = gfortran
else
LINKDIR =$(EPICS_EXTENSIONS_LIB)
endif

ifeq ($(T_A),linux-x86)
f77_OPTIONS += -m32
endif
ifeq ($(T_A),linux-x86_64)
f77_OPTIONS += -m64
endif

ifdef WIN32
FC = ifort
#This is very specific to my own computer but it shouldn't be too difficult for anyone to modify if needed.

fortranReadExample$(OBJ): ../fortranReadExample.f
	$(FC) /compile_only /threads /libs:dll /fast c:\\epics\\extensions\\src\\SDDS\\fortran\\fortranReadExample.f

fortranWriteExample$(OBJ): ../fortranWriteExample.f
	$(FC) /compile_only /threads /libs:dll /fast c:\\epics\\extensions\\src\\SDDS\\fortran\\fortranWriteExample.f

fortranReadExample: fortran_wrapper$(OBJ) fortranReadExample$(OBJ)
	$(FC) /exe:fortranReadExample.exe /threads /libs:dll \
	fortran_wrapper$(OBJ) fortranReadExample$(OBJ) \
	/link /incremental:no /opt:ref /release \
	/force:multiple \
	c:\\epics\\extensions\\lib\\$(T_A)\\mdbcommon.lib \
	c:\\epics\\extensions\\lib\\$(T_A)\\SDDS1.lib \
	c:\\epics\\extensions\\lib\\$(T_A)\\mdbmth.lib \
	c:\\epics\\extensions\\lib\\$(T_A)\\mdblib.lib \
	c:\\epics\\extensions\\lib\\$(T_A)\\z.lib
	$(CP) ../fortranReadExample.sdds .

fortranWriteExample: fortran_wrapper$(OBJ) fortranWriteExample$(OBJ)
	$(FC) /exe:fortranWriteExample.exe /threads /libs:dll \
	fortran_wrapper$(OBJ) fortranWriteExample$(OBJ) \
	/link /incremental:no /opt:ref /release \
	/force:multiple \
	c:\\epics\\extensions\\lib\\$(T_A)\\mdbcommon.lib \
	c:\\epics\\extensions\\lib\\$(T_A)\\SDDS1.lib \
	c:\\epics\\extensions\\lib\\$(T_A)\\mdbmth.lib \
	c:\\epics\\extensions\\lib\\$(T_A)\\mdblib.lib \
	c:\\epics\\extensions\\lib\\$(T_A)\\z.lib


else
fortranReadExample$(OBJ): ../fortranReadExample.f
	$(FC) $(f77_OPTIONS) -c ../fortranReadExample.f

fortranWriteExample$(OBJ): ../fortranWriteExample.f
	$(FC) $(f77_OPTIONS) -c ../fortranWriteExample.f


fortranReadExample: fortran_wrapper$(OBJ) fortranReadExample$(OBJ)
	$(FC) $(f77_OPTIONS) fortran_wrapper$(OBJ) fortranReadExample$(OBJ) -I$(INSTALL_LIB) -L$(LINKDIR) -lmdbcommon -lSDDS1 -lmdbmth -lmdblib -lz -o fortranReadExample
	$(CP) ../fortranReadExample.sdds .

fortranWriteExample: fortran_wrapper$(OBJ) fortranWriteExample$(OBJ)
	$(FC) $(f77_OPTIONS) fortran_wrapper$(OBJ) fortranWriteExample$(OBJ) -I$(INSTALL_LIB) -L$(LINKDIR) -lmdbcommon -lSDDS1 -lmdbmth -lmdblib -lz -o fortranWriteExample

endif

endif
