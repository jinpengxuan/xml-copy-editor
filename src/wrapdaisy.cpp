#include <cstdio>
#include <iostream>
#include <fstream>
#include <wx/wx.h>
#include <wx/progdlg.h>
#include "wrapdaisy.h"
#include "wraptempfilename.h"
#include "wraplibxml.h"
#include "xmlprodnote.h"
#include "mp3album.h"
#include "binaryfile.h"

WrapDaisy::WrapDaisy ( const wxString& daisyDirParameter ) :
    daisyDir ( daisyDirParameter)
{
    albumCover = daisyDir + _T("cover.jpg");
    
    memoryCwd = wxFileName::GetCwd();
    daisyCwd = daisyDir +
        _T("pipeline-20090410") +
        wxFileName::GetPathSeparator();
    wxFileName::SetCwd ( daisyCwd );
    
    std::string systemCmd = "cd ";
    systemCmd += daisyCwd.mb_str ( wxConvUTF8 );
    system ( systemCmd.c_str() );
    
    classPath = _T("\"pipeline.jar\";\".\"");
    commandLineUI = _T("org.daisy.pipeline.ui.CommandLineUI");
    
    baseCmd = _T("java -classpath ") +
        classPath +
        _T( " " ) +
        commandLineUI +
         _T(" ");
}

WrapDaisy::~WrapDaisy()
{
    wxFileName::SetCwd ( memoryCwd );   
}

bool WrapDaisy::run (
    wxString& fileIn,
    wxString& stylesheet,
    wxString& folder,
    bool quiet,
    bool suppressOptional,
    bool epub,
    bool rtf,
    bool fullDaisy,
    bool mp3Album )
{
    fileIn.Replace ( _T("\\"), _T("/") );
    stylesheet.Replace ( _T("\\"), _T("/") );

    std::auto_ptr<wxProgressDialog> pd ( new wxProgressDialog (
        _ ( "Export in progress" ),
        _ ( "Initializing..." ),
        100,
        NULL,
        wxPD_SMOOTH | wxPD_CAN_ABORT ) );
    
    // #1: convert to canonical XHTML
    pd->ProcessPendingEvents();
    if ( !pd->Update ( 20, _T("Preparing canonical XHTML...") ) )
    {
        error = _T( "Cancelled" );
        return false;   
    }
    
    WrapTempFileName canonicalFile;
    WrapTempFileName dtbFile ( wxEmptyString, _T(".xml"));
    WrapLibxml wrapLibxml;
    std::string stdStylesheet, stdFileIn;
    stdStylesheet = stylesheet.mb_str ( wxConvUTF8 );
    stdFileIn = fileIn.mb_str ( wxConvUTF8 );
        
    bool success = wrapLibxml.xslt ( stdStylesheet, stdFileIn );
   
    if ( !success )
    {
        std::string stdError = wrapLibxml.getLastError();
        error = wxString ( stdError.c_str(), wxConvUTF8, stdError.size() );
        return false;   
    }
    
    std::string output = wrapLibxml.getOutput();
    
    if ( output.empty() )
    {
        error = _ ("Empty XHTML file");
        return false;
    }
    
    if ( quiet )
    {
        // #1.5: apply quiet setting if req'd
        pd->ProcessPendingEvents();
        if ( !pd->Update ( 20, _("De-emphasizing production notes...") ) )
        {
            error = _ ( "Cancelled" );
            return false;
        }

    	auto_ptr<XmlProdnote> xp ( new XmlProdnote() );
    	if ( !xp->parse ( output.c_str() ) )
    	{
            std::string stdError = xp->getLastError();
            error = wxString ( stdError.c_str(), wxConvUTF8, stdError.size() );
            return false;
        }
        output = xp->getBuffer();
    }
    
    std::ofstream canonicalStream ( canonicalFile.name().c_str() );
    if ( !canonicalStream )
    {
        error = _T( "Cannot write canonical XHTML file" );
        return false;   
    }
    canonicalStream << output.c_str() << std::endl;
    canonicalStream.close();
    
    // #2: convert to DTBook
    pd->ProcessPendingEvents();
    if ( !pd->Update ( 40, _T("Preparing DTBook...") ) )
    {
        error = _T ( "Cancelled" );
        return false;   
    }

    wxString xhtml2dtbookScript;
    
    xhtml2dtbookScript += _T("scripts");
    xhtml2dtbookScript += wxFileName::GetPathSeparator();
    xhtml2dtbookScript += _T("create_distribute");
    xhtml2dtbookScript += wxFileName::GetPathSeparator();
    xhtml2dtbookScript += _T("dtbook");
    xhtml2dtbookScript += wxFileName::GetPathSeparator();
    xhtml2dtbookScript += _T("Xhtml2Dtbook.taskScript");
    
    wxString cmd = baseCmd +
        xhtml2dtbookScript +
        _T(" --inputFile=") +
        canonicalFile.wideName() + _T(" --outputFile=") +
        dtbFile.wideName();

    wxArrayString out, err;
	
    int result = wxExecute ( cmd, out, err );
	
    int count = err.GetCount();
    if ( count )
    {
        for ( int i = 0; i < count; i++ )
        {
            error += err.Item ( i );
            error += _T(" ");
        }
    }
    /*count = out.GetCount();
    if ( count )
    {
        for ( int i = 0; i < count; i++ )
        {
            error += out.Item ( i );
            error += _T(" ");
        }
    }*/
    if ( !error.empty() )
        return false;
        
    // #2.5: create EPUB version
    pd->ProcessPendingEvents();
    if ( !pd->Update ( 50, _T("Transforming to EPUB ebook...") ) )
    {
        error = _T ( "Cancelled" );
        return false;   
    }

    wxString epubScript;
    
    epubScript += _T("scripts");
    epubScript += wxFileName::GetPathSeparator();
    epubScript += _T("create_distribute");
    epubScript += wxFileName::GetPathSeparator();
    epubScript += _T("epub");
    epubScript += wxFileName::GetPathSeparator();
    epubScript += _T("OPSCreator.taskScript");

    cmd = baseCmd +
        _T("\"") + epubScript + _T("\" --input=\"") +
        canonicalFile.wideName() + _T("\" --output=\"") +
        folder + wxFileName::GetPathSeparator() + _T("ebook.epub\"");
        
    result = wxExecute ( cmd, out, err );
	
    count = err.GetCount();
    if ( count )
    {
        for ( int i = 0; i < count; i++ )
        {
            error += err.Item ( i );
            error += _T(" ");
        }
    }
    
/*
    count = out.GetCount();
    if ( count )
    {
        for ( int i = 0; i < count; i++ )
        {
            error += out.Item ( i );
            error += _T(" ");
        }
    }
*/

    if ( !error.empty() )
        return false;

    // #2.9: convert to RTF
    pd->ProcessPendingEvents();
    if ( !pd->Update ( 50, _T("Transforming to RTF...") ) )
    {
        error = _T ( "Cancelled" );
        return false;   
    }

    wxString rtfScript;
    
    rtfScript += _T("scripts");
    rtfScript += wxFileName::GetPathSeparator();
    rtfScript += _T("create_distribute");
    rtfScript += wxFileName::GetPathSeparator();
    rtfScript += _T("text");
    rtfScript += wxFileName::GetPathSeparator();
    rtfScript += _T("DtbookToRtf.taskScript");

    cmd = baseCmd +
        _T("\"") + rtfScript + _T("\" --input=\"") +
        canonicalFile.wideName() + _T("\" --output=\"") +
        folder + wxFileName::GetPathSeparator() + _T("document.rtf\" --inclToc=\"true\" --inclPagenum=\"false\"");
        
    result = wxExecute ( cmd, out, err );
	
    count = err.GetCount();
    if ( count )
    {
        for ( int i = 0; i < count; i++ )
        {
            error += err.Item ( i );
            error += _T(" ");
        }
    }
    
/**/
    count = out.GetCount();
    if ( count )
    {
        for ( int i = 0; i < count; i++ )
        {
            error += out.Item ( i );
            error += _T(" ");
        }
    }
/**/

    if ( !error.empty() )
        return false;    

    // #3: convert to full DAISY book
    pd->ProcessPendingEvents();
    if ( !pd->Update ( 60, _T("Preparing DAISY book...") ) )
    {
        error = _T ( "Cancelled" );
        return false;   
    }

    wxString narratorScript;
    
    narratorScript += _T("scripts");
    narratorScript += wxFileName::GetPathSeparator();
    narratorScript += _T("create_distribute");
    narratorScript += wxFileName::GetPathSeparator();
    narratorScript += _T("dtb");
    narratorScript += wxFileName::GetPathSeparator();
    narratorScript += _T("Narrator-DtbookToDaisy.taskScript");

    cmd = baseCmd +
        _T("\"") + narratorScript + _T("\" --input=") +
        dtbFile.wideName() + _T(" --outputPath=") +
        _T("\"") +
        folder +
        _T("\"");
        
    result = wxExecute ( cmd, out, err );
	
    count = err.GetCount();
    if ( count )
    {
        for ( int i = 0; i < count; i++ )
        {
            error += err.Item ( i );
            error += _T(" ");
        }
    }
    
/*
    count = out.GetCount();
    if ( count )
    {
        for ( int i = 0; i < count; i++ )
        {
            error += out.Item ( i );
            error += _T(" ");
        }
    }
*/

    if ( !error.empty() )
        return false;
    
    // #4: create MP3 album
    pd->ProcessPendingEvents();
    if ( !pd->Update ( 80, _T("Preparing MP3 album...") ) )
    {
        error = _T ( "Cancelled" );
        return false;   
    }
    
    //rename mp3 files in //z3986/
    wxFileName fn ( dtbFile.wideName() );
    wxString folderWithSmilFile, fileWithSmilAttribs;
    folderWithSmilFile =
        folder + wxFileName::GetPathSeparator() +
        _T("z3986") + wxFileName::GetPathSeparator();
    fileWithSmilAttribs = folderWithSmilFile + fn.GetFullName();

    std::string file = ( const char *) fileWithSmilAttribs.mb_str ( wxConvUTF8 );

	auto_ptr<Mp3Album> ma ( new Mp3Album() );
	
	BinaryFile *binaryfile;
    try
	{
		binaryfile = new BinaryFile ( file.c_str() );
	}
	catch ( ... )
	{
		error.Printf ( _ ( "Cannot open %s" ), file.c_str() );
		return false;
	}
    
    if ( !ma->parse ( binaryfile->getData(), binaryfile->getDataLen() ) )
	{
        std::string stdError = ma->getLastError();
        error = wxString ( stdError.c_str(), wxConvUTF8, stdError.size() );
		delete binaryfile;
        return false;
    }
    
    delete binaryfile;

    wxString albumDir = folder;
    albumDir += wxFileName::GetPathSeparator();
    
    std::string albumTitle = ma->getAlbumTitle();
    wxString wideAlbumTitle = wxString ( albumTitle.c_str(), wxConvUTF8, albumTitle.size() );
    albumDir += wideAlbumTitle;
#ifdef __WXMSW__
    albumDir.Replace ( _T("."), wxEmptyString );
#endif

    wxFileName dir ( albumDir );
    bool dirExists = dir.DirExists();
#ifdef __WXMSW__
    if ( !wxMkDir ( albumDir ) && !dirExists )
#else
    if ( !wxMkDir ( (const char *) albumDir.mb_str( wxConvUTF8 ), 0 ) && !dirExists )
#endif
    {
        error = _ ("Cannot create MP3 album folder ") + albumDir;
        return false;
    }

    std::vector<std::pair<std::string, std::string> > v;
    ma->getFileNameVector ( v );
    size_t vectorSize = v.size();
    wxString sourcePath, destPath, wideSourceFile, wideDestFile;
    std::string sourceFile, destFile;
    for ( size_t i = 0; i < vectorSize; i++ )
    {
        sourceFile = v[i].first;
        destFile = v[i].second;
        
        wideSourceFile = wxString ( sourceFile.c_str(), wxConvUTF8, sourceFile.size() );
        wideDestFile = wxString ( destFile.c_str(), wxConvUTF8, destFile.size() );
        
        sourcePath = folderWithSmilFile + wideSourceFile;
        destPath = albumDir + wxFileName::GetPathSeparator() + 
            wideDestFile + _T(".mp3");
        wxCopyFile ( sourcePath, destPath, true );
    }
    wxString destAlbumCover = albumDir +
        wxFileName::GetPathSeparator() +
        _T("cover.jpg");
    wxCopyFile ( albumCover, destAlbumCover, true );
    
    return true;
}

wxString WrapDaisy::getLastError()
{
    return error;   
}
