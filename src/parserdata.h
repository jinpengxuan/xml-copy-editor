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

#ifndef PARSER_DATA_H
#define PARSER_DATA_H

#include <string>
#include <vector>

class ParserData
{
	public:
		ParserData()
		{
			state = 0;
			count = 0;
		}
		~ParserData()
		{ }
		void push ( const std::string& s )
		{
			stack.push_back ( s );
		}
		void pop()
		{
			stack.pop_back();
		}
		void setState ( int i )
		{
			state = i;
			++count;
		}
		int getState()
		{
			return state;
		}
		int getCount()
		{
			return count;
		}
		std::vector<std::string> getStack()
		{
			return stack;
		}
		std::string getElement()
		{
			return ( stack.empty() ) ? "" : stack.at ( stack.size() - 1 );
		}
		std::string getParent()
		{
			return ( stack.size() < 2 ) ? "" : stack.at ( stack.size() - 2 );
		}
	private:
		int state, count;
		std::vector<std::string> stack;
};

#endif
