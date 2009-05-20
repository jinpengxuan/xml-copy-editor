# these are the headers for your project
noinst_HEADERS = $(srcdir)/*.h

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = xmlcopyeditor
bindir = ${prefix}/bin
xmlcopyeditordir = ${datarootdir}/xmlcopyeditor
pixmapdir = ${datarootdir}/pixmaps
desktopdir = ${datarootdir}/applications

# the application source, library search path, and link libraries
xmlcopyeditor_SOURCES = xmlcopyeditor.cpp associatedialog.cpp casehandler.cpp \
	contexthandler.cpp globalreplacedialog.cpp housestyle.cpp \
	housestylereader.cpp myhtmlpane.cpp nocasecompare.cpp readfile.cpp \
	replace.cpp styledialog.cpp wraplibxml.cpp \
	wrapregex.cpp wraptempfilename.cpp xmlassociatedtd.cpp xmlassociatexsd.cpp \
	xmlassociatexsl.cpp xmlctrl.cpp xmldoc.cpp xmlencodinghandler.cpp xmlfilterreader.cpp \
	xmlpromptgenerator.cpp xmlrulereader.cpp xmlschemalocator.cpp xmlutf8reader.cpp xsllocator.cpp \
	wrapexpat.cpp mypropertysheet.cpp rule.cpp housestylewriter.cpp myipc.cpp aboutdialog.cpp pathresolver.cpp mynotebook.cpp \
	insertpanel.cpp xmlwordcount.cpp getword.cpp locationpanel.cpp catalogresolver.cpp getlinuxappdir.cpp \
	xmlparseschemans.cpp xmlshallowvalidator.cpp wrapxerces.cpp \
	findreplacepanel.cpp commandpanel.cpp xercescatalogresolver.cpp \
	binaryfile.cpp xmlencodingspy.cpp wrapaspell.cpp validationthread.cpp \
	xmlcopyeditor.spec xmlcopyeditor.png custom.xpm \
	xmlcopyeditor.desktop
#       rulesets png rng xpm templates copying help po 

xmlcopyeditor_LDFLAGS = $(WX_LIBS) \
	-lexpat -lxslt -lxml2 -lpcre -lxerces-c -laspell

nobase_xmlcopyeditor_DATA = $(srcdir)/png/*.png\
	$(srcdir)/rulesets/*.* \
	$(srcdir)/rng/*.rng \
	$(srcdir)/rng/*.rnc \
	$(srcdir)/rng/exclude/*.rng \
	$(srcdir)/rng/modules/*.rng \
	$(srcdir)/xpm/*.xpm \
	$(srcdir)/templates/*.xml \
	$(srcdir)/templates/*.dtd \
	$(srcdir)/templates/*.xsl \
	$(srcdir)/templates/*.rss \
	$(srcdir)/templates/*.html \
	$(srcdir)/templates/*.xtm \
	$(srcdir)/templates/*.xsd \
	$(srcdir)/templates/*.rng \
	$(srcdir)/templates/*.lzx \
	$(srcdir)/templates/*.xlf \
	$(srcdir)/help/*.* $(srcdir)/help/html/*.* \
	$(srcdir)/copying/*.txt $(srcdir)/copying/boost_shared_ptr/* \
	$(srcdir)/copying/Expat/* $(srcdir)/copying/libxml2/* $(srcdir)/copying/libxslt/* \
	$(srcdir)/copying/PCRE/* $(srcdir)/copying/Scintilla/* \
	$(srcdir)/copying/wxStyledTextCtrl/* \
	$(srcdir)/copying/wxStEditor/* \
	$(srcdir)/copying/xmlcopyeditor/* \
	$(srcdir)/copying/Aspell/* \
	$(srcdir)/copying/Apache/LICENSE-2.0 \
	$(srcdir)/copying/Apache/LICENSE-2_files/* \
	$(srcdir)/po/sk/messages.mo \
	$(srcdir)/po/sv/messages.mo \
	$(srcdir)/po/fr/messages.mo \
	$(srcdir)/po/de/messages.mo \
	$(srcdir)/po/ru/messages.mo \
	$(srcdir)/po/es/messages.mo \
        $(srcdir)/po/zh_CN/messages.mo \
        $(srcdir)/po/zh_TW/messages.mo \
	$(srcdir)/po/uk_UA/messages.mo \
        $(srcdir)/po/uk_UA/messages.mo \
        $(srcdir)/po/en_us/messages.po

pixmap_DATA = xmlcopyeditor.png

desktop_DATA = xmlcopyeditor.desktop

INCLUDES = -I/usr/include/libxml2 

