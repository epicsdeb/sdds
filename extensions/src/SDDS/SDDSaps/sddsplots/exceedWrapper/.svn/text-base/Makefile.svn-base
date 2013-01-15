#
# $Log: not supported by cvs2svn $
# Revision 1.2  2000/04/17 19:20:05  soliday
# Only Microsoft Visual C can make this library.
#
# Revision 1.1  2000/04/17 18:31:04  soliday
# First version of Makefile.
#
#
#

TOP=../../../../..

include $(TOP)/config/CONFIG_EXTENSIONS

ifdef WIN32

ifneq ($(findstring orland, $(HOST_ARCH) $(EPICS_HOST_ARCH),orland)


include $(TOP)/config/RULES_ARCHS

else

all buildInstall::
	@echo This can only be built by Microsoft Visual C

clean::
	@echo This can only be built by Microsoft Visual C

endif

endif
