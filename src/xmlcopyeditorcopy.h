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

#define ENCODING_INFO _T("UTF-8, UTF-16, UTF-16LE, UTF-16BE, ISO-8859-1, US-ASCII")
#ifdef __WXMSW__
#define FILE_FILTER _("All files (*.*)|*.*|XML (*.xml)|*.xml|XHTML (*.html)|*.html|DTD (*.dtd)|*.dtd|XML Schema (*.xsd)|*.xsd|RELAX NG grammar (*.rng)|*.rng|XSL (*.xsl)|*.xsl")
#else
#define FILE_FILTER _("All files (*)|*|XML (*.xml)|*.xml|XHTML (*.html)|*.html|DTD (*.dtd)|*.dtd|XML Schema (*.xsd)|*.xsd|RELAX NG grammar (*.rng)|*.rng|XSL (*.xsl)|*.xsl")

#endif
#define ABOUT_COPYRIGHT _("Copyright © 2005-2009 Gerald Schmidt <gnschmidt@users.sourceforge.net>")
#define ABOUT_DESCRIPTION _("\nXML Copy Editor is free software released under the GNU\nGeneral Public License.\n\nMany thanks are due to Tim van Niekerk, Matt Smigielski,\nDavid Scholl, Jan Merka, Marcus Bingenheimer, Roberto\nRosselli Del Turco, Ken Zalewski, C.J. Meidlinger,\nThomas Zajic, Viliam Búr, David Håsäther, François\nBadier, Thomas Wenzel, Roger Sperberg, SHiNE CsyFeK,\nHSU PICHAN, YANG SHUFUN, CHENG PAULIAN,\nCHUANG KUO-PING, Justin Dearing, Serhij Dubyk,\nAntonio Angelo, Jose Luis Rivero, Siarhei Kuchuk,\nIan Abbott, Kev James, Marcos Pérez González, Anh\nTrinh and Rob Elemans.")
#define ABOUT_LICENSE _T(\
"This program is free software; you can redistribute it\n"\
"and/or modify it under the terms of the GNU General Public\n"\
"License as published by the Free Software Foundation;\n"\
"either version 2 of the License, or (at your option) any\n"\
"later version.\n\n"\
"This program is distributed in the hope that it will be\n"\
"useful, but WITHOUT ANY WARRANTY; without even the implied\n"\
"warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR\n"\
"PURPOSE. See the GNU General Public License for more\n"\
"details.\n\n"\
"You should have received a copy of the GNU General Public\n"\
"License along with this program; if not, write to the Free\n"\
"Software Foundation, Inc., 59 Temple Place, Suite 330,\n"\
"Boston, MA 02111-1307 USA.")
#define ABOUT_VERSION _T("1.2.0.6")
#define XMLCE_VAR _T("XMLCE_VAR")
