AC_INIT([xmlcopyeditor], [1.3.1.0])
AC_CONFIG_SRCDIR([src/xmlcopyeditor.cpp])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 foreign])

AC_PATH_PROG([APPDATA_VALIDATE], [appdata-validate], [/bin/true])
AC_PATH_PROG([DESKTOP_FILE_VALIDATE], [desktop-file-validate], [/bin/true])

GETTEXT_PACKAGE=AC_PACKAGE_NAME
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE",[The name of the gettext domain])
IT_PROG_INTLTOOL(0.40.0)

AC_PROG_CXX
AC_PROG_INSTALL

LT_INIT

PKG_PROG_PKG_CONFIG

CPPFLAGS="$CPPFLAGS -Wall -g -fexceptions"
CXXFLAGS?=

WXCONFIG=wx-config

AC_ARG_WITH(wx-config,
[[  --with-wx-config=FILE     Use the given path to wx-config when determining
                            wxWidgets configuration; defaults to "wx-config"]],
[
    if test "$withval" != "yes" -a "$withval" != ""; then
        WXCONFIG=$withval
    fi
])

wxversion=0

AC_DEFUN([WXTEST],
[
	AC_REQUIRE([AC_PROG_AWK])
	AC_MSG_CHECKING([wxWidgets version])
	if wxversion=`$WXCONFIG --version`; then
		AC_MSG_RESULT([$wxversion])
	else
		AC_MSG_RESULT([not found])
		AC_MSG_ERROR([wxWidgets is required. Try --with-wx-config.])
	fi])

# Call WXTEST func
WXTEST

# Verify minimus requires
vers=`echo $wxversion | $AWK 'BEGIN { FS = "."; } { printf "% d", ($1 * 1000 + $2) * 1000 + $3;}'`
if test -n "$vers" && test "$vers" -ge 2008000; then
	WX_CPPFLAGS="`$WXCONFIG --cppflags`"
	WX_CXXFLAGS="`$WXCONFIG --cxxflags | sed -e 's/-fno-exceptions//'`"
	WX_LIBS="`$WXCONFIG --unicode --libs std stc aui richtext`"
else
	AC_MSG_ERROR([wxWidgets 2.8.0 or newer is required])
fi

PKG_CHECK_MODULES([XML2], [libxml-2.0])
PKG_CHECK_MODULES([XSLT], [libxslt])

AC_ARG_ENABLE(debug,
[  --enable-debug,  Enable debug build],
[
  debugbuild="y"
  CXXFLAGS="${CXXFLAGS} -ggdb -O0"
], [
  CXXFLAGS="${CXXFLAGS} -DNDEBUG -DwxDEBUG_LEVEL=0"
])

# Check pcre is available
PKG_CHECK_MODULES([PCRE2], [libpcre2-8])

# Check boost::shared_ptr is available
AC_LANG(C++)
AC_CHECK_HEADER(boost/shared_ptr.hpp, ,
	AC_MSG_ERROR([boost headers not found]))

# Check xercesc is available
AC_LANG(C++)
AC_CHECK_HEADER(xercesc/util/PlatformUtils.hpp, ,
	AC_MSG_ERROR([Xerces-C headers not found]))

# Check unicode strings used in xercesc
AC_MSG_CHECKING([if we support the unicode strings used in xercesc])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
	#include <boost/static_assert.hpp>
	#include <xercesc/util/XMLChar.hpp>

	int main() {
		BOOST_STATIC_ASSERT(sizeof(XMLCh) == 1 || sizeof(XMLCh) == 2 || sizeof(XMLCh) == 4);
		return 0;
	}
	]])],
	[AC_MSG_RESULT([yes])],
	[AC_MSG_ERROR([no])
])

# Check expat is available
AC_LANG(C++)
AC_CHECK_HEADER(expat.h, ,
	AC_MSG_ERROR([Expat headers not found]))

# Check enchant-2
PKG_CHECK_MODULES(ENCHANT, [enchant-2],
	[AC_DEFINE([USE_ENCHANT]),
		AC_DEFINE([HAVE_LIBENCHANT_2])
	],
	# Check enchant
	PKG_CHECK_MODULES(ENCHANT, [enchant],
		[AC_DEFINE([USE_ENCHANT])],
		# otherwise Check ASPELL is available
		[AC_CHECK_HEADER(aspell.h, [ASPELL_LIBS="-laspell"],
			AC_MSG_ERROR([Aspell headers not found]))]
	)
)

AC_SUBST(ASPELL_LIBS)

AC_MSG_CHECKING([the GTK version to use])
AC_ARG_WITH([gtk],
	[AS_HELP_STRING([--with-gtk=2.0|3.0],
		[the GTK version to compile against [default: 3.0].
		It has to match the one that wxWidgets linked with.
		GTK is needed to make single-instance-check work.
		]
	)],
	[AS_CASE(["$with_gtk"],
		[2.0|3.0], [],
		[AC_MSG_ERROR([invalid GTK version specified])]
	)],
	[with_gtk=3.0]
)
AC_MSG_RESULT([$with_gtk])
AS_CASE(["$with_gtk"],
	[2.0], [PKG_CHECK_MODULES(GTK, [gtk+-2.0],
		[], [CXXFLAGS="$CXXFLAGS -D__NO_GTK__"])],
	[3.0], [PKG_CHECK_MODULES(GTK, [gtk+-3.0],
		[], [CXXFLAGS="$CXXFLAGS -D__NO_GTK__"])],
	[]
)

#AC_SUBST(GTK2_CFLAGS)
#AC_SUBST(GTK2_LIBS)

CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS"

AC_SUBST(WX_LIBS)

AC_CONFIG_FILES([Makefile data/Makefile m4/Makefile po/Makefile.in src/Makefile])
AC_OUTPUT

dnl Summarized output
echo
echo "***************************************"
echo

if [[ "$debugbuild" = "y" ]]; then
echo "Debug         Enabled"
else
echo "Debug         Disabled"
fi
if [[ "$GTK_LIBS" = "" ]]; then
echo "GTK           No. There will be problems when single instance is enabled."
fi

