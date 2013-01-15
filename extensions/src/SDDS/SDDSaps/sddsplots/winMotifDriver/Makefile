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
# Revision 1.2  2002/08/14 17:27:01  soliday
# Added Open License
#
# Revision 1.1  2001/03/23 21:25:57  soliday
# This version replaces the version that required Exceed to run on WIN32.
#
#

TOP=../../../../..

#If epics/extensions/configure directory exists, build with it.
#Otherwise use epics/extensions/config.
ifeq (0, $(words $(notdir $(wildcard $(TOP)/configure))))
include $(TOP)/config/CONFIG_EXTENSIONS
include $(TOP)/config/RULES_ARCHS
else
include $(TOP)/configure/CONFIG
include $(TOP)/src/SDDS/SDDSaps/sddsplots/winMotifDriver/Makefile.OAG
include $(TOP)/configure/RULES
endif

