ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = test_parse
lib_LIBRARIES = libcfgparse.a

libcfgparse_a_SOURCES = cfgparse_util.c \
			cfgparse_util.h \
			cfgparselib.h

test_parse_SOURCES = test_parse.c

LDADD = libcfgparse.a

TESTS = $(bin_PROGRAMS)

