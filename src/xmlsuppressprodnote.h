#ifndef XML_SUPPRESS_PRODNOTE_H
#define XML_SUPPRESS_PRODNOTE_H

#include <cstdio>
#include <vector>
#include <expat.h>
#include <string>
#include <memory>
#include "wrapexpat.h"

struct SuppressProdnoteData : public ParserData
{
    int level;
    bool skip;
    std::string encoding, buffer;
};

class XmlSuppressProdnote : public WrapExpat
{
	public:
		XmlSuppressProdnote (
		    bool parseDeclaration = false,
		    bool expandInternalEntities = true,
		    size_t size = BUFSIZ
		);
		virtual ~XmlSuppressProdnote();
		std::string getBuffer()
		{
			return d->buffer;
		}
		std::string getEncoding()
		{
			return d->encoding;
		}
	private:
		std::auto_ptr<SuppressProdnoteData> d;
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
