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
# Revision 1.11  2007/07/20 19:21:06  soliday
# Fixed problem on Windows when using non-cygwin perl.
#
# Revision 1.10  2003/09/16 21:11:20  soliday
# Added a minor change.
#
# Revision 1.9  2003/07/24 20:15:00  soliday
# The second library should always be relinked now.
#
# Revision 1.8  2003/07/23 02:01:55  soliday
# Updated last change.
#
# Revision 1.7  2003/07/23 00:42:07  soliday
# Modified makefiles to work with base versions 3.13 and 3.14
#
# Revision 1.6  2002/08/14 16:34:46  soliday
# Added Open License
#
# Revision 1.5  1999/10/08 21:55:11  soliday
# Fixed RM command for WIN32.
#
# Revision 1.4  1999/09/16 22:07:58  soliday
# Deletes the dll file before before attempting to make a new one.
#
# Revision 1.3  1999/04/14 14:05:05  borland
# Works with new build system.  Fixed some missing prototypes and function type mismatches.
#
# Revision 1.2  1998/07/07 22:29:40  borland
# Completely new method of scanning the namelists in the headers.
# Twice as fast and eliminates need for namelist library in the
# SDDS toolkit.
#
# Revision 1.1  1995/09/05  21:10:37  saunders
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
	@$(RM) O.WIN32/SDDS1.* O.WIN32/SDDS1c.*
endif
include $(TOP)/config/RULES_ARCHS
else
include $(TOP)/configure/CONFIG
include $(TOP)/src/SDDS/SDDSlib/Makefile.OAG
include $(TOP)/configure/RULES
buildInstall:
ifneq ($(MPI),1)
	$(MV) $(TOP)/src/SDDS/SDDSlib/Makefile $(TOP)/src/SDDS/SDDSlib/Makefile.TMP
	$(CP) $(TOP)/src/SDDS/SDDSlib/Makefile.SDDS1 $(TOP)/src/SDDS/SDDSlib/Makefile
	$(RM) MakefileInclude
	$(MAKE) -W SDDS_rpn.c -f $(TOP)/src/SDDS/SDDSlib/Makefile
	$(RM) $(TOP)/src/SDDS/SDDSlib/Makefile
	$(MV) $(TOP)/src/SDDS/SDDSlib/Makefile.TMP $(TOP)/src/SDDS/SDDSlib/Makefile
endif
endif

