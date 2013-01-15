#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#
# $Id: Makefile,v 1.3 2003-07-23 03:09:48 soliday Exp $
#

TOP=../../..

#If epics/extensions/configure directory exists, build with it.
#Otherwise use epics/extensions/config.
ifeq (0, $(words $(notdir $(wildcard $(TOP)/configure))))
include $(TOP)/config/CONFIG_EXTENSIONS
include $(TOP)/config/RULES_ARCHS
else
include $(TOP)/configure/CONFIG
include $(TOP)/src/SDDS/png/Makefile.OAG
include $(TOP)/configure/RULES
endif

