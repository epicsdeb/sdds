#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#
# makefile for Java SDDS Editor
# $Log: Makefile,v $
# Revision 1.4  2002/08/14 19:33:04  soliday
# Added Open License
#
# Revision 1.3  2002/06/24 15:44:07  soliday
# Changed the JARCMD option.
#
# Revision 1.2  2001/06/05 18:49:04  soliday
# Changed package name.
#
# Revision 1.1  2000/05/09 21:22:06  soliday
# First version.
#
#
#
#

TOP = ../../../..
include $(TOP)/config/CONFIG_EXTENSIONS

JAVAC_FLAGS = -O

PACKAGE = SDDS/java/SDDSplot
SDDS_LIBRARY = SDDS/java/SDDS

JAVA = TickInterval.java plotApplet.java

JAR_INPUT += $(INSTALL_JAVA)/$(PACKAGE)/.
JAR_INPUT += $(INSTALL_JAVA)/$(SDDS_LIBRARY)
JAR_CLASSES += $(INSTALL_JAVA)/$(PACKAGE)/TickInterval.class
JAR_CLASSES += $(INSTALL_JAVA)/$(PACKAGE)/plotApplet.class

JAR = SDDSplot.jar
JARCMD = $(subst \,/,$(JAVA_DIR)/bin/jar$(EXE))

include $(TOP)/config/RULES.Host

$(INSTALL_JAVA)/$(PACKAGE):
	$(MKDIR) $(INSTALL_JAVA)/$(PACKAGE)

$(JAR): $(JAR_CLASSES)
	$(JARCMD) -cf $(JAR) \
	-C $(INSTALL_JAVA) $(SDDS_LIBRARY) \
	-C $(INSTALL_JAVA) $(PACKAGE)
	$(CP) $(JAR) $(INSTALL_JAVA)

clean::
	$(RM) $(INSTALL_JAVA)/$(PACKAGE)/*.class
	$(RM) $(JAR) $(INSTALL_JAR)

