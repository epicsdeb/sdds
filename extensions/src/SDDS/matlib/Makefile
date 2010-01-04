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
# Revision 1.6  2003/07/23 02:14:39  soliday
# Modified makefiles to work with base versions 3.13 and 3.14
#
# Revision 1.5  2002/08/14 16:45:20  soliday
# Added Open License
#
# Revision 1.4  1999/10/08 21:55:38  soliday
# Fixed RM command for WIN32.
#
# Revision 1.3  1999/09/16 22:06:51  soliday
# Deletes the dll file before before attempting to make a new one.
#
# Revision 1.2  1999/04/14 14:05:10  borland
# Works with new build system.  Fixed some missing prototypes and function type mismatches.
#
# Revision 1.1  1995/09/05 21:15:59  saunders
# First test release of the SDDS1.5 package.
#
#
#  Lowest Level Directroy Makefile
#
#

TOP=../../..

#If epics/extensions/configure directory exists, build with it.
#Otherwise use epics/extensions/config.
ifeq (0, $(words $(notdir $(wildcard $(TOP)/configure))))
include $(TOP)/config/CONFIG_EXTENSIONS
ifdef WIN32
all buildInstall::
	@$(RM) O.WIN32/matlib.*
endif
include $(TOP)/config/RULES_ARCHS
else
include $(TOP)/configure/CONFIG
include $(TOP)/src/SDDS/matlib/Makefile.OAG
include $(TOP)/configure/RULES
endif
