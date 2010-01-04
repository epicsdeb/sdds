#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#
# makefile for Java SDDS command line applications
# $Log: Makefile,v $
# Revision 1.10  2002/12/02 22:07:41  soliday
# Added a new javadoc option
#
# Revision 1.9  2002/08/14 19:32:52  soliday
# Added Open License
#
# Revision 1.8  2002/06/24 16:01:08  soliday
# Added JAVADOC option
#
# Revision 1.7  2002/06/24 15:43:45  soliday
# Changed the JARCMD option
#
# Revision 1.6  2001/06/05 18:49:28  soliday
# Changed package names.
#
# Revision 1.5  2000/05/10 22:05:10  soliday
# Fixed a bug.
#
# Revision 1.4  2000/05/10 17:37:17  soliday
# Revamped makefile so that the class files are not in a package.
#
# Revision 1.3  2000/05/09 21:13:39  soliday
# Modified the method in which jar files are created.
#
# Revision 1.2  2000/05/03 16:52:22  soliday
# Modifed the manner in which documentation is created.
#
# Revision 1.1  2000/05/01 14:32:29  soliday
# First version.
#
#
#

TOP = ../../../..
include $(TOP)/config/CONFIG_EXTENSIONS

JAVAC_FLAGS = -O

JAVA = sddsconvert.java sddsquery.java sddsplot.java

JAR_CLASSES += $(INSTALL_JAVA)/sddsconvert.class
JAR_CLASSES += $(INSTALL_JAVA)/sddsquery.class
JAR_CLASSES += $(INSTALL_JAVA)/sddsplot.class

JAR = SDDSaps.jar
JARCMD = $(subst \,/,$(JAVA_DIR)/bin/jar$(EXE))
JAVADOC =$(subst \,/,$(JAVA_DIR)/bin/javadoc$(EXE))

include $(TOP)/config/RULES.Host

$(INSTALL_JAVA)/%.class:%.java
	@echo Creating java class file $@
	$(RM) $@
	$(JAVACCMD) -d $(INSTALL_JAVA) $<


$(JAR): $(JAR_CLASSES)
	$(JARCMD) -cf $(JAR) \
	-C $(INSTALL_JAVA) sddsconvert.class \
	-C $(INSTALL_JAVA) sddsplot.class \
	-C $(INSTALL_JAVA) sddsplot\$$1.class \
	-C $(INSTALL_JAVA) sddsquery.class
	$(CP) $(JAR) $(INSTALL_JAVA)


clean::
	$(RM) $(INSTALL_CLASSES)
	$(RM) $(INSTALL_JAVA)/sddsplot??.class
	$(RM) $(JAR) $(INSTALL_JAR)

doc:
	$(MKDIR) doc

html::  doc
	$(JAVADOC) -d doc \
	-version -author \
	-private -classpath $(INSTALL_JAVA) \
	-windowtitle "SDDS application documentation" -breakiterator \
	sddsconvert.java sddsquery.java sddsplot.java

