bin_PROGRAMS = propd_get propd_set
sbin_PROGRAMS = propagated

propagateddir = ${datadir}/propagated

propd_get_SOURCES = src/propd_get.c include/propagate.h
propd_set_SOURCES = src/propd_set.c include/propagate.h
propagated_SOURCES = src/main.c src/config.c src/propagate.c src/unix.c src/unix_epoll.c src/unix_select.c include/propagate.h include/propagated.h include/modules.h include/propagate_list.h src/propagate_list.c
dist_propagated_DATA = misc/SLES_init-propagated misc/sysconfig-propagated

AM_CPPFLAGS = -I./include

