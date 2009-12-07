#include <sstream>
#include "playlistrenamer.h"
#include "binaryfile.h"
#include "replace.h"
#include "wrapregex.h"
#include <wx/wx.h>

bool PlayListRenamer::run (
    const std::string& folder )
{
    std::string m3uFile, m3uBuffer;
    m3uFile = folder + "playlist.m3u";
    
    if ( !readFile ( m3uFile, m3uBuffer ) )
    {
        return false;
    }
    
    std::vector<std::string> lines;
    if ( !splitBuffer ( m3uBuffer, lines ) )
    {
        wxMessageBox ( _T("can't split buffer") );
        return false;
    }

    std::string title, from, to;
    size_t lineCount = lines.size();
    int trackNo = 0;
    
    wxString random;
    random.Printf ( _T("%i"), lineCount );
    wxMessageBox (random );
    for ( size_t i = 0; i < lineCount; i++ )
    {
        wxString line;
        line = wxString ( lines[i].c_str(), wxConvUTF8, lines[i].size() );
        wxMessageBox ( line );
        
        if ( lines[i][0] == '#' )
        {
            if ( lines[i].find ( "EXTINF" ) == std::string::npos )
                continue;

            //isolate m3u trackname
            std::string pattern0 = ".+?,";
            WrapRegex re0 ( pattern0, true );
            int replacements;
            title = re0.replaceGlobal ( lines[i], &replacements );

            bool abbrev = false;
            if ( title.size() > 22 )
            {
                title.erase ( 18 );
                abbrev = true;  
            }

            // no trailing whitespace
            std::string pattern1 = "\\s+$";
            WrapRegex re1 ( pattern1, false );
            title = re1.replaceGlobal ( title, &replacements );
    
            // no contiguous whitespace
            std::string pattern2 = "\\s\\s+";
            std::string replace2 = " ";
            WrapRegex re2 ( pattern2, false, replace2 );
            title = re2.replaceGlobal ( title, &replacements );
    
            // alphanumeric only
            std::string pattern3 = "[;\\.\\[\\],!^&*()]";
            WrapRegex re3 ( pattern3, false );
            title = re3.replaceGlobal ( title, &replacements );
    
            if ( abbrev )
                title.append ( "... " );
            
            title += ".mp3";                
        }
        else
        {
           from = lines[i];
           to = title;
           
            wxString wideFrom, wideTo;
            wideFrom = wxString ( from.c_str(), wxConvUTF8, from.size() );
            wideTo = wxString ( to.c_str(), wxConvUTF8, to.size() );

           renameFile ( from, to, folder );
           editFiles ( from, to, folder );
           
           title.clear();
        }
    }       
}

bool PlayListRenamer::readFile ( const std::string& path, std::string& buffer )
{    
    buffer.clear();
    try {
        BinaryFile bf ( path.c_str() );
        buffer.append ( bf.getData(), bf.getDataLen() );
    }
    catch ( ... )
    {
        return false;
    }
    return true;
}

bool PlayListRenamer::splitBuffer ( const std::string& buffer, std::vector<std::string>& lineVector )
{
    wxMessageBox ( wxString ( buffer.c_str(), wxConvUTF8, buffer.size() ) );
    std::string line;
    lineVector.clear();
    size_t bufferSize = buffer.size();
    
    for ( size_t i = 0; i < bufferSize; i++ )
    {
        if ( buffer[i] == '\n' )
        {
            if ( !line.empty() )
            {
                lineVector.push_back ( line );
                line.clear();   
            }
        }
        else if ( buffer[i] == '\r' )
        {
            ;
        }
        else
            line.append ( buffer[i], 1 );
    }
    if ( !line.empty() )
        lineVector.push_back ( line );

    if ( lineVector.empty() )
        return false;

    return true;
}

std::string PlayListRenamer::numToString ( int i)
{
    std::stringstream sstr;
    sstr << i;
    
    return sstr.str();
}

void PlayListRenamer::renameFile ( const std::string& from, const std::string& to, const std::string& folder )
{
    wxString wideFrom, wideTo, wideFolder;
    wideFolder = wxString ( folder.c_str(), wxConvUTF8, folder.size() );
    wideFrom = wxString ( from.c_str(), wxConvUTF8, from.size() );
    wideTo = wxString ( to.c_str(), wxConvUTF8, to.size() );
    wxMessageBox ( wideFolder + wideFrom, wideFolder + wideTo );
    //wxRenameFile ( wideFolder + wideFrom, wideFolder + wideTo );
}

void PlayListRenamer::editFiles ( const std::string& from, const std::string& to, const std::string& folder )
{
    //tbd
}
