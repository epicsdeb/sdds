#
# $Id: Makefile,v 1.2 2004-07-29 19:36:56 soliday Exp $
#

TOP=../../..

ifeq (0, $(words $(notdir $(wildcard $(TOP)/configure))))
include $(TOP)/config/CONFIG_EXTENSIONS
include $(TOP)/config/RULES_ARCHS
else
include $(TOP)/configure/CONFIG
include $(TOP)/src/SDDS/oagLib/Makefile.OAG
include $(TOP)/configure/RULES
endif
