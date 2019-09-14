/*
 * Copyright 2018 Zane U. Ji.
 *
 * This file is part of Xml Copy Editor.
 *
 * Xml Copy Editor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * Xml Copy Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xml Copy Editor; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef XERCESNETACCESSOR_H_
#define XERCESNETACCESSOR_H_

#include <xercesc/util/XMLNetAccessor.hpp>

using namespace xercesc;

class XercesNetAccessor : public XMLNetAccessor
{
public:
	XercesNetAccessor ( XMLNetAccessor *netAccesor );

	XMLNetAccessor *getNetAccessor() const { return mNetAccessor; }

	virtual const XMLCh *getId() const;

	virtual BinInputStream *makeNew ( const XMLURL &urlSrc,
		const XMLNetHTTPInfo *httpInfo = 0);

protected:
	XMLNetAccessor *mNetAccessor;
};

#endif /* XERCESNETACCESSOR_H_ */
