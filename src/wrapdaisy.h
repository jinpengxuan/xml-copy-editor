/*
 * Copyright 2005-2009 Gerald Schmidt.
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

#ifndef WRAPDAISY_H
#define WRAPDAISY_H

#include <wx/wx.h>
#include "xmlcopyeditor.h"

class WrapDaisy {
public:
  WrapDaisy(MyFrame *frame, const wxString &daisyDir, const wxString &path);
  ~WrapDaisy();
  bool run(wxString &fileIn, wxString &stylesheet, wxString &folder, bool quiet,
           bool suppressOptional, bool epub, bool rtf, bool doc, bool fullDaisy,
           bool mp3Album);
  const wxString &getLastError();

private:
  MyFrame *frame;
  wxString daisyDir, path, blankImage, classPath, commandLineUI, baseCmd, error,
      memoryCwd, daisyCwd, albumCover;
};

#endif
