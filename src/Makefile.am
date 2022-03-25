#=====================================================
# m2disk
# Lilith Machine Disk Utility
#
# Guido Hoss, 25.03.2022
#
# Published by Guido Hoss under GNU Public License V3.
#=====================================================

AM_CFLAGS = -Wall -DVERSION_BUILD_DATE=\""$(shell date +'%F')"\"

bin_PROGRAMS = m2disk

m2disk_SOURCES = \
	m2disk.c \
	m2disk.h \
	m2d_usage.c m2d_usage.h	\
	m2d_medos.c m2d_medos.h
