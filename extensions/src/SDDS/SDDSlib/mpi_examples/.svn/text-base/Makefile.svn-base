#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#
# $Id: Makefile,v 1.1 2008-04-14 18:35:13 shang Exp $
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
include $(TOP)/src/SDDS/SDDSlib/mpi_examples/Makefile.OAG
ifeq ($(OS_CLASS),solaris)
SHRLIB_LDFLAGS = -G -h libmdbplt.so -z nodefs
endif
ifeq ($(OS_CLASS),Linux)
SHRLIB_LDFLAGS = -shared
endif
include $(TOP)/configure/RULES
endif

