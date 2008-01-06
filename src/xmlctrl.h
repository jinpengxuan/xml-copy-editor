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

#ifndef XML_CTRL_H
#define XML_CTRL_H
#define DEFAULT_XML_DECLARATION L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
#define DEFAULT_XML_DECLARATION_UTF8 "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"

#include <wx/wx.h>
#include <wx/stc/stc.h>
#include <string>
#include <set>
#include <map>

struct XmlCtrlProperties
{
	bool completion;
	bool fold;
	bool number;
	bool currentLine;
	bool whitespaceVisible;
	bool wrap;
	bool indentLines;
	bool protectHiddenElements;
	bool toggleLineBackground;
	bool insertCloseTag;
	bool deleteWholeTag;
	bool validateAsYouType;
	bool highlightSyntax;
	int zoom, colorScheme;
	wxString font;
};

enum VisibilityStates
{
	SHOW_TAGS,
	HIDE_ATTRIBUTES,
	HIDE_TAGS
};

enum ColorSchemes
{
	COLOR_SCHEME_DEFAULT,
	COLOR_SCHEME_DEFAULT_BACKGROUND,
	COLOR_SCHEME_REDUCED_GLARE,
	COLOR_SCHEME_REDUCED_GLARE_BACKGROUND,
	COLOR_SCHEME_NONE
};

enum TagTypes
{
	TAG_TYPE_OPEN,
	TAG_TYPE_CLOSE,
	TAG_TYPE_EMPTY,
	TAG_TYPE_OTHER,
	TAG_TYPE_ERROR
};

enum BackgroundStates
{
	BACKGROUND_STATE_NORMAL,
	BACKGROUND_STATE_LIGHT
};

enum XmlFileTypes
{
	FILE_TYPE_XML,
	FILE_TYPE_DTD,
	FILE_TYPE_CSS,
	FILE_TYPE_PHP,
	FILE_TYPE_RNC,
	FILE_TYPE_BINARY
};

class XmlCtrl: public wxStyledTextCtrl
{
	public:
		XmlCtrl (
		    wxWindow *parent,
		    XmlCtrlProperties propertiesParameter,
		    bool *protectTagsParameter,
		    int visibilityStateParameter = SHOW_TAGS,
		    int typeParameter = FILE_TYPE_XML,
		    wxWindowID id = wxID_ANY,
		    //const std::string& buffer = DEFAULT_XML_DECLARATION_UTF8,
		    const char *buffer = NULL,
		    size_t bufferLen = 0,
		    const std::string& catalogPath = "",
		    const std::string& basePath = "",
		    const std::string& auxPath = "",
		    const wxPoint &position = wxDefaultPosition,
		    const wxSize& size = wxDefaultSize,
		    long style = 0 );
		~XmlCtrl()
		{
			attributeMap.clear();
			elementMap.clear();
			entitySet.clear();
		}
		int getType();
		int getParentCloseAngleBracket ( int pos, int range = USHRT_MAX * 4 );
		void applyProperties (
		    XmlCtrlProperties propertiesParameter,
		    bool zoomOnly = false );
		void applyVisibilityState ( int state = SHOW_TAGS );
		void updatePromptMaps();
		void updatePromptMaps ( const char *buffer, size_t bufferLen );
		void adjustCursor();
		void adjustSelection();
		void foldAll();
		void unfoldAll();
		void toggleFold();
		bool insertChild ( const wxString& child );
		bool insertSibling ( const wxString& sibling, const wxString& parent );
		bool insertEntity ( const wxString& entity );
		bool getGrammarFound();
		void setErrorIndicator ( int line, int column );
		void clearErrorIndicators ( int maxLine = 0 );
		wxString getParent();
		wxString getLastElementName ( int pos );
		std::set<wxString> getChildren ( const wxString& parent );
		std::set<std::string> getEntitySet();
		bool canInsertAt ( int pos );
		int getTagStartPos ( int pos );
		void toggleLineBackground();
		bool shallowValidate ( int maxLine = 0, bool segmentOnly = false );
		bool shallowValidate ( const char *buffer, size_t bufferLen,
		                       int startLine = 0,
		                       int maxLine = 0,
		                       int columnOffset = 0,
		                       bool segmentOnly = false );
		std::string myGetTextRaw(); // alternative to faulty stc implementation
		bool getValidationRequired();
		void setValidationRequired ( bool b );
	private:
		int type;
		bool *protectTags;
		bool validationRequired, grammarFound;
		int visibilityState;
		int controlState;
		int currentMaxLine;
		int lineBackgroundState;
		wxColour baseBackground, alternateBackground;
		std::map<std::string, std::map<std::string, std::set<std::string> > >
		attributeMap;
		std::map<std::string, std::set<std::string> > requiredAttributeMap;
		std::map<std::string, std::set<std::string> > elementMap;
		std::set<std::string> entitySet;
		std::string catalogPath, basePath, auxPath;
		XmlCtrlProperties properties;
		wxString getLastAttributeName ( int pos );
		int getAttributeStartPos ( int pos );
		int getAttributeSectionEndPos ( int pos, int range = USHRT_MAX );
		int getTagType ( int pos );
		int getLexerStyleAt ( int pos );
		bool isCloseTag ( int pos );
		bool canMoveLeftAt ( int pos );
		bool canMoveRightAt ( int pos );
		wxString getOpenTag ( const wxString& element );
		void handleOpenAngleBracket ( wxKeyEvent& event );
		void handleCloseAngleBracket ( wxKeyEvent& event );
		void handleEquals ( wxKeyEvent& event );
		void handleSpace ( wxKeyEvent& event );
		void handleAmpersand ( wxKeyEvent& event );
		void handleForwardSlash ( wxKeyEvent& event );
		void handleBackspace ( wxKeyEvent& event );
		void handleDelete ( wxKeyEvent& event );
		void OnMarginClick ( wxStyledTextEvent& event );
		void OnChar ( wxKeyEvent& event );
		void OnIdle ( wxIdleEvent& event );
		void OnKeyPressed ( wxKeyEvent& event );
		void OnMouseLeftDown ( wxMouseEvent& event );
		void OnMouseLeftUp ( wxMouseEvent& event );
		void OnMouseRightUp ( wxMouseEvent& event );
		void OnMiddleDown ( wxMouseEvent& event );
		void insertNewLine();
		void adjustNoColumnWidth();
		void adjustPosRight();
		void adjustPosLeft();
		void setColorScheme ( int scheme );
		void expandFoldsToLevel ( int level, bool expand );
		void protectHeadLine();

		DECLARE_NO_COPY_CLASS ( XmlCtrl )
		DECLARE_EVENT_TABLE()
};

#endif
