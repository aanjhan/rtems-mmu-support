##
##  $Id: Makefile.am
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = mmutests
mmutests_SOURCES = init.c system.h

dist_rtems_tests_DATA = mmutests.scn


include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

mmutests_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(mmutests_OBJECTS) $(mmutests_LDADD)
LINK_LIBS = $(mmutests_LDLIBS)

mmutests$(EXEEXT): $(mmutests_OBJECTS) $(mmutests_DEPENDENCIES)
	@rm -f mmutests$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
