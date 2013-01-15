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
# Revision 1.5  2003/07/23 15:13:44  soliday
# Modified makefiles to work with base versions 3.13 and 3.14
#
# Revision 1.4  2002/08/14 15:49:49  soliday
# Added Open License
#
# Revision 1.3  1999/10/08 21:54:22  soliday
# Fixed RM command for WIN32.
#
# Revision 1.2  1999/09/16 22:04:49  soliday
# Deletes the dll file before before attempting to make a new one.
#
# Revision 1.1  1999/07/12 19:43:28  soliday
# Makefile for customized fdlibm library used with WIN32
#
#

TOP=../../..

#If epics/extensions/configure directory exists, build with it.
#Otherwise use epics/extensions/config.
ifeq (0, $(words $(notdir $(wildcard $(TOP)/configure))))
include $(TOP)/config/CONFIG_EXTENSIONS
ifdef WIN32
all buildInstall::
	@$(RM) O.WIN32/fdlibm.*
endif
include $(TOP)/config/RULES_ARCHS
else
NO_BORLAND_STDC=1
include $(TOP)/configure/CONFIG
include $(TOP)/src/SDDS/fdlibm/Makefile.OAG
include $(TOP)/configure/RULES
endif

