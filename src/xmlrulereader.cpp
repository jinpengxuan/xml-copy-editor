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

#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include "xmlrulereader.h"
#include "rule.h"
#include "stringset.h"

using namespace std;

RuleData::RuleData (
    boost::shared_ptr<StringSet<char> > dictionaryParameter,
    boost::shared_ptr<StringSet<char> > passiveDictionaryParameter,
    boost::shared_ptr<vector<boost::shared_ptr<Rule> > > ruleVectorParameter ) :
		dictionary ( dictionaryParameter ),
		passiveDictionary ( passiveDictionaryParameter ),
		ruleVector ( ruleVectorParameter )
{
	cipher = false;
	dictionaryFound = false;
	initialiseAttributes();
	ruleCount = 0;
}

RuleData::~RuleData()
{ }

XmlRuleReader::XmlRuleReader (
    boost::shared_ptr<StringSet<char> > dictionaryParameter,
    boost::shared_ptr<StringSet<char> > passiveDictionaryParameter,
    boost::shared_ptr<vector<boost::shared_ptr<Rule> > > ruleVectorParameter ) :
		ud ( new RuleData (
		         dictionaryParameter,
		         passiveDictionaryParameter,
		         ruleVectorParameter ) )
{
	ud->setState ( STATE_UNKNOWN );
	ud->p = p;

	XML_SetUserData ( p, ud.get() );
	XML_SetElementHandler ( p, start, end );
	XML_SetCharacterDataHandler ( p, characterdata );
}

XmlRuleReader::~XmlRuleReader()
{}

int XmlRuleReader::getRuleCount()
{
	return ud->ruleCount;
}

string XmlRuleReader::getIncorrectPatternReport()
{
	return ud->incorrectPatternReport;
}

void XmlRuleReader::getExcludeVector ( vector<string> &v )
{
	v = ud->excludeVector;
}

void XmlRuleReader::getIncludeVector ( vector<string> &v )
{
	v = ud->includeVector;
}

void XMLCALL XmlRuleReader::start (
    void *data,
    const XML_Char *el,
    const XML_Char **attr )
{
	RuleData *ud;
	ud = ( RuleData * ) data;

	if ( !strcmp ( el, "rule" ) )
		ud->setState ( STATE_IN_RULE );
	else if ( !strcmp ( el, "term" ) )
	{
		ud->setState ( STATE_IN_TERM );
		while ( *attr )
		{
			if ( !strcmp ( *attr, "passive" ) && !strcmp ( * ( attr + 1 ), "true" ) )
				ud->passive = true;
			attr += 2;
		}
	}
	else if ( !strcmp ( el, "find" ) )
	{
		while ( *attr )
		{
			if ( !strcmp ( *attr, "matchcase" ) && !strcmp ( * ( attr + 1 ), "true" ) )
				ud->matchcase = true;
			else if ( !strcmp ( *attr, "cipher" ) && !strcmp ( * ( attr + 1 ), "true" ) )
				ud->cipher = true;
			attr += 2;
		}
		ud->setState ( STATE_IN_FIND );
	}
	else if ( !strcmp ( el, "replace" ) )
	{
		while ( *attr )
		{
			if ( !strcmp ( *attr, "adjustcase" ) && !strcmp ( * ( attr + 1 ), "true" ) )
				ud->adjustcase = true;
			else if ( !strcmp ( *attr, "tentative" ) && !strcmp ( * ( attr + 1 ), "true" ) )
				ud->tentative = true;
			attr += 2;
		}
		ud->setState ( STATE_IN_REPLACE );
	}
	else if ( !strcmp ( el, "report" ) )
		ud->setState ( STATE_IN_REPORT );
	else if ( !strcmp ( el, "exclude" ) )
		ud->setState ( STATE_IN_EXCLUDE );
	else if ( !strcmp ( el, "include" ) )
		ud->setState ( STATE_IN_INCLUDE );
	else if ( !strcmp ( el, "title" ) )
		ud->setState ( STATE_IN_TITLE );
	else
		ud->setState ( STATE_UNKNOWN );
}

void XMLCALL XmlRuleReader::end ( void *data, const XML_Char *el )
{
	RuleData *ud;
	ud = ( RuleData * ) data;

	if ( !strcmp ( el, "term" ) )
	{
		if ( ud->term != "" )
		{
			ud->dictionary->insert ( ud->term );
			ud->dictionaryFound = true;
			if ( ud->passive )
			{
				ud->passiveDictionary->insert ( ud->term );
				ud->passive = false;
			}

			ud->term = "";
		}
		ud->setState ( STATE_UNKNOWN );
	}
	// handle end of rule
	else if ( !strcmp ( el, "rule" ) )
	{
		try
		{
			boost::shared_ptr<Rule> rule ( new Rule (
			                                   ud->find,
			                                   ud->matchcase,
			                                   ud->replace ) );

			string report = ud->title;
			if ( ud->report != "" )
			{
				report += ": ";
				report += ud->report;
			}
			rule->setReport ( report );

			rule->setTentativeAttribute ( ud->tentative );
			rule->setAdjustCaseAttribute ( ud->adjustcase );
			ud->ruleVector->push_back ( rule );
			++ ( ud->ruleCount );

			ud->find = "";
			ud->replace = "";
			ud->report = "";
			ud->setState ( STATE_UNKNOWN );
			ud->initialiseAttributes();
		}
		catch ( exception& e )
		{
			ud->incorrectPatternReport = "Cannot compile: " +
			                             ud->find +
			                             "\r\nError: " +
			                             e.what();
			XML_StopParser ( ud->p, XML_FALSE );
		}
	}
	else if ( !strcmp ( el, "find" ) )
		ud->setState ( STATE_UNKNOWN );
	else if ( !strcmp ( el, "replace" ) )
		ud->setState ( STATE_UNKNOWN );
	else if ( !strcmp ( el, "report" ) )
		ud->setState ( STATE_UNKNOWN );

	// handle excludes/includes
	else if ( !strcmp ( el, "exclude" ) )
	{
		ud->excludeVector.push_back ( ud->exclude );
		ud->exclude = "";
		ud->setState ( STATE_UNKNOWN );
	}
	else if ( !strcmp ( el, "include" ) )
	{
		ud->includeVector.push_back ( ud->include );
		ud->include = "";
		ud->setState ( STATE_UNKNOWN );
	}
	else if ( !strcmp ( el, "title" ) )
		ud->setState ( STATE_UNKNOWN );

	// count each dictionary as one rule
	else if ( !strcmp ( el, "dictionary" ) )
	{
		if ( ud->dictionaryFound )
			++ ( ud->ruleCount );
	}
}

void XMLCALL XmlRuleReader::characterdata (
    void *data,
    const XML_Char *s,
    int len )
{
	RuleData *ud;
	ud = ( RuleData * ) data;

	switch ( ud->getState() )
	{
		case STATE_IN_FIND:
			ud->find.append ( s, len );
			break;
		case STATE_IN_REPLACE:
			ud->replace.append ( s, len );
			break;
		case STATE_IN_REPORT:
			ud->report.append ( s, len );
			break;
		case STATE_IN_EXCLUDE:
			ud->exclude.append ( s, len );
			break;
		case STATE_IN_INCLUDE:
			ud->include.append ( s, len );
			break;
		case STATE_IN_TITLE:
			ud->title.append ( s, len );
			break;
		case STATE_IN_TERM:
			ud->term.append ( s, len );
			break;
		default:
			break;
	}
}

void RuleData::initialiseAttributes()
{
	matchcase = adjustcase = tentative = passive = false;
}
