# these are the headers for your project
noinst_HEADERS = $(srcdir)/*.h

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = xmlcopyeditor
bindir = ${prefix}/bin
xmlcopyeditordir = ${prefix}/share/xmlcopyeditor
pixmapdir = /usr/share/pixmaps
applicationsdir = /usr/share/applications

# the application source, library search path, and link libraries
xmlcopyeditor_SOURCES = xmlcopyeditor.cpp associatedialog.cpp casehandler.cpp \
	contexthandler.cpp dtd2schema.cpp globalreplacedialog.cpp housestyle.cpp \
	housestylereader.cpp myhtmlpane.cpp nocasecompare.cpp readfile.cpp \
	replace.cpp styledialog.cpp threadreaper.cpp wraplibxml.cpp \
	wrapregex.cpp wraptempfilename.cpp xmlassociatedtd.cpp xmlassociatexsd.cpp \
	xmlassociatexsl.cpp xmlctrl.cpp xmldoc.cpp xmlencodinghandler.cpp \
	xmlfilterreader.cpp xmlpromptgenerator.cpp xmlrulereader.cpp \
	xmlschemalocator.cpp xmlutf8reader.cpp xsllocator.cpp \
	wrapexpat.cpp mypropertysheet.cpp rule.cpp housestylewriter.cpp myipc.cpp \
	aboutdialog.cpp pathresolver.cpp mynotebook.cpp insertpanel.cpp \
	xmlwordcount.cpp getword.cpp locationpanel.cpp catalogresolver.cpp \
	xmlparseschemans.cpp xmlshallowvalidator.cpp wrapxerces.cpp \
	findreplacepanel.cpp commandpanel.cpp xercescatalogresolver.cpp \
	binaryfile.cpp xmlencodingspy.cpp wrapaspell.cpp validationthread.cpp \
	wrapdaisy.cpp exportdialog.cpp mp3album.cpp xmlprodnote.cpp \
	xmlsuppressprodnote.cpp xmlcopyimg.cpp xmlschemagenerator.cpp \
	xmlcopyeditor.spec xmlcopyeditor.png custom.xpm \
	xmlcopyeditor.desktop

xmlcopyeditor_LDADD = $(WX_LIBS) \
	-lexpat -lxslt -lxml2 -lpcre -lxerces-c $(ASPELL_LIBS) $(ENCHANT_LIBS) $(GTK_LIBS)

nobase_dist_xmlcopyeditor_DATA = $(srcdir)/catalog/catalog \
	$(srcdir)/copying/*.txt \
	$(srcdir)/copying/Apache/LICENSE-2.0 \
	$(srcdir)/copying/Apache/LICENSE-2_files/* \
	$(srcdir)/copying/Aspell/* \
	$(srcdir)/copying/boost_shared_ptr/* \
	$(srcdir)/copying/Expat/* \
	$(srcdir)/copying/libxml2/* \
	$(srcdir)/copying/libxslt/* \
	$(srcdir)/copying/PCRE/* \
	$(srcdir)/copying/Scintilla/* \
	$(srcdir)/copying/wxStEditor/* \
	$(srcdir)/copying/wxStyledTextCtrl/* \
	$(srcdir)/copying/xmlcopyeditor/* \
	$(srcdir)/dtd/*.* \
	$(srcdir)/dtd/dita/*.* \
	$(srcdir)/dtd/docbook/4.4/*.* \
	$(srcdir)/dtd/docbook/4.4/ent/*.* \
	$(srcdir)/dtd/docbook/5.0/*.* \
	$(srcdir)/dtd/tei/p4/*.* \
	$(srcdir)/dtd/tei/p5/*.* \
	$(srcdir)/help/*.* \
	$(srcdir)/help/html/*.* \
	$(srcdir)/png/*.png \
	$(srcdir)/po/*/messages.mo \
	$(srcdir)/rng/*.rnc \
	$(srcdir)/rng/*.rng \
	$(srcdir)/rng/exclude/*.rng \
	$(srcdir)/rng/modules/*.rng \
	$(srcdir)/rulesets/*.* \
	$(srcdir)/templates/*.dtd \
	$(srcdir)/templates/*.html \
	$(srcdir)/templates/*.lzx \
	$(srcdir)/templates/*.rng \
	$(srcdir)/templates/*.rss \
	$(srcdir)/templates/*.xlf \
	$(srcdir)/templates/*.xml \
	$(srcdir)/templates/*.xsd \
	$(srcdir)/templates/*.xsl \
	$(srcdir)/templates/*.xtm \
	$(srcdir)/xpm/*.xpm \
	$(srcdir)/xsl/openlearn2daisyhtml/*.*

pixmap_DATA = xmlcopyeditor.png

applications_DATA = xmlcopyeditor.desktop

AM_CPPFLAGS = -I/usr/include/libxml2 $(ENCHANT_CFLAGS) $(GTK_CFLAGS)

