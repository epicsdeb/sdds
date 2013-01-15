#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#
# makefile for Java SDDS 3D Plotter
# $Log: not supported by cvs2svn $
# Revision 1.4  2005/05/11 15:13:42  soliday
# Updated so that the intended jar command is used and not the default jar program.
#
# Revision 1.3  2003/08/04 19:19:18  soliday
# Fixed typo
#
# Revision 1.2  2003/08/04 19:11:03  soliday
# Fixed the classpath
#
# Revision 1.1  2003/07/15 17:33:16  soliday
# Added files to cvs.
#
#
#
#

TOP = ../../../..
include $(TOP)/configure/CONFIG

JAR = SDDS3Dplot.jar
JAR_PACKAGES = SDDS/java/SDDS3Dplot
JARMANIFEST = ../manifest
JAVACFLAGS = -O

JAVA =	Frame1.java \
	Application1.java \
	Dialog1.java \
	Dialog4.java

USR_CLASSPATH = ../visad.jar

include $(TOP)/configure/RULES

