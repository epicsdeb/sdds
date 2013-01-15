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
# Revision 1.3  2002/08/14 17:35:27  soliday
# Added Open License
#
# Revision 1.2  1999/04/14 14:04:59  borland
# Works with new build system.  Fixed some missing prototypes and function type mismatches.
#
# Revision 1.1  1995/09/05 21:08:42  saunders
# First test release of the SDDS1.5 package.
#
#
#  Lowest Level Directroy Makefile
#
#

TOP=../../../..

#If epics/extensions/configure directory exists, build with it.
#Otherwise use epics/extensions/config.
ifeq (0, $(words $(notdir $(wildcard $(TOP)/configure))))
include $(TOP)/config/CONFIG_EXTENSIONS
include $(TOP)/config/RULES_ARCHS
else
include $(TOP)/configure/CONFIG
include $(TOP)/src/SDDS/SDDSaps/sddscontours/Makefile.OAG
include $(TOP)/configure/RULES
endif
