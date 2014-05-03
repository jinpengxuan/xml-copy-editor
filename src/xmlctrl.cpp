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

#include "xmlctrl.h"
#include "xmlpromptgenerator.h"
#include "xmlshallowvalidator.h"
#include "xmlencodinghandler.h"
//#include "wrapxerces.h"
#include "xmlcopyeditor.h" // needed to enable validation-as-you-type alerts
#include <utility>
#include <memory>
#include "validationthread.h"


// adapted from wxSTEdit (c) 2005 John Labenski, Otto Wyss
#define XMLCTRL_HASBIT(value, bit) (((value) & (bit)) != 0)

BEGIN_EVENT_TABLE ( XmlCtrl, wxStyledTextCtrl )
	EVT_CHAR ( XmlCtrl::OnChar )
	EVT_KEY_DOWN ( XmlCtrl::OnKeyPressed )
	EVT_IDLE ( XmlCtrl::OnIdle )
	EVT_STC_MARGINCLICK ( wxID_ANY, XmlCtrl::OnMarginClick )
	EVT_LEFT_DOWN ( XmlCtrl::OnMouseLeftDown )
	EVT_LEFT_UP ( XmlCtrl::OnMouseLeftUp )
	EVT_RIGHT_UP ( XmlCtrl::OnMouseRightUp )
	EVT_MIDDLE_DOWN ( XmlCtrl::OnMiddleDown )
	EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_VALIDATION_COMPLETED, XmlCtrl::OnValidationCompleted)
	EVT_NOTIFY ( myEVT_NOTIFY_PROMPT_GENERATED, wxID_ANY, XmlCtrl::OnPromptGenerated )
END_EVENT_TABLE()

// global protection for validation threads
wxCriticalSection xmlcopyeditorCriticalSection;

XmlCtrl::XmlCtrl (
    wxWindow *parent,
    const XmlCtrlProperties &propertiesParameter,
    bool *protectTagsParameter,
    int visibilityStateParameter,
    int typeParameter,
    wxWindowID id,
    const char *buffer, // could be NULL
    size_t bufferLen,
    const wxString& basePathParameter,
    const wxString& auxPathParameter,
    const wxPoint& position,
    const wxSize& size,
    long style
    )
	: wxStyledTextCtrl ( parent, id, position, size, style )
	, type ( typeParameter )
	, protectTags ( protectTagsParameter )
	, visibilityState ( visibilityStateParameter )
	, basePath ( basePathParameter )
	, auxPath ( auxPathParameter )
{
	validationThread = NULL;
	mPromptGeneratorThread = NULL;

	grammarFound = false;
	validationRequired = (buffer) ? true : false; // NULL for plain XML template

	currentMaxLine = 1;

	applyProperties ( propertiesParameter );

	SetTabWidth ( 2 );
	SetWrapStartIndent ( 2 );
	SetWrapVisualFlags ( wxSTC_WRAPVISUALFLAG_START );
	SetUseTabs ( false );
	SetBackSpaceUnIndents ( true );
	SetTabIndents ( false );
	SetUndoCollection ( false );

	// handle NULL buffer
	if ( !buffer )
	{
		buffer = DEFAULT_XML_DECLARATION_UTF8;
		bufferLen = strlen ( DEFAULT_XML_DECLARATION_UTF8 );
	}

#if wxCHECK_VERSION(2,9,0)
	if ( type != FILE_TYPE_BINARY )
		SetScrollWidthTracking ( true );
#endif

#if wxCHECK_VERSION(2,9,0)
	AddTextRaw ( buffer, bufferLen );
#else
	SendMsg ( 2001, bufferLen, ( wxIntPtr ) buffer );
#endif

	SetSelection ( 0, 0 );

	// position cursor
	if ( type == FILE_TYPE_XML &&
	        bufferLen > 5 &&
	        buffer[0] == '<' &&
	        buffer[1] == '?' &&
	        buffer[2] == 'x' &&
	        buffer[3] == 'm' &&
	        buffer[4] == 'l' &&
	        GetLineCount() > 1 )
	{
		GotoLine ( 1 ); // == line 2 of the document
	}


	SetSavePoint();
	SetUndoCollection ( true );
	AutoCompSetSeparator ( '<' );

	applyVisibilityState ( visibilityState );
	lineBackgroundState = BACKGROUND_STATE_NORMAL;

	for ( int i = 0; i < wxSTC_INDIC_MAX; ++i )
		IndicatorSetStyle ( i, wxSTC_INDIC_HIDDEN );
	IndicatorSetStyle ( 2, wxSTC_INDIC_SQUIGGLE );
	IndicatorSetForeground ( 2, *wxRED );
#if wxCHECK_VERSION(2,9,0)
	SetIndicatorCurrent ( 2 );
#endif
}


XmlCtrl::~XmlCtrl()
{
	attributeMap.clear();
	elementMap.clear();
	entitySet.clear();

	if ( validationThread != NULL )
	{
		validationThread->PendingDelete();
		//validationThread->Delete();
		//delete validationThread;
	}
	if ( mPromptGeneratorThread != NULL )
		mPromptGeneratorThread->PendingDelete();
}


// taken from wxStyledNotebook (c) Eran Ifrah <eranif@bezeqint.net>
static wxColor LightColour ( const wxColour& color, int percent )
{
	int rd, gd, bd, high = 0;
	wxColor end_color = wxT ( "WHITE" );
	rd = end_color.Red() - color.Red();
	gd = end_color.Green() - color.Green();
	bd = end_color.Blue() - color.Blue();
	high = 100;

	// We take the percent way of the color from color --> white
	int i = percent;
	int r = color.Red() + ( ( i*rd*100 ) /high ) /100;
	int g = color.Green() + ( ( i*gd*100 ) /high ) /100;
	int b = color.Blue() + ( ( i*bd*100 ) /high ) /100;
	return wxColor ( r, g, b );
}

void XmlCtrl::OnIdle ( wxIdleEvent& event )
{
	if ( properties.number && type != FILE_TYPE_BINARY )
		adjustNoColumnWidth(); // exits if unchanged
}

void XmlCtrl::OnValidationCompleted ( wxCommandEvent &event )
{
	wxCriticalSectionLocker locker ( xmlcopyeditorCriticalSection );

	if ( validationThread == NULL )
		return;

	MyFrame *frame = (MyFrame *)GetGrandParent();
	clearErrorIndicators ( GetLineCount() );
	if ( validationThread->isSucceeded() )
	{
		frame->statusProgress ( wxEmptyString );
	}
	else
	{
		setErrorIndicator ( validationThread->getPosition().first - 1, 0 );
		frame->statusProgress ( validationThread->getMessage() );
	}

	validationThread->Wait();
	delete validationThread;
	validationThread = NULL;
}

void XmlCtrl::OnChar ( wxKeyEvent& event )
{
	if ( *protectTags )
	{
		SetOvertype ( false );
		if ( GetSelectionStart() != GetSelectionEnd() )
			adjustSelection();
		if ( !canInsertAt ( GetCurrentPos() ) )
			adjustPosRight();
	}

	if ( ( !properties.completion || GetOvertype() ) &&
	        !*protectTags )
	{
		event.Skip();
		return;
	}

	switch ( event.GetKeyCode() )
	{
		case '<':
			handleOpenAngleBracket ( event );
			return;
		case '>':
			handleCloseAngleBracket ( event );
			return;
		case ' ':
			handleSpace ( event );
			return;
		case '=':
			handleEquals ( event );
			return;
		case '&':
			handleAmpersand ( event );
			return;
		case '/':
			handleForwardSlash ( event );
			return;
		default:
			break;
	}
	event.Skip();
}

void XmlCtrl::handleBackspace ( wxKeyEvent& event )
{
	protectHeadLine();

	validationRequired = true;

	if ( canMoveLeftAt ( GetCurrentPos() ) )
	{
		DeleteBack();//event.Skip();
		return;
	}

	int currentPos, limitPos;
	currentPos = GetCurrentPos();
	if ( currentPos < 1 )
		return;

	limitPos = currentPos - 1;

	// tag
	int limitStyle = getLexerStyleAt ( limitPos );
	if ( GetCharAt ( limitPos ) == '>' &&
	        ( limitStyle == wxSTC_H_TAG ||
	          limitStyle == wxSTC_H_TAGUNKNOWN ||
	          limitStyle == wxSTC_H_TAGEND ||
	          limitStyle == wxSTC_H_XMLSTART ||
	          limitStyle == wxSTC_H_XMLEND ) )
	{
		if ( GetSelectionStart() != GetSelectionEnd() )
		{
			if ( *protectTags )
				adjustSelection();
			else
				DeleteBack();//event.Skip();
			return;
		}
		if ( !properties.deleteWholeTag )
		{
			if ( ! ( *protectTags ) )
				DeleteBack();//event.Skip();
			return;
		}
		// delete tag to left of caret
		for ( ;
		        limitPos &&
		        GetCharAt ( limitPos ) != '<';
		        limitPos-- )
			;
		SetSelection ( currentPos, limitPos );
		if ( *protectTags )
		{
			SetReadOnly ( true ); // needed to prevent erroneous BS insertion by control
			int ret =
			    wxMessageBox ( _ ( "Delete tag?" ),
			                   _ ( "Tags Locked" ),
			                   wxOK | wxCANCEL | wxICON_QUESTION );
			SetReadOnly ( false );
			if ( ret != wxOK )
			{
				return;
			}
			// ensure selection is set correctly
			if ( GetSelectionStart() != currentPos || GetSelectionEnd() != limitPos )
				SetSelection ( currentPos, limitPos );
		}
		DeleteBack();
		return;
	}

	// entity reference
	else if ( GetCharAt ( limitPos ) == ';' && getLexerStyleAt ( limitPos ) == wxSTC_H_ENTITY )
	{
		// delete entity to left of caret
		for ( ;
		        limitPos &&
		        getLexerStyleAt ( limitPos ) == wxSTC_H_ENTITY &&
		        GetCharAt ( limitPos ) != '&';
		        limitPos-- )
			;
		SetSelection ( limitPos, currentPos );
		if ( *protectTags )
		{
			SetReadOnly ( true ); // needed to prevent erroneous BS insertion by control
			int ret =
			    wxMessageBox ( _ ( "Delete entity reference?" ),
			                   _ ( "Tags Locked" ),
			                   wxOK | wxCANCEL | wxICON_QUESTION );
			SetReadOnly ( false );
			if ( ret != wxOK )
				return;
			// ensure selection is set correctly
			if ( GetSelectionStart() != currentPos || GetSelectionEnd() != limitPos )
				SetSelection ( currentPos, limitPos );
		}
		DeleteBack();
		return;
	}
	else if ( *protectTags )
	{
		return;
	}
	event.Skip();
}

void XmlCtrl::handleDelete ( wxKeyEvent& event )
{
	protectHeadLine();

	validationRequired = true;

	if ( !canMoveRightAt ( GetCurrentPos() ) &&
	        GetSelectionStart() == GetSelectionEnd() )
	{
		int currentPos, limitPos;
		limitPos = currentPos = GetCurrentPos();

		// tag
		int limitStyle = getLexerStyleAt ( limitPos );
		if ( GetCharAt ( limitPos ) == '<' &&
		        ( limitStyle == wxSTC_H_TAG ||
		          limitStyle == wxSTC_H_TAGUNKNOWN ||
		          limitStyle == wxSTC_H_TAGEND ||
		          limitStyle == wxSTC_H_XMLSTART ||
		          limitStyle == wxSTC_H_XMLEND
		        ) )
		{
			if ( GetSelectionStart() != GetSelectionEnd() )
			{
				if ( *protectTags )
					adjustSelection();
				else
					DeleteBack();//event.Skip();
				return;
			}
			if ( !properties.deleteWholeTag )
			{
				if ( ! ( *protectTags ) )
					DeleteBack();//event.Skip();
				return;
			}

			for ( ;
			        GetCharAt ( limitPos ) != '>' && limitPos < GetLength();
			        limitPos++ )
			{
				if ( limitPos > ( currentPos + BUFSIZ ) )
				{
					DeleteBack();//event.Skip();
					return;
				}
			}
			if ( currentPos != limitPos )
			{
				SetSelection ( currentPos, limitPos + 1 );
				if ( *protectTags )
				{
					int ret =
					    wxMessageBox ( _ ( "Delete tag?" ),
					                   _ ( "Tags Locked" ),
					                   wxOK | wxCANCEL | wxICON_QUESTION );
					if ( ret != wxOK )
						return;
				}
				DeleteBack();
				return;
			}
		}
		// entity
		else if ( GetCharAt ( limitPos ) == '&' && getLexerStyleAt ( limitPos ) == wxSTC_H_ENTITY )
		{
			for ( ;
			        getLexerStyleAt ( limitPos ) == wxSTC_H_ENTITY && limitPos <= GetLength();
			        limitPos++ )
			{
				if ( GetCharAt ( limitPos ) == ';' )
					break;
				else if ( GetCharAt ( limitPos ) == '\n' || limitPos > ( currentPos + BUFSIZ ) )
				{
					DeleteBack();//event.Skip();
					return;
				}
			}
			if ( currentPos != limitPos )
			{
				SetSelection ( currentPos, limitPos + 1 );
				if ( *protectTags )
				{
					int ret =
					    wxMessageBox ( _ ( "Delete entity reference?" ),
					                   _ ( "Tags Locked" ),
					                   wxOK | wxCANCEL | wxICON_QUESTION );
					if ( ret != wxOK )
						return;
				}
				DeleteBack();
				return;
			}
		}

		else if ( *protectTags )
			return;
	}
	event.Skip();
}

void XmlCtrl::handleOpenAngleBracket ( wxKeyEvent& event )
{
	if ( AutoCompActive() )
		AutoCompCancel();

	validationRequired = true;

	if ( *protectTags )
	{
		AddText ( _T ( "&lt;" ) );
		return;
	}

	AddText ( _T ( "<" ) );

	int pos = GetCurrentPos();

	// exit conditions based on style
	int style = getLexerStyleAt ( pos );
	switch ( style )
	{
		case wxSTC_H_DOUBLESTRING:
		case wxSTC_H_SINGLESTRING:
			return;
		default:
			break;
	}

	// determine parent element
	int parentCloseAngleBracket;
	parentCloseAngleBracket = getParentCloseAngleBracket ( pos );

	if ( parentCloseAngleBracket < 0 )
		return;

	wxString parent = getLastElementName ( parentCloseAngleBracket );
	if ( elementMap.find ( parent ) == elementMap.end() )
		return;

	wxString choice;
	std::set<wxString> &childSet = elementMap[parent];
	std::set<wxString>::iterator it;
	for ( it = childSet.begin(); it != childSet.end(); ++it )
	{
		if ( !choice.empty() )
			choice.append ( _T ( "<" ) );
		choice.append ( *it );
	}
	if ( !choice.empty() )
		UserListShow ( 0, choice );
}

void XmlCtrl::handleCloseAngleBracket ( wxKeyEvent& event )
{
	if ( AutoCompActive() )
		AutoCompCancel();

	validationRequired = true;

	if ( *protectTags )
	{
		AddText ( _T ( "&gt;" ) );
		return;
	}
	
	wxString insertBuffer;
	int pos;
	pos = GetCurrentPos();

	wxString elementName = getLastElementName ( pos );
	if ( !elementName.empty() )
	{
		attributeMap[elementName]; // Just want to put it there
	}

	// exit condition 1
	if ( pos <= 1 )
	{
		event.Skip();
		return;
	}
	// exit condition 2 (empty tag/end of CDATA section)
	else if ( GetCharAt ( pos - 1 ) == '/' ||
	          GetCharAt ( pos - 1 ) == ']' )
	{
		event.Skip();
		return;
	}
	// exit condition 3 (comment/CDATA)
	else if ( getLexerStyleAt ( pos - 1 ) == wxSTC_H_COMMENT ||
	          getLexerStyleAt ( pos - 1 ) == wxSTC_H_CDATA ||
	          ( getLexerStyleAt ( pos - 1 ) == wxSTC_H_DOUBLESTRING && ( GetCharAt ( pos - 1 ) != '"' ) ) ||
	          ( getLexerStyleAt ( pos - 1 ) == wxSTC_H_SINGLESTRING && ( GetCharAt ( pos - 1 ) != '\'' ) ) )
	{
		event.Skip();
		return;
	}

	if ( !elementName.empty() )
	{
		if ( !properties.insertCloseTag )
		{
			event.Skip();
			return;
		}
		AddText ( _T ( ">" ) );
		insertBuffer += _T ( "</" );
		insertBuffer += elementName;
		insertBuffer += _T ( ">" );
		InsertText ( pos + 1, insertBuffer );
		SetSelection ( pos + 1, pos + 1 );
	}
	else
		event.Skip();
}

void XmlCtrl::handleEquals ( wxKeyEvent& event )
{
	if ( AutoCompActive() )
		AutoCompCancel();

	int pos = GetCurrentPos();
	if ( pos <= 0 || getLexerStyleAt ( pos - 1 ) != wxSTC_H_ATTRIBUTE )
	{
		event.Skip();
		return;
	}
	AddText ( _T ( "=\"\"" ) );
	SetSelection ( pos + 2, pos + 2 );

	// tbd: identify possible attribute values
	wxString choice, elementName, attributeName;
	elementName = getLastElementName ( pos );
	attributeName = getLastAttributeName ( pos );

	std::set<wxString> &valueSet = attributeMap[elementName][attributeName];
	if ( valueSet.empty() )
		return;

	std::set<wxString>::iterator valueSetIterator;
	int cutoff = BUFSIZ;
	for ( valueSetIterator = valueSet.begin();
	        valueSetIterator != valueSet.end();
	        ++valueSetIterator )
	{
		if ( ! ( cutoff-- ) )
			break;
		if ( !choice.empty() )
			choice.Append ( _T ( "<" ) );
		choice.Append ( *valueSetIterator );
	}

	if ( !choice.empty() )
		UserListShow ( 0, choice );
}

void XmlCtrl::handleSpace ( wxKeyEvent& event )
{
	if ( AutoCompActive() )
		AutoCompCancel();

	int pos = GetCurrentPos();
	if ( pos <= 2 )
	{
		event.Skip();
		return;
	}

	int style = getLexerStyleAt ( pos - 1 );

	char c = GetCharAt ( pos - 1 );

	bool proceed = false;
	// space pressed after element name
	if (
	    style == wxSTC_H_TAG ||
	    style == wxSTC_H_TAGUNKNOWN ||
	    style == wxSTC_H_ATTRIBUTEUNKNOWN ||
	    style == wxSTC_H_ATTRIBUTE )
	{
		proceed = true;
	}
	// space pressed after attribute value
	else if (
	    ( style == wxSTC_H_DOUBLESTRING ||
	      style == wxSTC_H_SINGLESTRING ) &&
	    ( c == '\'' || c == '"' ) &&
	    GetCharAt ( pos - 2 ) != '=' )
	{
		proceed = true;
	}

	int tagStartPos = getTagStartPos ( pos );
	if ( !proceed || tagStartPos == -1 )
	{
		event.Skip();
		return;
	}

	AddText ( _T ( " " ) );

	wxString elementName = getLastElementName ( pos );
	if ( attributeMap.find ( elementName ) == attributeMap.end() )
		return;

	wxString choice;
	wxString tag = GetTextRange ( tagStartPos, pos );
	std::map<wxString, std::set<wxString> > &curAttMap = attributeMap[elementName];
	std::map<wxString, std::set<wxString> >::iterator it;
	for ( it = curAttMap.begin(); it != curAttMap.end(); ++it )
	{
		// avoid duplicate attributes
		if ( tag.Contains ( it->first + _T ( "=" ) ) )
			continue;

		if ( !choice.empty() )
			choice.Append ( _T ( "<" ) );
		choice.Append ( it->first );
	}
	if ( !choice.empty() )
	{
		UserListShow ( 0, choice );
		validationRequired = true;
	}
}

void XmlCtrl::handleAmpersand ( wxKeyEvent& event )
{
	if ( AutoCompActive() )
		AutoCompCancel();

	validationRequired = true;

	if ( *protectTags )
	{
		AddText ( _T ( "&amp;" ) );
		return;
	}

	int pos, style;
	pos = GetCurrentPos();
	style = getLexerStyleAt ( pos );
	if ( style != wxSTC_H_COMMENT &&
	        style != wxSTC_H_CDATA &&
	        style != wxSTC_H_TAGUNKNOWN &&
	        entitySet.size() >= 4 ) // min. 4 default entities
	{
		AddText ( _T ( "&" ) );
		wxString choice;
		std::set<wxString>::iterator it = entitySet.begin();
		choice += *it;
		choice += _T ( ";" );
		for ( it++; it != entitySet.end(); ++it )
		{
			choice += _T ( "<" );
			choice += *it;
			choice += _T ( ";" );
		}
		UserListShow ( 0, choice );
	}
	else
		event.Skip();
}

void XmlCtrl::handleForwardSlash ( wxKeyEvent& event )
{
	if ( AutoCompActive() )
		AutoCompCancel();

	int pos = GetCurrentPos();
	if (
	    ( pos <= 0 ) ||
	    GetCharAt ( pos - 1 ) != '<' ||
	    getLexerStyleAt ( pos ) == wxSTC_H_COMMENT ||
	    getLexerStyleAt ( pos ) == wxSTC_H_CDATA )
	{
		event.Skip();
		return;
	}

	AddText ( _T ( "/" ) );

	int parentCloseAngleBracket = getParentCloseAngleBracket ( pos );
	if ( parentCloseAngleBracket < 0 )
		return;

	wxString wideParent = getLastElementName ( parentCloseAngleBracket );
	if ( wideParent.empty() )
		return;
	AddText ( wideParent + _T ( ">" ) );
	validationRequired = true;
}

void XmlCtrl::OnKeyPressed ( wxKeyEvent& event )
{
	if ( *protectTags && GetSelectionStart() != GetSelectionEnd() )
	{
		adjustSelection();
	}
	if ( *protectTags )
		SetOvertype ( false );

	int pos, iteratorPos, maxPos;
	char c;
	wxString s;
	switch ( event.GetKeyCode() )
	{
		case WXK_RETURN:
			if ( AutoCompActive() )
			{
				AutoCompComplete();
				return;
			}

			if ( *protectTags )
				adjustPosRight();

			insertNewLine();
			return;
		case WXK_RIGHT:
			pos = GetCurrentPos();

			if ( *protectTags && !canMoveRightAt ( pos ) )
			{
				SetSelection ( pos + 1, pos + 1 );
				adjustPosRight();
				return;
			}

			maxPos = GetLength();
			c = GetCharAt ( pos );
			if ( c == '<' && event.ControlDown() && !event.ShiftDown() )
			{
				for (
				    iteratorPos = pos;
				    iteratorPos < maxPos &&
				    GetCharAt ( iteratorPos ) != ' ' &&
				    GetCharAt ( iteratorPos ) != '>' &&
				    GetCharAt ( iteratorPos ) != '\n';
				    ++iteratorPos )
					;
				++iteratorPos;
				SetSelection ( iteratorPos, iteratorPos );
				return;
			}
			else
				break;
		case WXK_LEFT:
			pos = GetCurrentPos();
			if ( *protectTags && !canMoveLeftAt ( pos ) )
			{
				adjustPosLeft();
				return;
			}

			if ( pos < 3 )
				break;
			c = GetCharAt ( pos - 1 );
			if ( c == '>' && event.ControlDown() && !event.ShiftDown() )
			{
				for (
				    iteratorPos = pos - 1;
				    iteratorPos > 0 &&
				    GetCharAt ( iteratorPos ) != '<' &&
				    GetCharAt ( iteratorPos ) != ' ' &&
				    GetCharAt ( iteratorPos ) != '=' &&
				    GetCharAt ( iteratorPos ) != '\n';
				    --iteratorPos )
					;
				if (
				    GetCharAt ( iteratorPos ) != '<' &&
				    GetCharAt ( iteratorPos ) != '=' )
					++iteratorPos;
				SetSelection ( iteratorPos, iteratorPos );
				return;
			}
			else
				break;
		case WXK_UP:
			if ( *protectTags )
			{
				LineUp();
				if ( !canInsertAt ( GetCurrentPos() ) )
					adjustPosLeft();
				return;
			}
			break;
		case WXK_DOWN:
			if ( *protectTags )
			{
				LineDown();
				if ( !canInsertAt ( GetCurrentPos() ) )
					adjustPosRight();
				return;
			}
			break;
		case WXK_INSERT:
			if ( *protectTags )
				return;
			break;
		case WXK_BACK:
			handleBackspace ( event );
			validationRequired = true;
			return;
		case WXK_TAB:
			if ( *protectTags )
			{
				if ( !canInsertAt ( GetCurrentPos() ) )
					adjustPosRight();
			}
			break;
		case WXK_HOME:
			if ( *protectTags && !event.ControlDown() && !event.ShiftDown() )
			{
				Home();
				if ( !canInsertAt ( GetCurrentPos() ) )
					adjustPosLeft();
				return;
			}
			break;
		case WXK_END:
			if ( *protectTags && !event.ControlDown() && !event.ShiftDown() )
			{
				LineEnd();
				if ( !canInsertAt ( GetCurrentPos() ) )
					adjustPosRight();
				return;
			}
			break;
		case WXK_PAGEUP:
			if ( *protectTags )
			{
				PageUp();
				if ( !canInsertAt ( GetCurrentPos() ) )
					adjustPosLeft();
				return;
			}
			break;
		case WXK_PAGEDOWN:
			if ( *protectTags )
			{
				PageDown();
				if ( !canInsertAt ( GetCurrentPos() ) )
					adjustPosRight();
				return;
			}
			break;
		case WXK_DELETE:
			handleDelete ( event );
			validationRequired = true;
			return;
		default:
			break;
	}
	event.Skip();
}

wxString XmlCtrl::getLastElementName ( int pos )
{
	if ( pos < 1 )
		return _T ( "" );

	int startPos, iteratorPos;

	for ( startPos = pos - 1; startPos >= 0; --startPos )
	{
		char c = GetCharAt ( startPos );
		if ( c == '>' )
			return _T ( "" );
		else if ( c == '<' )
			break;
	}

	// exit if not found or closing/declaration/command tag
	if ( GetCharAt ( startPos ) != '<' ||
	        GetCharAt ( startPos + 1 ) == '/' ||
	        GetCharAt ( startPos + 1 ) == '?' ||
	        GetCharAt ( startPos + 1 ) == '!' )
		return _T ( "" );

	++startPos;
	for ( iteratorPos = startPos;
	        iteratorPos < pos &&
	        GetCharAt ( iteratorPos ) != ' ' &&
	        GetCharAt ( iteratorPos ) != '\n' &&
	        GetCharAt ( iteratorPos ) != '\t' &&
	        GetCharAt ( iteratorPos ) != '/' &&
	        GetCharAt ( iteratorPos ) != '>';
	        ++iteratorPos )
		;
	if ( startPos == iteratorPos )
		return _T ( "" );
	return GetTextRange ( startPos, iteratorPos );
}

const std::set<wxString> &XmlCtrl::getChildren ( const wxString& parent )
{
	static std::set<wxString> mySet;
	if ( elementMap.find ( parent ) == elementMap.end() )
		return mySet;

	return elementMap[parent];
}

wxString XmlCtrl::getLastAttributeName ( int pos )
{
	if ( pos < 1 )
		return _T ( "" );

	int startPos;

	for ( startPos = pos - 1; startPos >= 0; --startPos )
	{
		char c = GetCharAt ( startPos );
		if ( c == ' ' || c == '<' )
			break;
	}

	if ( GetCharAt ( startPos ) != ' ' ||
	        startPos >= pos - 1 )
		return _T ( "" );

	return GetTextRange ( startPos + 1, pos );
}

int XmlCtrl::findNextEndTag (
		int pos,
		unsigned depth /*= 1*/,
		int character /*= '>'*/,
		int range /*= USHRT_MAX * 4*/ )
{
	wxASSERT ( character == '<' || character == '>' );

	unsigned openAngleBrackets = 0;
	int cutoff = pos + range;
	if ( cutoff > GetEndStyled() )
		cutoff = GetEndStyled();
	for ( ; pos < cutoff; ++pos )
	{
		int ch = GetCharAt ( pos );
		if ( ch == '<' )
			openAngleBrackets = ( openAngleBrackets << 1 ) | 1;// Just a flag
		// Check for empty tags, which have start tags but no end tags
		if ( character != '>' || !openAngleBrackets )
			if ( ch == '>' && getLexerStyleAt ( pos ) == wxSTC_H_TAGEND )
				--depth;
		if ( ch != character )
			continue;

		int style = getLexerStyleAt ( pos );
		if ( style == wxSTC_H_TAG || style == wxSTC_H_TAGUNKNOWN )
		{
			int type = getTagType ( pos );
			switch ( type )
			{
			case TAG_TYPE_CLOSE:
				--depth;
				break;
			case TAG_TYPE_OPEN:
				// In case that the cursor is inside a start tag
				if ( character != '>' || openAngleBrackets > 0 )
					++depth;
				break;
			case TAG_TYPE_EMPTY:
			case TAG_TYPE_OTHER:
			case TAG_TYPE_ERROR:
				break;
			}
		}

		if ( !depth )
			return pos + 1;
	}
	return -1;
}

int XmlCtrl::findPreviousStartTag (
		int pos,
		unsigned depth /*= 1*/,
		int character /*= '<'*/,
		int range /*= USHRT_MAX * 4*/ )
{
	wxASSERT ( character == '<' || character == '>' );

	int cutoff = ( ( pos - range ) > 2 ) ? pos - range : 2;
	unsigned closeAngleBrackets = 0;
	while ( pos-- > cutoff )
	{
		int ch = GetCharAt ( pos );
		if ( ch == '>' )
		{
			closeAngleBrackets = ( closeAngleBrackets << 1 ) | 1;// Just a flag
			// Check for empty tags, which have start tags but no end tags
			if ( character != '>' && getLexerStyleAt ( pos ) == wxSTC_H_TAGEND )
				++depth;
		}
		if ( ch != character )
			continue;

		int style = getLexerStyleAt ( pos );
		if ( style == wxSTC_H_TAG || style == wxSTC_H_TAGUNKNOWN )
		{
			int type = getTagType ( pos );
			switch ( type )
			{
			case TAG_TYPE_CLOSE:
				// If the cursor is already in an end tag
				if ( character != '<' || closeAngleBrackets > 0 )
					++depth;
				break;
			case TAG_TYPE_OPEN:
				--depth;
				break;
			case TAG_TYPE_EMPTY:
			case TAG_TYPE_OTHER:
			case TAG_TYPE_ERROR:
				break;
			}
		}
		if ( !depth )
			return pos;
	}
	return -1;
}

void XmlCtrl::adjustNoColumnWidth()
{	
	int maxLine = GetLineCount();
	if ( maxLine == currentMaxLine )
		return;

	int digits, charWidth, width;
	digits = 0;
	do
	{
		++digits;
		maxLine /= 10;
	}
	while ( maxLine );
	digits = ( digits > 2 ) ? digits : 2;
	charWidth = TextWidth ( wxSTC_STYLE_LINENUMBER, _T ( "9" ) );
	width = ( digits + 2 ) * charWidth;
	SetMarginWidth ( 0, width );
	currentMaxLine = maxLine;
}

void XmlCtrl::updatePromptMaps()
{
	std::string bufferUtf8 = myGetTextRaw();

	updatePromptMaps ( bufferUtf8.c_str(), bufferUtf8.size() );
}

void XmlCtrl::updatePromptMaps ( const char *utf8Buffer, size_t bufferLen )
{
	attributeMap.clear();
	elementMap.clear();
	elementStructureMap.clear();

	wxCriticalSectionLocker locker ( xmlcopyeditorCriticalSection );

	if ( mPromptGeneratorThread )
		return;

	mPromptGeneratorThread = new XmlPromptGenerator (
		GetEventHandler(), utf8Buffer, bufferLen, basePath, auxPath, "UTF-8"
	);

	if ( mPromptGeneratorThread->Create() != wxTHREAD_NO_ERROR
	    || mPromptGeneratorThread->Run() != wxTHREAD_NO_ERROR )
	{
		delete mPromptGeneratorThread;
		mPromptGeneratorThread = NULL;
	}
}

void XmlCtrl::OnPromptGenerated ( wxNotifyEvent &event )
{
	wxCriticalSectionLocker locker ( xmlcopyeditorCriticalSection );

	if ( mPromptGeneratorThread == NULL )
		return;

	attributeMap = mPromptGeneratorThread->getAttributeMap();
	requiredAttributeMap = mPromptGeneratorThread->getRequiredAttributeMap();
	elementMap = mPromptGeneratorThread->getElementMap();
	elementStructureMap = mPromptGeneratorThread->getElementStructureMap();
	entitySet = mPromptGeneratorThread->getEntitySet();
	grammarFound = mPromptGeneratorThread->getGrammarFound();
	entitySet.insert ( _T ( "amp" ) );
	entitySet.insert ( _T ( "apos" ) );
	entitySet.insert ( _T ( "quot" ) );
	entitySet.insert ( _T ( "lt" ) );
	entitySet.insert ( _T ( "gt" ) );

	mPromptGeneratorThread->Wait();
	delete mPromptGeneratorThread;
	mPromptGeneratorThread = NULL;

	wxPostEvent ( GetParent()->GetEventHandler(), event );
}

void XmlCtrl::applyProperties (
    const XmlCtrlProperties &propertiesParameter,
    bool zoomOnly )
{
	properties = propertiesParameter;

	SetZoom ( ( type != FILE_TYPE_BINARY ) ? properties.zoom : 0 );

	if ( zoomOnly )
		return;

	SetCaretLineVisible ( properties.currentLine );
	SetIndentationGuides (
	    ( type != FILE_TYPE_BINARY ) ? properties.indentLines : false );
	SetWrapMode ( ( type != FILE_TYPE_BINARY ) ? properties.wrap : false );
	SetViewWhiteSpace (
	    ( properties.whitespaceVisible ) ?
	    wxSTC_WS_VISIBLEAFTERINDENT : wxSTC_WS_INVISIBLE );

	switch ( type )
	{
		case FILE_TYPE_BINARY:
			SetLexer ( wxSTC_LEX_NULL );
			break;
		case FILE_TYPE_CSS:
			SetLexer ( wxSTC_LEX_CSS );
			break;
		default:
			SetLexer ( wxSTC_LEX_XML );
			break;
	}

	setColorScheme (
	    ( type != FILE_TYPE_BINARY ) ? properties.colorScheme : COLOR_SCHEME_NONE );

	// line no margin
	if ( properties.number ) // permit line nos for large files
	{
		SetMarginType ( 0, wxSTC_MARGIN_NUMBER ); // width set at idle time
		adjustNoColumnWidth();
	}
	else
		SetMarginWidth ( 0, 0 );

	SetMarginWidth ( 1, 0 );

	if ( properties.fold && type != FILE_TYPE_BINARY )
	{
		// folding margin
		SetMarginType ( 2, wxSTC_MARGIN_SYMBOL );
		SetMarginMask ( 2, wxSTC_MASK_FOLDERS );
		SetMarginSensitive ( 2, true );
		SetMarginWidth ( 2, 16 );

		// define folding markers
		MarkerDefine ( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED,  *wxWHITE, *wxBLACK );
		MarkerDefine ( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, *wxWHITE, *wxBLACK );
		MarkerDefine ( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER, *wxWHITE, *wxBLACK );
		MarkerDefine ( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER, *wxWHITE, *wxBLACK );
		MarkerDefine ( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE, *wxWHITE, *wxBLACK );
		MarkerDefine ( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS, *wxWHITE, *wxBLACK );
		MarkerDefine ( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS, *wxWHITE, *wxBLACK );
	}
	else
	{
		SetMarginWidth ( 2, 0 );
	}

	if ( type != FILE_TYPE_BINARY )
	{
		Colourise ( 0, -1 );
	}

	wxString value = ( properties.fold && type != FILE_TYPE_BINARY ) ? _T ( "1" ) : _T ( "0" );
	SetProperty ( _T ( "fold" ), value );
	SetProperty ( _T ( "fold.compact" ), value );
	SetProperty ( _T ( "fold.html" ), value );
}

void XmlCtrl::OnMarginClick ( wxStyledTextEvent& event )
{
	const int line = LineFromPosition ( event.GetPosition() );
	const int margin = event.GetMargin();

	// let others process this first
	if ( GetParent()->GetEventHandler()->ProcessEvent ( event ) )
		return;

	if ( margin == 2 )
	{
		const int level = GetFoldLevel ( line );
		if ( ( ( level ) & ( wxSTC_FOLDLEVELHEADERFLAG ) ) != 0 )
			ToggleFold ( line );
	}
	else
		event.Skip();
}

void XmlCtrl::OnMouseLeftDown ( wxMouseEvent& event )
{
	SetMouseDownCaptures ( !*protectTags );
	event.Skip();
}

void XmlCtrl::OnMouseLeftUp ( wxMouseEvent& event )
{
	event.Skip();
	if ( *protectTags && !canInsertAt ( GetCurrentPos() ) )
		adjustPosRight();
}

void XmlCtrl::OnMouseRightUp ( wxMouseEvent& event )
{
	if ( *protectTags )
		return;
	event.Skip();
}

bool XmlCtrl::isCloseTag ( int pos )
{
	int iteratorPos;
	for ( iteratorPos = pos; iteratorPos >= 0; --iteratorPos )
	{
		if ( GetCharAt ( iteratorPos ) == '<' && getLexerStyleAt ( iteratorPos ) == wxSTC_H_TAG )
			return ( GetCharAt ( iteratorPos + 1 ) == '/' ) ? true : false;
	}
	return false;
}

int XmlCtrl::getTagStartPos ( int pos )
{
	int iteratorPos;
	for ( iteratorPos = pos; iteratorPos >= 0; --iteratorPos )
	{
		if ( GetCharAt ( iteratorPos ) == '<' && getLexerStyleAt ( iteratorPos ) == wxSTC_H_TAG )
			return iteratorPos;
	}
	return -1;
}

int XmlCtrl::getAttributeStartPos ( int pos )
{
	int iteratorPos;
	char c;
	bool attributeSeen = false;
	for ( iteratorPos = pos; iteratorPos > 0; iteratorPos-- )
	{
		c = GetCharAt ( iteratorPos );
		if ( ( getLexerStyleAt ( iteratorPos ) ) == wxSTC_H_ATTRIBUTE )
			attributeSeen = true;
		else if ( ( c == ' ' || c == '\t' || c == '\n' ) && attributeSeen )
			return iteratorPos;
	}
	return -1;
}

int XmlCtrl::getAttributeSectionEndPos ( int pos, int range )
{
	int iteratorPos, limit, style;
	limit = pos + range;

	for ( iteratorPos = pos; iteratorPos < limit; iteratorPos++ )
	{
		style = getLexerStyleAt ( iteratorPos );
		switch ( style )
		{
			case wxSTC_H_TAG:
			case wxSTC_H_TAGEND:
			case wxSTC_H_XMLEND:
			case wxSTC_H_QUESTION:
				return iteratorPos;
			default:
				continue;
		}
	}
	return -1;
}

void XmlCtrl::adjustCursor()
{
	if ( !canInsertAt ( GetCurrentPos() ) )
		adjustPosRight();
}

bool XmlCtrl::canInsertAt ( int pos )
{
	if ( pos < 0 )
		return false;

	int style = getLexerStyleAt ( pos );
	switch ( style )
	{
		case wxSTC_H_TAG:
		case wxSTC_H_TAGUNKNOWN:
		case wxSTC_H_QUESTION:
		case wxSTC_H_CDATA:
		case wxSTC_H_COMMENT:
		case wxSTC_H_SGML_DEFAULT:
			return ( GetCharAt ( pos ) == '<' ) ? true : false;
		case wxSTC_H_ENTITY:
			return ( GetCharAt ( pos ) == '&' ) ? true : false;
		case wxSTC_H_DEFAULT:
			return true;
	}
	return false;
}

bool XmlCtrl::canMoveRightAt ( int pos )
{
	int style = getLexerStyleAt ( pos );
	switch ( style )
	{
		case wxSTC_H_DEFAULT:
			return true;
	}
	return false;
}

bool XmlCtrl::canMoveLeftAt ( int pos )
{
	if ( pos < 1 )
		return false;

	int style = getLexerStyleAt ( pos - 1 );
	switch ( style )
	{
		case wxSTC_H_DEFAULT:
			return true;
	}
	return false;
}

void XmlCtrl::adjustPosRight()
{
	int pos, max;
	pos = GetCurrentPos();
	max = GetLength();
	for ( ; pos <= max; pos++ )
		if ( canInsertAt ( pos ) )
		{
			break;
		}
	SetSelection ( pos, pos );
}

void XmlCtrl::adjustPosLeft()
{
	int pos;
	pos = GetCurrentPos() - 1;
	if ( pos < 0 )
	{
		SetSelection ( 0, 0 );
		return;
	}
	for ( ; pos > 0; pos-- )
		if ( canInsertAt ( pos ) )
		{
			break;
		}
	SetSelection ( pos, pos );
}

void XmlCtrl::adjustSelection()
{
	int start, end, iterator;
	start = GetSelectionStart();
	end = GetSelectionEnd();

	// exit condition 1
	if ( start == end && canInsertAt ( start ) )
		return;

	// exit condition 2
	else if ( !canInsertAt ( start ) || start > end )
	{
		SetSelection ( start, start );
		return;
	}

	for ( iterator = start; iterator < end; iterator++ )
	{
		if ( !canMoveRightAt ( iterator ) )
			break;
	}
	SetSelection ( start, iterator );
}

void XmlCtrl::setColorScheme ( int scheme )
{
	StyleSetFaceName ( wxSTC_STYLE_DEFAULT, properties.font );

	switch ( scheme )
	{
		case COLOR_SCHEME_DEFAULT:
			StyleSetForeground ( wxSTC_STYLE_DEFAULT, *wxBLACK );
			StyleSetBackground ( wxSTC_STYLE_DEFAULT, *wxWHITE );
			StyleClearAll();

			baseBackground = LightColour ( wxTheColourDatabase->Find ( _T ( "CYAN" ) ), 75 );
			alternateBackground = LightColour ( wxTheColourDatabase->Find ( _T ( "CYAN" ) ), 95 );
			SetCaretLineBackground ( baseBackground );

			SetCaretForeground ( *wxBLACK );
			SetSelBackground ( true,  LightColour ( wxTheColourDatabase->Find ( _T ( "YELLOW" ) ), 20));//wxTheColourDatabase->Find ( _T ( "LIGHT GREY" ) ) );

			if ( type == FILE_TYPE_CSS )
			{
				// CSS colours
				StyleSetForeground ( wxSTC_CSS_DEFAULT, *wxBLACK );
				StyleSetForeground ( wxSTC_CSS_TAG, *wxBLUE );
				StyleSetForeground ( wxSTC_CSS_CLASS, *wxBLUE );
				StyleSetForeground ( wxSTC_CSS_PSEUDOCLASS, *wxBLUE );
				StyleSetForeground ( wxSTC_CSS_UNKNOWN_PSEUDOCLASS, *wxBLUE );
				StyleSetForeground ( wxSTC_CSS_OPERATOR, *wxBLUE );
				StyleSetForeground ( wxSTC_CSS_IDENTIFIER, *wxBLUE );

				StyleSetForeground ( wxSTC_CSS_UNKNOWN_IDENTIFIER, *wxBLUE );
				StyleSetForeground ( wxSTC_CSS_VALUE, *wxBLACK );
				StyleSetForeground ( wxSTC_CSS_COMMENT,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );
				StyleSetForeground ( wxSTC_CSS_ID, *wxBLACK );
				StyleSetForeground ( wxSTC_CSS_IMPORTANT, *wxRED );
				StyleSetForeground ( wxSTC_CSS_DIRECTIVE, *wxBLUE );
				StyleSetForeground ( wxSTC_CSS_DOUBLESTRING, *wxRED );
				StyleSetForeground ( wxSTC_CSS_SINGLESTRING, *wxRED );
				StyleSetForeground ( wxSTC_CSS_IDENTIFIER2, *wxRED );
				StyleSetForeground ( wxSTC_CSS_ATTRIBUTE, *wxBLUE );
			}
			else // XML
			{
				StyleSetForeground ( wxSTC_H_DEFAULT, *wxBLACK );
				StyleSetForeground ( wxSTC_H_TAG, *wxBLUE );
				StyleSetForeground ( wxSTC_H_TAGUNKNOWN, *wxBLUE );
				StyleSetForeground ( wxSTC_H_ATTRIBUTE, *wxRED );
				StyleSetForeground ( wxSTC_H_ATTRIBUTEUNKNOWN, *wxRED );
				StyleSetForeground ( wxSTC_H_NUMBER, *wxBLACK );
				StyleSetForeground ( wxSTC_H_DOUBLESTRING, *wxBLACK );
				StyleSetForeground ( wxSTC_H_SINGLESTRING, *wxBLACK );
				StyleSetForeground ( wxSTC_H_OTHER, *wxBLUE );
				StyleSetForeground ( wxSTC_H_COMMENT,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );
				StyleSetForeground ( wxSTC_H_ENTITY, *wxRED );
				StyleSetForeground ( wxSTC_H_TAGEND, *wxBLUE );
				StyleSetForeground ( wxSTC_H_XMLSTART,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );
				StyleSetForeground ( wxSTC_H_XMLEND,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );
				StyleSetForeground ( wxSTC_H_CDATA, *wxRED );
				StyleSetForeground ( wxSTC_H_QUESTION,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );

				for ( int i = wxSTC_H_SGML_DEFAULT; i <= wxSTC_H_SGML_BLOCK_DEFAULT; i++ )
					StyleSetForeground ( i, *wxBLUE );
				StyleSetForeground ( wxSTC_H_SGML_ENTITY, *wxRED );
				StyleSetForeground ( wxSTC_H_SGML_SPECIAL, *wxBLACK );
				StyleSetForeground ( wxSTC_H_SGML_SIMPLESTRING, *wxRED );
				StyleSetForeground ( wxSTC_H_SGML_DEFAULT,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );
			}
			break;
		case COLOR_SCHEME_DEFAULT_BACKGROUND:
			StyleSetForeground ( wxSTC_STYLE_DEFAULT, *wxBLACK );
			StyleSetBackground ( wxSTC_STYLE_DEFAULT, *wxWHITE );
			StyleClearAll();

			baseBackground = LightColour ( wxTheColourDatabase->Find ( _T ( "CYAN" ) ), 75 );
			alternateBackground = LightColour ( wxTheColourDatabase->Find ( _T ( "CYAN" ) ), 95 );
			SetCaretLineBackground ( baseBackground );

			SetCaretForeground ( *wxBLACK );
			SetSelBackground ( true, LightColour ( wxTheColourDatabase->Find ( _T ( "YELLOW" ) ), 20) );//wxTheColourDatabase->Find ( _T ( "LIGHT GREY" ) ) );

			if ( type == FILE_TYPE_CSS )
			{
				// CSS colours
				StyleSetForeground ( wxSTC_CSS_DEFAULT, *wxBLACK );
				StyleSetForeground ( wxSTC_CSS_TAG,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_CSS_CLASS,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_CSS_PSEUDOCLASS,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_CSS_UNKNOWN_PSEUDOCLASS,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_CSS_OPERATOR,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_CSS_IDENTIFIER,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );

				StyleSetForeground ( wxSTC_CSS_UNKNOWN_IDENTIFIER,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_CSS_VALUE, *wxBLACK );
				StyleSetForeground ( wxSTC_CSS_COMMENT,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );
				StyleSetForeground ( wxSTC_CSS_ID, *wxBLACK );
				StyleSetForeground ( wxSTC_CSS_IMPORTANT,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_CSS_DIRECTIVE,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_CSS_DOUBLESTRING,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_CSS_SINGLESTRING,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_CSS_IDENTIFIER2,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_CSS_ATTRIBUTE,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
			}
			else // XML
			{
				StyleSetForeground ( wxSTC_H_DEFAULT, *wxBLACK );
				StyleSetForeground ( wxSTC_H_TAG,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_H_TAGUNKNOWN,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_H_ATTRIBUTE,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_H_ATTRIBUTEUNKNOWN,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_H_NUMBER,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );
				StyleSetForeground ( wxSTC_H_DOUBLESTRING,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );
				StyleSetForeground ( wxSTC_H_SINGLESTRING,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );
				StyleSetForeground ( wxSTC_H_OTHER,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_H_COMMENT,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );
				StyleSetForeground ( wxSTC_H_ENTITY,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_H_TAGEND,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_H_XMLSTART,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );
				StyleSetForeground ( wxSTC_H_XMLEND,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );
				StyleSetForeground ( wxSTC_H_CDATA,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_H_QUESTION,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );

				for ( int i = wxSTC_H_SGML_DEFAULT; i <= wxSTC_H_SGML_BLOCK_DEFAULT; i++ )
					StyleSetForeground ( i, wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_H_SGML_ENTITY,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_H_SGML_SPECIAL, *wxBLACK );
				StyleSetForeground ( wxSTC_H_SGML_SIMPLESTRING,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_H_SGML_DEFAULT,
				                     wxTheColourDatabase->Find ( _T ( "GREY" ) ) );
			}
			break;
		case COLOR_SCHEME_REDUCED_GLARE:
			StyleSetForeground ( wxSTC_STYLE_DEFAULT,
			                     wxTheColourDatabase->Find ( _T ( "LIGHT GREY" ) ) );
			StyleSetBackground ( wxSTC_STYLE_DEFAULT,
			                     wxTheColourDatabase->Find ( _T ( "MIDNIGHT BLUE" ) ) );
			StyleClearAll();

			baseBackground = LightColour ( wxTheColourDatabase->Find ( _T ( "NAVY" ) ), 0 );
			alternateBackground = LightColour ( wxTheColourDatabase->Find ( _T ( "NAVY" ) ), 10 );
			SetCaretLineBackground ( baseBackground );

			StyleSetForeground ( wxSTC_STYLE_LINENUMBER, *wxBLACK );

			SetCaretForeground ( *wxWHITE );
			SetSelBackground ( true, wxTheColourDatabase->Find ( _T ( "GREY" ) ) );

			if ( type == FILE_TYPE_CSS )
			{
				// CSS colours
				StyleSetForeground ( wxSTC_CSS_DEFAULT,
				                     wxTheColourDatabase->Find ( _T ( "LIGHT GREY" ) ) );
				StyleSetForeground ( wxSTC_CSS_TAG,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_CSS_CLASS,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_CSS_PSEUDOCLASS,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_CSS_UNKNOWN_PSEUDOCLASS,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_CSS_OPERATOR,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_CSS_IDENTIFIER,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );

				StyleSetForeground ( wxSTC_CSS_UNKNOWN_IDENTIFIER,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_CSS_VALUE, *wxWHITE );
				StyleSetForeground ( wxSTC_CSS_COMMENT,
				                     wxTheColourDatabase->Find ( _T ( "LIGHT GREY" ) ) );
				StyleSetForeground ( wxSTC_CSS_ID, *wxWHITE );
				StyleSetForeground ( wxSTC_CSS_IMPORTANT,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_CSS_DIRECTIVE,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_CSS_DOUBLESTRING,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_CSS_SINGLESTRING,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_CSS_IDENTIFIER2,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_CSS_ATTRIBUTE,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
			}
			else // XML
			{
				StyleSetForeground ( wxSTC_H_DEFAULT, *wxWHITE );
				StyleSetForeground ( wxSTC_H_TAG,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_H_TAGUNKNOWN,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_H_ATTRIBUTE,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_H_ATTRIBUTEUNKNOWN,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_H_NUMBER,
				                     wxTheColourDatabase->Find ( _T ( "LIGHT GREY" ) ) );
				StyleSetForeground ( wxSTC_H_DOUBLESTRING,
				                     wxTheColourDatabase->Find ( _T ( "LIGHT GREY" ) ) );
				StyleSetForeground ( wxSTC_H_SINGLESTRING,
				                     wxTheColourDatabase->Find ( _T ( "LIGHT GREY" ) ) );
				StyleSetForeground ( wxSTC_H_OTHER,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_H_COMMENT,
				                     wxTheColourDatabase->Find ( _T ( "LIGHT GREY" ) ) );
				StyleSetForeground ( wxSTC_H_ENTITY,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_H_TAGEND,
				                     wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_H_XMLSTART,
				                     wxTheColourDatabase->Find ( _T ( "LIGHT GREY" ) ) );
				StyleSetForeground ( wxSTC_H_XMLEND,
				                     wxTheColourDatabase->Find ( _T ( "LIGHT GREY" ) ) );
				StyleSetForeground ( wxSTC_H_CDATA,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_H_QUESTION,
				                     wxTheColourDatabase->Find ( _T ( "LIGHT GREY" ) ) );

				for ( int i = wxSTC_H_SGML_DEFAULT; i <= wxSTC_H_SGML_BLOCK_DEFAULT; i++ )
					StyleSetForeground ( i, wxTheColourDatabase->Find ( _T ( "SKY BLUE" ) ) );
				StyleSetForeground ( wxSTC_H_SGML_ENTITY,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_H_SGML_SPECIAL, *wxWHITE );
				StyleSetForeground ( wxSTC_H_SGML_SIMPLESTRING,
				                     wxTheColourDatabase->Find ( _T ( "ORANGE" ) ) );
				StyleSetForeground ( wxSTC_H_SGML_DEFAULT,
				                     wxTheColourDatabase->Find ( _T ( "LIGHT GREY" ) ) );
			}
			break;
		case COLOR_SCHEME_NONE:
			baseBackground = LightColour ( wxTheColourDatabase->Find ( _T ( "CYAN" ) ), 75 );
			alternateBackground = LightColour ( wxTheColourDatabase->Find ( _T ( "CYAN" ) ), 95 );
			SetCaretLineBackground ( baseBackground );

			SetSelBackground ( true, wxTheColourDatabase->Find ( _T ( "LIGHT GREY" ) ) );
			SetCaretForeground ( *wxBLACK );

			StyleSetForeground ( wxSTC_STYLE_DEFAULT, *wxBLACK );
			StyleSetBackground ( wxSTC_STYLE_DEFAULT, *wxWHITE );
			StyleClearAll();

			break;
		default:
			break;
	}
	if ( type == FILE_TYPE_CSS )
	{
		StyleSetBold ( wxSTC_CSS_CLASS, true );
		StyleSetBold ( wxSTC_CSS_PSEUDOCLASS, true );
		StyleSetBold ( wxSTC_CSS_UNKNOWN_PSEUDOCLASS, true );
		StyleSetBold ( wxSTC_CSS_DOUBLESTRING, true );
		StyleSetBold ( wxSTC_CSS_SINGLESTRING, true );
		StyleSetBold ( wxSTC_CSS_DIRECTIVE, true );
	}
	else
	{
		StyleSetBold ( wxSTC_H_XMLSTART, true );
		StyleSetBold ( wxSTC_H_XMLEND, true );
		StyleSetBold ( wxSTC_H_ATTRIBUTEUNKNOWN, true );
		StyleSetBold ( wxSTC_H_ENTITY, true );
		StyleSetBold ( wxSTC_H_QUESTION, true );
		StyleSetBold ( wxSTC_H_SGML_ENTITY, true );
		StyleSetBold ( wxSTC_H_SGML_DEFAULT, true );
	}

	applyVisibilityState ( visibilityState );
}

void XmlCtrl::applyVisibilityState ( int state )
{
	if ( type == FILE_TYPE_BINARY )
		return;

	visibilityState = state;

	bool visible;

	// hide tags/attributes
	visible = ( state == HIDE_ATTRIBUTES || state == HIDE_TAGS ) ? false : true;
	StyleSetVisible ( wxSTC_H_OTHER, visible );
	StyleSetVisible ( wxSTC_H_ATTRIBUTE, visible );
	StyleSetVisible ( wxSTC_H_ATTRIBUTEUNKNOWN, visible );
	StyleSetVisible ( wxSTC_H_DOUBLESTRING, visible );
	StyleSetVisible ( wxSTC_H_SINGLESTRING, visible );

	// hide tags
	visible = ( state == HIDE_TAGS ) ? false : true;
	StyleSetVisible ( wxSTC_H_TAG, visible );
	StyleSetVisible ( wxSTC_H_TAGUNKNOWN, visible );
	StyleSetVisible ( wxSTC_H_TAGEND, visible );

	StyleSetVisible ( wxSTC_H_XMLSTART, visible );
	StyleSetVisible ( wxSTC_H_XMLEND, visible );
	StyleSetVisible ( wxSTC_H_CDATA, visible );
	StyleSetVisible ( wxSTC_H_QUESTION, visible );
	StyleSetVisible ( wxSTC_H_COMMENT, visible );
	for ( int i = wxSTC_H_SGML_DEFAULT; i <= wxSTC_H_SGML_BLOCK_DEFAULT; i++ )
		StyleSetVisible ( i, visible );

	Colourise ( 0, -1 );
}

int XmlCtrl::getType()
{
	return type;
}

void XmlCtrl::foldAll()
{
	expandFoldsToLevel ( 1, false );
}

void XmlCtrl::unfoldAll()
{
	expandFoldsToLevel ( wxSTC_FOLDLEVELNUMBERMASK, true );
}

void XmlCtrl::toggleFold()
{
	int pos, line, level;
	pos = GetCurrentPos();
	if ( pos == -1 )
		return;
	line = LineFromPosition ( pos );
	level = GetFoldLevel ( line );

	for ( int iterator = line; iterator > 0; iterator-- )
	{
		if ( iterator != line )
		{
			GotoLine ( iterator );
			level = GetFoldLevel ( iterator );
		}

		if ( XMLCTRL_HASBIT ( level, wxSTC_FOLDLEVELHEADERFLAG ) )
		{
			ToggleFold ( iterator );
			break;
		}
	}
}

// adapted from wxSTEdit (c) 2005 John Labenski, Otto Wyss
void XmlCtrl::expandFoldsToLevel ( int level, bool expand )
{
	Colourise ( 0, -1 );

	const int line_n = GetLineCount();
	for ( int n = 0; n < line_n; n++ )
	{
		int line_level = GetFoldLevel ( n );
		if ( XMLCTRL_HASBIT ( line_level, wxSTC_FOLDLEVELHEADERFLAG ) )
		{
			line_level -= wxSTC_FOLDLEVELBASE;
			line_level &= wxSTC_FOLDLEVELNUMBERMASK;

			if ( ( ( expand && ( line_level <= level ) ) ||
			        ( !expand && ( line_level >= level ) ) ) && ( GetFoldExpanded ( n ) != expand ) )
				ToggleFold ( n );
		}
	}

	EnsureCaretVisible(); // seems to keep it in nearly the same place
}

void XmlCtrl::protectHeadLine()
{
	if ( *protectTags || !properties.fold )
		return;

	int pos1, pos2, line1, line2, level;
	pos1 = GetSelectionStart();
	pos2 = GetSelectionEnd();
	line1 = LineFromPosition ( pos1 );
	line2 = LineFromPosition ( pos2 );

	if ( line2 < line1 )
	{
		int temp = line1;
		line1 = line2;
		line2 = temp;
	}

	for ( int i = line1; i <= line2; i++ )
	{
		level = GetFoldLevel ( i );
		if ( XMLCTRL_HASBIT ( level, wxSTC_FOLDLEVELHEADERFLAG ) && !GetFoldExpanded ( i ) )
			ToggleFold ( i );
	}
}

wxString XmlCtrl::getOpenTag ( const wxString& element )
{
	wxString openTag;
	openTag = _T ( "<" ) + element;
	std::set<wxString> requiredAttributeSet;
	std::set<wxString>::iterator it;
	requiredAttributeSet = requiredAttributeMap[element];
	if ( !requiredAttributeSet.empty() )
	{
		for ( it = requiredAttributeSet.begin(); it != requiredAttributeSet.end(); ++it )
		{
			openTag += _T ( " " );
			openTag += *it;
			openTag += _T ( "=\"\"" );
		}
	}
	openTag += _T ( ">" );
	return openTag;
}

bool XmlCtrl::insertChild ( const wxString& child )
{
	int start, end;
	start = GetSelectionStart();
	end = GetSelectionEnd();
	int offset;

	wxString openTag, closeTag;

	openTag = getOpenTag ( child );
	closeTag = _T ( "</" ) + child + _T ( ">" );

	if ( start == end )
	{
		if ( !canInsertAt ( start ) )
			return false;
		offset = openTag.Length();

		wxString tag;
		tag = openTag + closeTag;
		InsertText ( start, tag );
		SetSelection ( start + offset, start + offset );
		SetFocus();
		return true;
	}
	if ( *protectTags )
		adjustSelection();
	offset = openTag.Length();
	if ( start > end )
	{
		int temp = end;
		start = end;
		end = temp;
	}
	InsertText ( start, openTag );
	InsertText ( end + offset, closeTag );
	SetSelection ( start + offset, end + offset );
	return true;
}

bool XmlCtrl::insertSibling ( const wxString& sibling, const wxString& parent )
{
	int start = GetSelectionStart();
	int limit = GetLength();
	wxString parentCloseTag = _T ( "</" ) + parent + _T ( ">" );
	int parentCloseTagStart = FindText ( start, limit, parentCloseTag );
	if ( parentCloseTagStart == -1 ||
	        !canInsertAt ( parentCloseTagStart + parentCloseTag.Length() ) )
		return false;

	int insertionPoint = parentCloseTagStart + parentCloseTag.Length();
	SetSelection ( insertionPoint, insertionPoint );
	insertNewLine();

	wxString openTag, closeTag;
	int newCurrentPos, newAdjustedPos;
	newCurrentPos = GetCurrentPos();

	openTag = getOpenTag ( sibling );
	closeTag = _T ( "</" ) + sibling + _T ( ">" );

	InsertText ( newCurrentPos, openTag + closeTag );
	newAdjustedPos = newCurrentPos + openTag.Length();
	SetSelection ( newAdjustedPos, newAdjustedPos );
	return true;
}

bool XmlCtrl::insertEntity ( const wxString& entity )
{
	if ( *protectTags )
		adjustCursor();

	wxString insertBuffer;
	insertBuffer.Printf ( _T ( "&%s;" ), entity.c_str() );
	int pos = GetCurrentPos();
	InsertText ( GetCurrentPos(), insertBuffer );
	pos += insertBuffer.size();
	SetSelection ( pos, pos );
	return true;
}

wxString XmlCtrl::getParent()
{
	int current, parentCloseAngleBracket;
	current = GetCurrentPos();
	parentCloseAngleBracket = getParentCloseAngleBracket ( current );
	return getLastElementName ( parentCloseAngleBracket );
}

void XmlCtrl::insertNewLine()
{
	bool autoindent = false;
	int pos, line, startPos, iteratorPos, newPos;
	pos = GetCurrentPos();
	line = LineFromPosition ( pos );
	startPos = PositionFromLine ( line );
	iteratorPos = startPos;

	for ( iteratorPos = startPos;
	        ( GetCharAt ( iteratorPos ) == ' ' ||
	          GetCharAt ( iteratorPos ) == '\t' ) &&
	        iteratorPos < pos;
	        ++iteratorPos )
		autoindent = true;
	wxString s = GetTextRange ( startPos, iteratorPos );
	NewLine();
	if ( autoindent )
	{
		newPos = PositionFromLine ( line + 1 );
		InsertText ( newPos, s );
		SetSelection ( newPos + s.size(), newPos + s.size() );
	}
}

void XmlCtrl::toggleLineBackground()
{
	if ( !properties.toggleLineBackground || visibilityState != HIDE_TAGS )
	{
		if ( lineBackgroundState != BACKGROUND_STATE_NORMAL )
		{
			SetCaretLineBackground ( baseBackground );
			lineBackgroundState = BACKGROUND_STATE_NORMAL;
		}
		return;
	}
	lineBackgroundState = ( lineBackgroundState == BACKGROUND_STATE_NORMAL ) ?
	                      BACKGROUND_STATE_LIGHT : BACKGROUND_STATE_NORMAL;
	SetCaretLineBackground ( ( lineBackgroundState == BACKGROUND_STATE_NORMAL ) ? baseBackground : alternateBackground );
}

const std::set<wxString> &XmlCtrl::getEntitySet()
{
	return entitySet;
}

const std::set<std::string> &XmlCtrl::getAttributes ( const wxString& parent )
{
	static std::set<std::string> retVal;
	return retVal;
}

wxString XmlCtrl::getElementStructure ( const wxString& element )
{
	if ( elementStructureMap.find ( element ) == elementStructureMap.end() )
	{
		return wxEmptyString;
	}
	return elementStructureMap[element];
}

bool XmlCtrl::backgroundValidate()
{
	if ( !properties.validateAsYouType || type != FILE_TYPE_XML )
		return true;

	std::string bufferUtf8 = myGetTextRaw();

	return backgroundValidate (
		bufferUtf8.c_str(),
		basePath,
		bufferUtf8.size() );
}

bool XmlCtrl::backgroundValidate (
				const char *buffer,
				const wxString &system,
				size_t bufferLen
				)
{
	if ( !validationRequired )
		return true;

	wxCriticalSectionLocker locker ( xmlcopyeditorCriticalSection );

	if ( validationThread != NULL )
	{
		return true; // wait for next idle cycle call from main app frame
	}
	validationRequired = false;

	validationThread = new ValidationThread(
		GetEventHandler(),
		buffer,
		system
	);

	if ( validationThread->Create() != wxTHREAD_NO_ERROR
	    || validationThread->Run() != wxTHREAD_NO_ERROR )
	{
		delete validationThread;
		validationThread = NULL;
		return false;
	}

	return true;
}

std::string XmlCtrl::myGetTextRaw()
{
	return ( const char * ) GetTextRaw();
}

void XmlCtrl::setErrorIndicator ( int line, int column )
{
	int startPos, endPos, endStyled;
	startPos = PositionFromLine ( line ) + column;
	endPos = GetLineEndPosition ( line );
	endStyled = GetEndStyled();
	if ( endPos > endStyled ) endPos = endStyled;

	int length = endPos - startPos;
	if ( length > 0 && length + startPos < GetLength() )
	{
#if wxCHECK_VERSION(2,9,0)
		IndicatorFillRange ( startPos, length );
#else
		StartStyling ( startPos, wxSTC_INDIC2_MASK );
		SetStyling ( length, wxSTC_INDIC2_MASK );
#endif
	}
}

void XmlCtrl::clearErrorIndicators ( int maxLine )
{
	if ( maxLine < 0 )
		return;

	int length = GetLength();
	if ( !length )
		return;

	int end = GetEndStyled();
	length = ( maxLine ) ? GetLineEndPosition ( maxLine ) : length;
	if ( end > 0 && length > end ) length = end;

#if wxCHECK_VERSION(2,9,0)
	IndicatorClearRange ( 0, length );
#else
	StartStyling ( 0, wxSTC_INDIC2_MASK );
	SetStyling ( length, 0 );
#endif
}

bool XmlCtrl::getValidationRequired()
{
	return validationRequired;
}

void XmlCtrl::setValidationRequired ( bool b )
{
	validationRequired = b;
}

int XmlCtrl::getTagType ( int pos )
{
	int iteratorPos;

	// preliminary checks
	if ( pos < 2 )
	{
		return TAG_TYPE_ERROR;
	}
	else if ( GetCharAt ( pos - 1 ) == '/' )
	{
		return TAG_TYPE_EMPTY;
	}

	// go to start of tag
	for ( iteratorPos = pos; iteratorPos >= 0; --iteratorPos )
	{
		int style = getLexerStyleAt ( iteratorPos );
		//style &= ~wxSTC_INDIC2_MASK;

		if ( GetCharAt ( iteratorPos ) == '<' &&
		        ( style == wxSTC_H_TAG || style == wxSTC_H_TAGUNKNOWN ) )
			break;
	}
	if ( GetCharAt ( iteratorPos ) != '<' )
		return TAG_TYPE_ERROR;

	char c = GetCharAt ( iteratorPos + 1 );
	if ( c == '!' || c == '?' )
		return TAG_TYPE_OTHER;
	else if ( c == '/' )
	{
		return TAG_TYPE_CLOSE;
	}
	else
	{
		return TAG_TYPE_OPEN;
	}
}

// fetch style int disregarding indicator
int XmlCtrl::getLexerStyleAt ( int pos )
{
	int style = GetStyleAt ( pos );
#if !wxCHECK_VERSION(2,9,0)
	style &= ~wxSTC_INDIC2_MASK;
#endif
	return style;
}

bool XmlCtrl::getGrammarFound()
{
	return grammarFound;
}

void XmlCtrl::OnMiddleDown ( wxMouseEvent& event )
{
	if ( GetSelectionStart() == GetSelectionEnd() )
	{
		event.Skip();
		return;
	}
	Copy();
	long x, y;
	event.GetPosition ( &x, &y );
	int pastePosition = PositionFromPointClose ( x, y );
	if ( pastePosition == wxSTC_INVALID_POSITION )
	{
		event.Skip();
		return;
	}
	SetSelection ( pastePosition, pastePosition );
	Paste();
}

bool XmlCtrl::selectCurrentElement()
{
	if ( type != FILE_TYPE_XML )
		return false;

	Colourise ( 0, -1 );

	int pos = GetCurrentPos();
	int style = getLexerStyleAt ( pos ) ;
	if ( style == wxSTC_H_COMMENT )
	{
		int i = pos;
		while ( --i >= 0 && getLexerStyleAt ( i ) == wxSTC_H_COMMENT )
			continue;
		SetSelectionStart ( i + 1 );

		int styled = GetEndStyled();
		i = pos;
		while ( i < styled && getLexerStyleAt ( i ) == wxSTC_H_COMMENT )
			i++;
		SetSelectionEnd ( i );
	}
	else
	{
		// Select current tag
		int start = findPreviousStartTag ( pos, 1, '<', pos );
		if ( start < 0 )
		{
			MyFrame *frame = ( MyFrame * ) wxTheApp->GetTopWindow();
			frame->statusProgress ( _("Cannot find the start tag") );
			return false;
		}
		int range = GetTextLength() - pos;
		int end = findNextEndTag ( pos, 1, '>', range );
		if ( end < 0 )
		{
			MyFrame *frame = ( MyFrame * ) wxTheApp->GetTopWindow();
			frame->statusProgress ( _("Cannot find the end tag") );
			return false;
		}
		SetSelection ( start, end );
	}

	return true;
}

void XmlCtrl::toggleComment()
{
	MyFrame *frame = ( MyFrame * ) wxTheApp->GetTopWindow();
	frame->statusProgress ( wxEmptyString );

	int pos = -1;
	wxString commentStart = _T ( "<!--" );
	wxString commentEnd = _T ( "-->" );

	// Is there a selection?
	int from = GetSelectionStart();
	int to = GetSelectionEnd();
	switch ( type )
	{
	case FILE_TYPE_BINARY:
		return;

	case FILE_TYPE_CSS:
		if ( from == to )
			return;

		commentStart = _T ( "/*" );
		commentEnd = _T ( "*/" );
		break;

	case FILE_TYPE_XML:
		if ( from != to )
			break;

		// Select current element
		pos = GetCurrentPos();
		if ( !selectCurrentElement() )
			return;
		break;

	default:
		if ( from == to )
			return;
		break;
	}

	wxString text = GetSelectedText();
	wxASSERT ( !text.IsEmpty() );

	// Skip leading spaces
	wxString::iterator itr, start, end;
	itr = start = text.begin();
	end = text.end();
	while ( itr != end && wxIsspace ( *itr ) )
		++itr;

	size_t startPos = itr - start;
	int ret = text.compare ( startPos, commentStart.length(), commentStart );
	if ( ret == 0 )
	{
		start = itr;
		itr = end;
		do {
			--itr;
		} while ( itr != start && wxIsspace ( *itr ) );

		size_t endPos = itr - start;
		if ( endPos > commentEnd.length() )
		{
			endPos = itr - text.begin() - commentEnd.length() + 1;
			ret = text.compare ( endPos, commentEnd.length(), commentEnd );

			// Is commented?
			if ( ret == 0 )
			{
				text.erase ( endPos, commentEnd.length() );
				text.erase ( startPos, commentStart.length() );

				ReplaceSelection ( text );
				pos -= commentStart.length();
				if ( pos >= 0 )
					SetSelection ( pos, pos );
				return;
			}
		}
	}

	// Comment selection

	// "--" is not allowed in comments
	if ( commentStart == _T ( "<!--" ) )
	{
		const static wxString doubleHyphen = _T ( "--" );
		size_t offset = 0;
		while ( ( offset = text.find ( doubleHyphen, offset ) ) != wxString::npos )
		{
			text.replace ( offset, doubleHyphen.length(), _T ( "- -" ) );
			offset += 2; // WARNING: Not three!
		}
	}

	text = commentStart + text + commentEnd;

	ReplaceSelection ( text );
	if ( pos >= 0 )
	{
		pos += commentStart.length();
		SetSelection ( pos, pos );
	}
}
