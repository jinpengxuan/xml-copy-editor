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
xmlcopyeditor_SOURCES = aboutdialog.cpp \
	associatedialog.cpp \
	binaryfile.cpp \
	casehandler.cpp \
	catalogresolver.cpp \
	commandpanel.cpp \
	contexthandler.cpp \
	dtd2schema.cpp \
	exportdialog.cpp \
	findreplacepanel.cpp \
	getword.cpp \
	globalreplacedialog.cpp \
	housestyle.cpp \
	housestylereader.cpp \
	housestylewriter.cpp \
	insertpanel.cpp \
	locationpanel.cpp \
	mp3album.cpp \
	myhtmlpane.cpp \
	myipc.cpp \
	mynotebook.cpp \
	mypropertysheet.cpp \
	nocasecompare.cpp \
	pathresolver.cpp \
	readfile.cpp \
	replace.cpp \
	rule.cpp \
	styledialog.cpp \
	threadreaper.cpp \
	validationthread.cpp \
	wrapaspell.cpp \
	wrapdaisy.cpp \
	wrapexpat.cpp \
	wraplibxml.cpp \
	wrapregex.cpp \
	wraptempfilename.cpp \
	wrapxerces.cpp \
	xercescatalogresolver.cpp \
	xercesnetaccessor.cpp \
	xmlassociatedtd.cpp \
	xmlassociatexsd.cpp \
	xmlassociatexsl.cpp \
	xmlcopyeditor.cpp \
	xmlcopyimg.cpp \
	xmlctrl.cpp \
	xmldoc.cpp \
	xmlencodinghandler.cpp \
	xmlencodingspy.cpp \
	xmlfilterreader.cpp \
	xmlparseschemans.cpp \
	xmlprodnote.cpp \
	xmlpromptgenerator.cpp \
	xmlrulereader.cpp \
	xmlschemagenerator.cpp \
	xmlschemalocator.cpp \
	xmlshallowvalidator.cpp \
	xmlsuppressprodnote.cpp \
	xmltextinfo.cpp \
	xmlutf8reader.cpp \
	xmlwordcount.cpp \
	xsllocator.cpp \
	xmlcopyeditor.spec \
	xmlcopyeditor.png \
	custom.xpm

xmlcopyeditor_LDADD = $(WX_LIBS) \
	$(ASPELL_LIBS) \
	$(ENCHANT_LIBS) \
	$(GTK_LIBS) \
	-lexpat -lxslt -lxml2 -lpcre -lxerces-c

nobase_dist_xmlcopyeditor_DATA = $(srcdir)/catalog/catalog \
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

EXTRA_DIST = \
	$(srcdir)/aspell \
	$(srcdir)/bin \
	$(srcdir)/bmp \
	$(srcdir)/copying \
	$(srcdir)/dtd/docbook/4.4/ent/README \
	$(srcdir)/dtd/docbook/4.4/ChangeLog \
	$(srcdir)/dtd/docbook/4.4/README \
	$(srcdir)/ico \
	$(srcdir)/.cproject \
	$(srcdir)/.project \
	$(srcdir)/myprintout.cpp \
	$(srcdir)/xmlcopyeditor.iss \
	$(srcdir)/xmlcopyeditor64.iss \
	$(srcdir)/xmlcopyeditor.rc \
	$(srcdir)/xmlschemaparser.cpp

AM_CPPFLAGS = -I/usr/include/libxml2 $(ENCHANT_CFLAGS) $(GTK_CFLAGS)

