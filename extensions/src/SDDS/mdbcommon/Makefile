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
# Revision 1.3  2003/07/23 02:19:55  soliday
# Modified makefiles to work with base versions 3.13 and 3.14
#
# Revision 1.2  2002/08/14 16:48:26  soliday
# Added Open License
#
# Revision 1.1  2000/04/11 16:20:27  soliday
# First version of Makefile.
#
#
#

TOP=../../..

#If epics/extensions/configure directory exists, build with it.
#Otherwise use epics/extensions/config.
ifeq (0, $(words $(notdir $(wildcard $(TOP)/configure))))
include $(TOP)/config/CONFIG_EXTENSIONS
ifdef WIN32
all buildInstall::
	@$(RM) O.WIN32/mdbcommon.*
endif
include $(TOP)/config/RULES_ARCHS
else
include $(TOP)/configure/CONFIG
include $(TOP)/src/SDDS/mdbcommon/Makefile.OAG
include $(TOP)/configure/RULES
endif
