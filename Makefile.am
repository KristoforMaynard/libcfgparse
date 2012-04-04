ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = test_argparse.c test_fileparse.c
lib_LIBRARIES = libcfgparse.a

libkdv_a_SOURCES = configparse.c \
		   configparse.h \
		   fileparse.c \
		   fileparse.h \
		   cfgparselib.h

test_argparse_SOURCES = test_argparse.c

test_fileparse_SOURCES = test_fileparse.c

TESTS = $(bin_PROGRAMS)

