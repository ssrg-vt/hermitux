SUBDIRS=src doc tests

EXTRA_DIST=autogen.sh SConstruct CMakeLists.txt meson.build meson_options.txt README.FreeBSD scripts

CFLAGS += -g3 -O0
CXXFLAGS = -g3 -O0
LDFLAGS += -static -lm

ACLOCAL_AMFLAGS = -I m4 -I scripts/m4

distcleancheck_listfiles = find -type f -exec sh -c 'test -f $(srcdir)/{} || echo {}' ';'

AM_DISTCHECK_CONFIGURE_FLAGS=--with-openssl
