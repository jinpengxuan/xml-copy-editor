/*
 * Copyright 2005-2007 Gerald Schmidt.
 *
 * This file is part of Xml Copy Editor.
 *
 * Xml Copy Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#ifndef HOUSESTYLE_H
#define HOUSESTYLE_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <boost/shared_ptr.hpp>
#include "stringset.h"
#include "contexthandler.h"
#include "xmlrulereader.h"
#include "housestylereader.h"
#include "xmlfilterreader.h"
#include "wrapaspell.h"
#include "casehandler.h"

enum {
		HS_TYPE_SPELL = 0,
		HS_TYPE_STYLE
};

class HouseStyle
{
	public:
		HouseStyle (
		    int type,
		    const std::string& bufferParameter,
		    const wxString& ruleDirectoryParameter,
		    const wxString& ruleFileParameter,
		    const wxString& filterDirectoryParameter,
		    const wxString& filterFileParameter,
		    const wxString& pathSeparatorParameter,
#ifdef __WXMSW__
		    const wxString& aspellDataPathParameter,
		    const wxString& aspellDictPathParameter,
#endif
		    int contextRangeParameter );
		~HouseStyle();
		bool createReport();
		const wxString &getLastError();
		const std::vector<ContextMatch> &getMatchVector();
	private:
		int type;
		std::string buffer;
		wxString ruleDirectory
		       , ruleFile
		       , filterDirectory
		       , filterFile
		       , pathSeparator
		       , error
#ifdef __WXMSW__
               , aspellDataPath
               , aspellDictPath
#endif
               ;
		int contextRange;
		boost::shared_ptr<std::vector<boost::shared_ptr<Rule> > > ruleVector;
		std::map<std::string, std::map<std::string, std::set<std::string> > >
		    filterMap;
		std::vector<ContextMatch> matchVector;
		boost::shared_ptr<StringSet<char> > dictionary, passiveDictionary;
		void collectFilter (
		    const std::string& fileName,
		    std::set<std::string>& excludeSet,
		    int *filterCount );
		void collectRules (
		    const std::string& fileName,
		    boost::shared_ptr<std::vector<boost::shared_ptr<Rule> > > ruleVector,
		    std::set<string>& excludeSet,
		    int *ruleCount );
		int updateFilter();
		int updateRules();
};

#endif
