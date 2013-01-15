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
# $Log: not supported by cvs2svn $
# Revision 1.5  2002/08/14 19:32:57  soliday
# Added Open License
#
# Revision 1.4  2002/06/24 15:44:53  soliday
# Changed so that it compiles all the java files at the same time.
#
# Revision 1.3  2001/06/05 18:48:39  soliday
# Changed package name.
#
# Revision 1.2  2000/05/03 13:36:34  soliday
# Fixed a small compiling bug.
#
# Revision 1.1  2000/05/01 14:34:20  soliday
# First version.
#
#
#

TOP = ../../../..
include $(TOP)/configure/CONFIG

JAR = SDDSedit.jar
JAR_PACKAGES = SDDS/java/SDDSedit
JARMANIFEST = ../manifest
JAVACFLAGS = -O

JAVA =	sddsEditAttributesDialog.java \
	SearchDialog.java \
	TextInsertDialog.java \
	InfoDialog.java \
	sddsEdit.java \
        MatchList.java


include $(TOP)/configure/RULES
