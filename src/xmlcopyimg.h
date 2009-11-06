#ifndef XML_COPYIMG_H
#define XML_COPYIMG_H

#include <vector>
#include <expat.h>
#include <string>
#include <memory>
#include "wrapexpat.h"

struct ImgData : public ParserData
{
    wxString path, blankImage, imagesDir, mediaDir;
    std::string buffer, encoding;
    std::vector<std::string> tempFiles;
};

class XmlCopyImg : public WrapExpat
{
	public:
		XmlCopyImg (
		    const wxString& blankImage,
		    const wxString& imagesDir,
		    const wxString& mediaDir,
		    const wxString& path,
		    bool parseDeclaration = false,
		    bool expandInternalEntities = true,
		    size_t size = BUFSIZ
		);
		virtual ~XmlCopyImg();
		std::string getBuffer()
		{
			return d->buffer;
		}
		std::string getEncoding()
		{
			return d->encoding;
		}
	private:
		std::auto_ptr<ImgData> d;
		std::string encoding, element, attribute;
		static void XMLCALL xmldeclhandler (
		    void *data,
		    const XML_Char *version,
		    const XML_Char *encoding,
		    int standalone );
		static void XMLCALL defaulthandler (
		    void *data,
		    const XML_Char *s,
		    int len );
		static void XMLCALL start ( void *data, const XML_Char *el, const XML_Char **attr );
		static void XMLCALL end ( void *data, const XML_Char *el );
};

#endif
