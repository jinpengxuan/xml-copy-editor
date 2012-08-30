/*
 * Copyright 2012 Zane U. Ji.
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

#ifndef THREADREAPER_H_
#define THREADREAPER_H_

#include <wx/wx.h>
#include <vector>
#include <boost/shared_ptr.hpp>

class ThreadReaper
{
protected:
	ThreadReaper();
	virtual ~ThreadReaper();

public:
	static ThreadReaper &get();

	void add ( wxThread *thread );
	// Kills all threads in the list.
	void clear();

protected:
	std::vector<boost::shared_ptr<wxThread> > mList;
};

#endif /* THREADREAPER_H_ */
