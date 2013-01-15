#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#
# $Id: Makefile,v 1.4 2003-07-23 03:54:08 soliday Exp $
#
#  Lowest Level Directroy Makefile
# $Log: not supported by cvs2svn $
# Revision 1.3  2002/08/14 17:12:33  soliday
# Added Open License
#
# Revision 1.2  1999/04/14 14:04:54  borland
# Works with new build system.  Fixed some missing prototypes and function type mismatches.
#
# Revision 1.1  1995/09/05 21:05:40  saunders
# First test release of the SDDS1.5 package.
#
#

TOP=../../..

#If epics/extensions/configure directory exists, build with it.
#Otherwise use epics/extensions/config.
ifeq (0, $(words $(notdir $(wildcard $(TOP)/configure))))
include $(TOP)/config/CONFIG_EXTENSIONS
DIRS = sddsplots pseudoInverse sddscontours
include $(TOP)/config/RULES_ARCHS
else
include $(TOP)/configure/CONFIG
DIRS = sddsplots pseudoInverse sddscontours
include $(TOP)/src/SDDS/SDDSaps/Makefile.OAG
include $(TOP)/configure/RULES
endif
