AC_INIT(FVR, 1.0.0, james@stev.org)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_MACRO_DIR([m4])

LT_INIT
AM_INIT_AUTOMAKE([foreign])
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

AX_CXX_COMPILE_STDCXX_11(noext, mandatory)

m4_ifdef([AX_COMPILER_FLAGS],
	[
		AX_COMPILER_FLAGS([WARN_CFLAGS],[WARN_LDFLAGS])
		AC_SUBST([AM_CXXFLAGS], $WARN_CXXFLAGS)
	]
,
	[
		AC_SUBST([AM_CXXFLAGS], ['-Wall -Werror'])
	]
)

dnl give error and exit if we don't have pkgconfig
AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, [ ], [
  AC_MSG_ERROR([You need to have pkg-config installed!])
])

AC_CHECK_LIB([pthread], [pthread_mutex_init], [], [
	AC_MSG_ERROR([You need to have pthread installed])
])

PKG_CHECK_MODULES(DBUS, [
  dbus-1 >= 0.29.1
], [
  AC_SUBST(DBUS_CFLAGS)
  AC_SUBST(DBUS_LIBS)
], [
  AC_MSG_ERROR([
      You need to install or upgrade the dbus dev package
  ])
])


dnl required versions of gstreamer and plugins-base
GST_REQUIRED=1.12.0
GSTPB_REQUIRED=1.12.0

PKG_CHECK_MODULES(GST, [
  gstreamer-1.0 >= $GST_REQUIRED
  gstreamer-base-1.0 >= $GST_REQUIRED
  gstreamer-controller-1.0 >= $GST_REQUIRED
  gstreamer-app-1.0 >= $GST_REQUIRED
], [
  AC_SUBST(GST_CFLAGS)
  AC_SUBST(GST_LIBS)
], [
  AC_MSG_ERROR([
      You need to install or upgrade the GStreamer development
      packages on your system. On debian/ubuntu these are
      libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev gstreamer1.0-dev libgstreamer1.0-dev
      gstreamer1.0-plugins-ugly gstreamer1.0-plugins-bad gstreamer1.0-plugins-good
      The minimum version required is $GST_REQUIRED.
  ])
])

AC_OUTPUT(
	Makefile
	src/Makefile
)

