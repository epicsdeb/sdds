#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#
# $Id: Makefile,v 1.3 2003-07-23 02:26:24 soliday Exp $
#
# $Log: not supported by cvs2svn $
# Revision 1.2  2002/08/14 16:49:35  soliday
# Added Open License
#
# Revision 1.1  2000/04/11 19:55:53  soliday
# First version of Makefile.
#
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
include $(TOP)/src/SDDS/utils/Makefile.OAG
include $(TOP)/configure/RULES
endif

