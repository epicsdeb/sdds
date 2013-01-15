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
# Revision 1.1  2002/09/19 16:39:45  soliday
# Added libtiff library
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
	@$(RM) O.WIN32/libtiff.*
endif
include $(TOP)/config/RULES_ARCHS
else
include $(TOP)/configure/CONFIG
include $(TOP)/src/SDDS/tiff/Makefile.OAG
include $(TOP)/configure/RULES
endif

