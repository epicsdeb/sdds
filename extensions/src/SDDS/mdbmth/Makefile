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
# Revision 1.5  2002/08/14 16:18:52  soliday
# Added Open License
#
# Revision 1.4  1999/10/08 21:54:56  soliday
# Fixed RM command for WIN32.
#
# Revision 1.3  1999/09/16 22:06:14  soliday
# Deletes the dll file before before attempting to make a new one.
#
# Revision 1.2  1999/04/14 14:05:13  borland
# Works with new build system.  Fixed some missing prototypes and function type mismatches.
#
# Revision 1.1  1995/09/05 21:19:16  saunders
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
	@$(RM) O.WIN32/mdbmth.*
endif
include $(TOP)/config/RULES_ARCHS
else
include $(TOP)/configure/CONFIG
include $(TOP)/src/SDDS/mdbmth/Makefile.OAG
include $(TOP)/configure/RULES
endif
