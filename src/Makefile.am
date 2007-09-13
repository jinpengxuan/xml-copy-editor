# these are the headers for your project
noinst_HEADERS = *.h

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
	contexthandler.cpp globalreplacedialog.cpp housestyle.cpp \
	housestylereader.cpp myhtmlpane.cpp nocasecompare.cpp readfile.cpp \
	replace.cpp spellcheck.cpp styledialog.cpp wraplibxml.cpp \
	wrapregex.cpp wraptempfilename.cpp xmlassociatedtd.cpp xmlassociatexsd.cpp \
	xmlassociatexsl.cpp xmlctrl.cpp xmldoc.cpp xmlencodinghandler.cpp xmlfilterreader.cpp \
	xmlpromptgenerator.cpp xmlrulereader.cpp xmlschemalocator.cpp xmlutf8reader.cpp xsllocator.cpp \
	wrapexpat.cpp mypropertysheet.cpp rule.cpp housestylewriter.cpp myipc.cpp aboutdialog.cpp pathresolver.cpp mynotebook.cpp \
	insertpanel.cpp mytoolbar.cpp mymenubar.cpp xmlwordcount.cpp getword.cpp locationpanel.cpp catalogresolver.cpp getlinuxappdir.cpp \
	xmlparseschemans.cpp xmlshallowvalidator.cpp wrapxerces.cpp \
	findreplacepanel.cpp xmlschemaparser.cpp commandpanel.cpp \
	binaryfile.cpp xmlencodingspy.cpp \
	rulesets filters png rng xpm templates copying help po \
	docbook xmlcopyeditor.spec xmlcopyeditor.png custom.xpm \
	xmlcopyeditor.desktop
xmlcopyeditor_LDFLAGS = $(WX_LIBS) \
	-lexpat -lxslt -lxml2 -lpcre -lxerces-c

nobase_xmlcopyeditor_DATA = png/*.png \
	rulesets/*.* \
	filters/*.* \
	rng/*.rng \
	rng/*.rnc \
	rng/exclude/*.rng \
	rng/modules/*.rng \
	xpm/*.xpm \
	templates/*.* \
	help/*.* help/html/*.* \
	copying/*.txt copying/boost_shared_ptr/* \
	copying/Expat/* copying/libxml2/* copying/libxslt/* \
	copying/PCRE/* copying/Scintilla/* \
	copying/scowl/* \
	copying/wxCoolBar/* \
	copying/wxStyledTextCtrl/* copying/ximian/* \
	copying/wxStEditor/* \
	copying/xmlcopyeditor/* \
	copying/Apache/LICENSE-2.0 \
	copying/Apache/LICENSE-2_files/* \
	po/sk/messages.mo \
	po/sv/messages.mo \
	po/fr/messages.mo \
	po/de/messages.mo \
        po/zh_CN/messages.mo \
        po/zh_TW/messages.mo \
	po/uk_UA/messages.mo \
        po/uk_UA/messages.mo \
        po/en_us/messages.po \

pixmap_DATA = xmlcopyeditor.png

applications_DATA = xmlcopyeditor.desktop

INCLUDES = -I/usr/include/libxml2 

