#!/bin/sh
sudo checkinstall -D --requires libexpat1,libxslt1.1,libxml2,libpcre3,libxerces-c3.0,libwxgtk2.8-0 --maintainer "Gerald Schmidt \<gnschmidt@users.sourceforge.net\>" --pkgaltsource "http://xml-copy-editor.svn.sourceforge.net/viewvc/xml-copy-editor/" make install

