#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#
# $Log: not supported by cvs2svn $
# Revision 1.3  2002/08/14 15:40:12  soliday
# Added Open License
#
# Revision 1.2  1999/04/14 14:05:09  borland
# Works with new build system.  Fixed some missing prototypes and function type mismatches.
#
# Revision 1.1  1995/09/18 16:08:14  saunders
# Header file installation for SDDS1.5.
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
include $(TOP)/config/RULES_ARCHS
else
include $(TOP)/configure/CONFIG
include $(TOP)/src/SDDS/include/Makefile.OAG
include $(TOP)/configure/RULES
endif

