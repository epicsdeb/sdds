#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#
# makefile for Java SDDS library
# $Log: not supported by cvs2svn $
# Revision 1.11  2002/12/02 22:07:03  soliday
# Added a new javadoc option
#
# Revision 1.10  2002/08/14 19:32:45  soliday
# Added Open License
#
# Revision 1.9  2002/06/24 15:53:47  soliday
# Added the JAVADOC variable
#
# Revision 1.8  2002/06/24 15:43:23  soliday
# Changed the JARCMD option
#
# Revision 1.7  2001/12/04 22:59:14  soliday
# Added array support written by Seth Nemesure.
#
# Revision 1.6  2001/10/17 22:12:28  soliday
# Fixed a problem on Solaris.
#
# Revision 1.5  2001/06/05 18:47:11  soliday
# Changed package name.
#
# Revision 1.4  2000/05/09 21:11:19  soliday
# Made a minor change.
#
# Revision 1.3  2000/05/09 21:07:37  soliday
# Modified the method in which the jar file is created.
#
# Revision 1.2  2000/05/03 16:56:01  soliday
# Changed how html documentation is created.
#
# Revision 1.1  2000/05/01 14:29:24  soliday
# First version.
#
#
#

TOP = ../../../..
include $(TOP)/configure/CONFIG

JAR = SDDS.jar
JAR_PACKAGES = SDDS
JAVADOC = SDDS
JAVACFLAGS += -O
JAVADOCFLAGS += -windowtitle "SDDS documentation"

JAVA = 	SDDSUtil.java \
        SDDSParameter.java \
        SDDSColumn.java \
        SDDSArray.java \
        SDDSFile.java \
	LEDataInputStream.java \
        LEDataOutputStream.java \
        LERandomAccessFile.java


include $(TOP)/configure/RULES
