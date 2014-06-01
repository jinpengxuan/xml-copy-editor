NULL =

desktopdir = $(datadir)/applications
desktop_in_files = xmlcopyeditor.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

appdatadir = $(datadir)/appdata
appdata_in_files = xmlcopyeditor.appdata.xml.in
appdata_DATA = $(appdata_in_files:.xml.in=.xml)
@INTLTOOL_XML_RULE@

EXTRA_DIST = \
	$(appdata_in_files) \
	$(desktop_in_files)

CLEANFILES = \
	$(appdata_DATA) \
	$(desktop_DATA)

check-local: $(appdata_DATA) $(desktop_DATA)
	$(APPDATA_VALIDATE) $(appdata_DATA)
	$(DESKTOP_FILE_VALIDATE) $(desktop_DATA)

