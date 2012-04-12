ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = test_parse
pkglib_LTLIBRARIES = libcfgparse.la

libcfgparse_la_SOURCES = cfgparse_util.c \
			cfgparse_arg.c \
			cfgparse_file.c
			
pkginclude_HEADERS = cfgparse_util.h \
		     cfgparse_arg.h \
		     cfgparse_file.h \
		     cfgparselib.h

test_parse_SOURCES = test_parse.c
test_parse_LDADD = libcfgparse.la

TESTS = $(bin_PROGRAMS)

