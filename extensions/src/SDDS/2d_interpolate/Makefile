#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#
#
# High-level makefile for building 2d_interpolate
# $Log: not supported by cvs2svn $

TOP=../../..
ifeq (0, $(words $(notdir $(wildcard $(TOP)/configure))))
include $(TOP)/config/CONFIG_EXTENSIONS
else
include $(TOP)/configure/CONFIG
endif

DIRS = nn csa
ifneq ($(HOST_ARCH),Linux)
ifneq ($(HOST_ARCH),linux-x86)
ifneq ($(HOST_ARCH),linux-athlon)
ifneq ($(HOST_ARCH),linux-x86-borland)
ifneq ($(EPICS_HOST_ARCH),linux-x86)
ifneq ($(EPICS_HOST_ARCH),linux-x86-borland) 
endif
endif
endif
endif
endif
endif
 
ifeq (0, $(words $(notdir $(wildcard $(TOP)/configure))))
include $(TOP)/config/RULES_DIRS
else
include $(TOP)/configure/RULES_DIRS
endif
