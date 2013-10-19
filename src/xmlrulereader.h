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

#ifndef XML_RULE_READER_H
#define XML_RULE_READER_H

#include <set>
#include <string>
#include <vector>
#include <expat.h>
#include <boost/shared_ptr.hpp>
#include "wrapexpat.h"
#include "rule.h"
#include "stringset.h"

using namespace std;

class RuleData : public ParserData
{
	public:
		RuleData (
		    boost::shared_ptr<StringSet<char> > dictionaryParameter,
		    boost::shared_ptr<StringSet<char> > passiveDictionaryParameter,
		    boost::shared_ptr<vector<boost::shared_ptr<Rule> > > ruleVectorParameter );
		~RuleData();

		XML_Parser p;
		string title, find, replace, report, include, exclude, term;
		string incorrectPatternReport;
		bool matchcase, cipher, adjustcase, tentative, passive;
		bool dictionaryFound;
		int ruleCount;
		vector<string> excludeVector;
		vector<string> includeVector;
		boost::shared_ptr<StringSet<char> > dictionary, passiveDictionary;
		boost::shared_ptr<vector<boost::shared_ptr<Rule> > > ruleVector;
		void initialiseAttributes();
};

class XmlRuleReader : public WrapExpat
{
	public:
		XmlRuleReader (
		    boost::shared_ptr<StringSet<char> > dictionary,
		    boost::shared_ptr<StringSet<char> > passiveDictionary,
		    boost::shared_ptr<vector<boost::shared_ptr<Rule> > > ruleVector );
		virtual ~XmlRuleReader();

		enum
		{
			STATE_UNKNOWN,
			STATE_IN_RULE,
			STATE_IN_FIND,
			STATE_IN_REPLACE,
			STATE_IN_REPORT,
			STATE_IN_INCLUDE,
			STATE_IN_EXCLUDE,
			STATE_IN_TITLE,
			STATE_IN_TERM
		};

		int getRuleCount();
		string getIncorrectPatternReport();
		void getExcludeVector ( vector<string> &v );
		void getIncludeVector ( vector<string> &v );
	private:
		std::auto_ptr<RuleData> ud;
		boost::shared_ptr<StringSet<char> > dictionary, passiveDictionary;
		boost::shared_ptr<vector<boost::shared_ptr<Rule> > > ruleVector;

		static void XMLCALL start (
		    void *data,
		    const XML_Char *el,
		    const XML_Char **attr );
		static void XMLCALL end ( void *data, const XML_Char *el );
		static void XMLCALL characterdata ( void *data, const XML_Char *s, int len );
};

#endif
