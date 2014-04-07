AC_INIT([xmlcopyeditor], [1.2.1.1])
AC_CONFIG_SRCDIR([src/xmlcopyeditor.cpp])
AM_INIT_AUTOMAKE

AC_PROG_CXX
AC_PROG_INSTALL
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

CPPFLAGS="$CPPFLAGS -Wall -g -fexceptions"
CXXFLAGS="$CXXFLAGS -Wall -g -fexceptions"

WXCONFIG=wx-config
LIBXMLCONFIG=xml2-config
LIBXSLTCONFIG=xslt-config

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

# Check for libxml2
libxmlversion=0

AC_DEFUN([LIBXML2TEST],
[
	AC_REQUIRE([AC_PROG_AWK])
	AC_MSG_CHECKING([libxml2])
	if libxmlversion=`$LIBXMLCONFIG --version`; then
		AC_MSG_RESULT([$libxmlversion])
	else
		AC_MSG_RESULT([not found])
		AC_MSG_ERROR([libxml2.])
	fi])

# Call XML2 TEST func
LIBXML2TEST

# Check for libxslt
libxsltversion=0

AC_DEFUN([LIBXSLTTEST],
[
	AC_REQUIRE([AC_PROG_AWK])
	AC_MSG_CHECKING([libxslt])
	if libxsltversion=`$LIBXSLTCONFIG --version`; then
		AC_MSG_RESULT([$libxsltversion])
	else
		AC_MSG_RESULT([not found])
		AC_MSG_ERROR([libxslt])
	fi])

# Call XSLT TEST func
LIBXSLTTEST

CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS"

AC_SUBST(WX_LIBS)

AC_ARG_ENABLE(debug,
[  --enable-debug,  Enable debug build],
[
  debugbuild="y"
  CXXFLAGS="${CXXFLAGS} -ggdb -O0"
])

# Check pcre is available
AC_CHECK_HEADER(pcre.h, ,
	AC_MSG_ERROR([PCRE headers not found]))

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

# Check enchant is available
PKG_CHECK_MODULES(ENCHANT, [enchant], [CXXFLAGS="$CXXFLAGS -DUSE_ENCHANT"], 
	# otherwise Check ASPELL is available
	  [AC_CHECK_HEADER(aspell.h, [ASPELL_LIBS="-laspell"],
		AC_MSG_ERROR([Aspell headers not found]))]
)

AC_SUBST(ASPELL_LIBS)

# Check gtk
# This has to match the one wxWidgets linked with
# It is needed to make single-instance-check work
PKG_CHECK_MODULES(GTK, [gtk+-2.0], [AC_MSG_RESULT([gtk+-2.0])],
	[CXXFLAGS="$CXXFLAGS -D__NO_GTK__"]
	#[PKG_CHECK_MODULES(GTK, [gtk+-3.0], [AC_MSG_RESULT([gtk+-3.0])])]
)

AC_OUTPUT(Makefile src/Makefile)

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

