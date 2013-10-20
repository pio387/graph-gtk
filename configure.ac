AC_INIT(graph-gtk, 0.1)

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])

LT_INIT

AC_CONFIG_FILES([  graph-gtk.pc:graph-gtk.pc.in
		   Makefile
		   graph-gtk/Makefile 
		   tests/Makefile ])


AM_INIT_AUTOMAKE
AM_SILENT_RULES

AC_CONFIG_SRCDIR([graph-gtk/graph-gtk-view.c])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
PKG_CHECK_MODULES(GTK, gtk+-3.0)

AC_OUTPUT
