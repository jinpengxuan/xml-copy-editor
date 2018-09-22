build:
	`wx-config --cxx --cxxflags --libs all` -I /usr/include -I /usr/local/include -I /usr/include/libxml2 -I ../src -lxml2 -lxslt -laspell.15 -lxerces-c -lpcre -o xmlcopyeditor ../src/*.cpp
bundle: Info.plist xmlcopyeditor version.plist InfoPlist.strings xmlcopyeditor.icns
	SetFile -t APPL xmlcopyeditor
	-mkdir xmlcopyeditor.app
	-mkdir xmlcopyeditor.app/Contents
	-mkdir xmlcopyeditor.app/Contents/MacOS
	-mkdir xmlcopyeditor.app/Contents/Resources
	-mkdir xmlcopyeditor.app/Contents/Resources/English.lproj
	-mkdir xmlcopyeditor.app/Contents/SharedSupport
	-mkdir xmlcopyeditor.app/Contents/Frameworks
	cp Info.plist xmlcopyeditor.app/Contents/
	cp version.plist xmlcopyeditor.app/Contents/
	cp InfoPlist.strings xmlcopyeditor.app/Contents/Resources/English.lproj/
	echo -n 'APPLxmlc' > xmlcopyeditor.app/Contents/PkgInfo
	cp xmlcopyeditor xmlcopyeditor.app/Contents/MacOS/xmlcopyeditor
	cp -R ../src/png xmlcopyeditor.app/Contents/SharedSupport/
	cp -R ../src/catalog xmlcopyeditor.app/Contents/SharedSupport/
	cp -R ../src/dtd xmlcopyeditor.app/Contents/SharedSupport/
	cp -R ../src/help xmlcopyeditor.app/Contents/SharedSupport/
	cp -R ../src/ico xmlcopyeditor.app/Contents/SharedSupport/
	cp -R ../src/rng xmlcopyeditor.app/Contents/SharedSupport/
	cp -R ../src/rulesets xmlcopyeditor.app/Contents/SharedSupport/
	cp -R ../src/templates xmlcopyeditor.app/Contents/SharedSupport/
	cp -R ../src/xpm xmlcopyeditor.app/Contents/SharedSupport/
	cp -R ../src/xsl xmlcopyeditor.app/Contents/SharedSupport/
	cp xmlcopyeditor.icns xmlcopyeditor.app/Contents/Resources/
	# process xmlcopyeditor
	for DYLIB in lib/libxml2.2 lib/libxslt.1 lib/libpcre.0 local/lib/libaspell.15 local/lib/libxerces-c-3.1 local/lib/libpng15.15 local/lib/libjpeg.8 local/lib/libtiff.5 lib/libexpat.1 lib/libz.1 lib/libiconv.2; do BASENAME=`basename /usr/$${DYLIB}.dylib`; echo "relinking $${BASENAME}"; cp /usr/$${DYLIB}.dylib xmlcopyeditor.app/Contents/Frameworks/; install_name_tool -change /usr/$${DYLIB}.dylib @executable_path/../Frameworks/$${BASENAME} xmlcopyeditor.app/Contents/MacOS/xmlcopyeditor; done
	chmod +w xmlcopyeditor.app/Contents/Frameworks/*.dylib
	#process libtiff
	for DYLIB in local/lib/libjpeg.8 lib/libz.1; do BASENAME=`basename /usr/$${DYLIB}.dylib`; echo "relinking $${BASENAME}"; install_name_tool -change /usr/$${DYLIB}.dylib @executable_path/../Frameworks/$${BASENAME} xmlcopyeditor.app/Contents/Frameworks/libtiff.5.dylib; done
	#process libxslt
	for DYLIB in lib/libxml2.2; do BASENAME=`basename /usr/$${DYLIB}.dylib`; echo "relinking $${BASENAME}"; install_name_tool -change /usr/$${DYLIB}.dylib @executable_path/../Frameworks/$${BASENAME} xmlcopyeditor.app/Contents/Frameworks/libxslt.1.dylib; done
	#process libxerces-c
	for DYLIB in lib/libcurl.4; do BASENAME=`basename /usr/$${DYLIB}.dylib`; echo "relinking $${BASENAME}"; cp /usr/$${DYLIB}.dylib xmlcopyeditor.app/Contents/Frameworks/; install_name_tool -change /usr/$${DYLIB}.dylib @executable_path/../Frameworks/$${BASENAME} xmlcopyeditor.app/Contents/Frameworks/libxerces-c-3.1.dylib; done
	#process libcurl
	for DYLIB in lib/libcrypto.35 lib/libssl.35 lib/libapple_nghttp2; do BASENAME=`basename /usr/$${DYLIB}.dylib`; echo "relinking $${BASENAME}"; cp /usr/$${DYLIB}.dylib xmlcopyeditor.app/Contents/Frameworks/; install_name_tool -change /usr/$${DYLIB}.dylib @executable_path/../Frameworks/$${BASENAME} xmlcopyeditor.app/Contents/Frameworks/libcurl.4.dylib; done
dmg:
	make -f Makefile.diskimage
clean:
	rm -f xmlcopyeditor
	rm -rf xmlcopyeditor.app/
	make -f Makefile.diskimage clean
all: clean build bundle dmg
