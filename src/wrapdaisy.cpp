#include <cstdio>
#include <iostream>
#include <fstream>
#include <wx/wx.h>
#include <wx/progdlg.h>
#include "wrapdaisy.h"
#include "wraptempfilename.h"
#include "wraplibxml.h"
#include "xmlprodnote.h"
#include "xmlsuppressprodnote.h"
#include "xmlcopyimg.h"
#include "binaryfile.h"
#include "replace.h"
#include "wrapregex.h"
#include "mp3album.h"
//#include "playlistrenamer.h"

#ifdef __WXMSW__
#include <wx/msw/ole/automtn.h>
#endif

WrapDaisy::WrapDaisy (
    MyFrame *frameParameter,
    const wxString& daisyDirParameter,
    const wxString& pathParameter ) :
    frame ( frameParameter ), daisyDir ( daisyDirParameter), path ( pathParameter )
{
    albumCover = daisyDir + _T("cover.jpg");
    blankImage = daisyDir + _T("blank.jpg");

    //wxFileName::SetCwd ( daisyDir ); // not working
    //_chdir ( systemCmd.c_str() ); // not working
    //wxFileName::SetCwd ( daisyCwd ); // not working
    //wxSetWorkingDirectory ( daisyDir ); // not working

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
    bool doc,
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

    // prepare dtbook location
    wxString dtbDir, dtbFilePath;
    
    dtbDir = folder + wxFileName::GetPathSeparator() + _T("dtbook");
    dtbFilePath =
        dtbDir +
        wxFileName::GetPathSeparator() +
        _T("dtbook.xml");
    
    wxFileName dtbDirTest ( dtbDir );
    bool dtbDirExists = dtbDirTest.DirExists();    

#ifdef __WXMSW__
    if ( wxMkDir ( dtbDir ) && !dtbDirExists )
#else
    if ( wxMkDir ( (const char *) dtbDir.mb_str( wxConvUTF8 ), 0 ) && !dtbDirExists )
#endif

    {
        error = _ ("Cannot create folder [b]") + dtbDir + _T("[/b]");
        return false;
    }

    std::string output;

    if ( !stylesheet.empty() ) // stylesheet found
    {
        // #1: convert to canonical XHTML
#if wxCHECK_VERSION(2,9,0)
    	pd->GetEventHandler()->ProcessPendingEvents();
#else
    	pd->ProcessPendingEvents();
#endif
        while (wxTheApp->Pending())
            wxTheApp->Dispatch();

        if ( !pd->Update ( 10, _T("Preparing canonical XHTML...") ) )
        {
            error = _T( "Cancelled" );
            return false;   
        }    
    
        WrapLibxml wrapLibxml;
            
        bool success = wrapLibxml.xslt ( stylesheet, fileIn );
       
        if ( !success )
        {
            error = wrapLibxml.getLastError();
            return false;   
        }
        
        output = wrapLibxml.getOutput();
        
        if ( output.empty() )
        {
            error = _ ("Empty XHTML file");
            return false;
        }
    }
    else // no stylesheet
    {
        BinaryFile bf( fileIn );
        if ( !bf.getData() )
        {
            output.append ( bf.getData(), bf.getDataLen() );
            error = _( "Cannot read [b]" ) + fileIn + ( _("[/b]") );
            return false;
        }
    }
    
    if ( suppressOptional )
    {
#if wxCHECK_VERSION(2,9,0)
    	pd->GetEventHandler()->ProcessPendingEvents();
#else
    	pd->ProcessPendingEvents();
#endif
        while (wxTheApp->Pending())
            wxTheApp->Dispatch();
        if ( !pd->Update ( 15, _("Suppressing optional production notes...") ) )
        {
            error = _ ( "Cancelled" );
            return false;
        }

    	auto_ptr<XmlSuppressProdnote> xsp ( new XmlSuppressProdnote() );
    	if ( !xsp->parse ( output.c_str() ) )
    	{
            frame->newDocument ( output );
            error = xsp->getLastError();
            return false;
        }
        output = xsp->getBuffer();
    }

    if ( quiet )
    {
        // #1.5: apply quiet setting if req'd
#if wxCHECK_VERSION(2,9,0)
    	pd->GetEventHandler()->ProcessPendingEvents();
#else
    	pd->ProcessPendingEvents();
#endif
        while (wxTheApp->Pending())
            wxTheApp->Dispatch();
        if ( !pd->Update ( 20, _("De-emphasizing production notes...") ) )
        {
            error = _ ( "Cancelled" );
            return false;
        }

    	auto_ptr<XmlProdnote> xp ( new XmlProdnote() );
    	if ( !xp->parse ( output.c_str() ) )
    	{
            frame->newDocument ( output );
            error = xp->getLastError();
            return false;
        }
        output = xp->getBuffer();
    }

    // prevent MIME type errors in href="www..." attributes
    Replace::run ( output, "href=\"www", "href=\"http://www", true );
    
    // remove em-space
    Replace::run ( output, "\xE2\x80\x83", " ", true );
    
    // remove blank paragraphs
    Replace::run ( output, "<p></p>", "", true );
    
    int replaceCount;
    WrapRegex regexParaWhitespace ( "<p>\\w+</p>", true );
    output = regexParaWhitespace.replaceGlobal ( output, &replaceCount );
    
    WrapRegex regexContiguousWhitespace ( "[\\t ]+", true, " " );
    output = regexContiguousWhitespace.replaceGlobal ( output, &replaceCount );

    // copy images
    wxString htmlDir, imagesDir, mediaDir;
    htmlDir = folder + wxFileName::GetPathSeparator() + _T("html");
    imagesDir = htmlDir + wxFileName::GetPathSeparator() + _T("images");
    mediaDir = htmlDir + wxFileName::GetPathSeparator() + _T("media");
    bool htmlDirExists, imagesDirExists, mediaDirExists;
    wxFileName htmlDirTest ( htmlDir );
    wxFileName imagesDirTest ( imagesDir );
    wxFileName mediaDirTest ( mediaDir );
    htmlDirExists = htmlDirTest.DirExists();
    imagesDirExists = imagesDirTest.DirExists();
    mediaDirExists = mediaDirTest.DirExists();    

#ifdef __WXMSW__
    if ( wxMkDir ( htmlDir ) && !htmlDirExists )
#else
    if ( wxMkDir ( (const char *) htmlDir.mb_str( wxConvUTF8 ), 0 ) && !htmlDirExists )
#endif
    {
        error = _ ("Cannot create HTML folder [b]") + htmlDir + _T("[/b]");
        return false;
    }

#ifdef __WXMSW__
    if ( wxMkDir ( imagesDir ) && !imagesDirExists )
#else
    if ( wxMkDir ( (const char *) imagesDir.mb_str( wxConvUTF8 ), 0 ) && !imagesDirExists )
#endif

    {
        error = _ ("Cannot create image folder [b]") + imagesDir + _T("[/b]");
        return false;
    }
    
#ifdef __WXMSW__
    if ( wxMkDir ( mediaDir ) && !mediaDirExists )
#else
    if ( wxMkDir ( (const char *) mediaDir.mb_str( wxConvUTF8 ), 0 ) && !mediaDirExists )
#endif

    {
        error = _ ("Cannot create folder [b]") + mediaDir + _T("[/b]");
        return false;
    }
        
    // copy images
#if wxCHECK_VERSION(2,9,0)
   	pd->GetEventHandler()->ProcessPendingEvents();
#else
   	pd->ProcessPendingEvents();
#endif
    while (wxTheApp->Pending())
        wxTheApp->Dispatch();

    if ( !pd->Update ( 25, _("Copying files...") ) )
    {
        error = _ ( "Cancelled" );
        return false;
    }

	auto_ptr<XmlCopyImg> xci (
        new XmlCopyImg ( blankImage, imagesDir, mediaDir, path )
    );
	if ( !xci->parse ( output.c_str() ) )
	{
        frame->newDocument ( output );
        error = xci->getLastError();
        return false;
    }
    output = xci->getBuffer();
    
    // write out canonical file
    wxString canonicalFile = htmlDir + wxFileName::GetPathSeparator() + _T("index.html");
    std::string stdCanonicalFile = ( const char *) canonicalFile.mb_str ( wxConvUTF8 );
    std::ofstream canonicalStream ( stdCanonicalFile.c_str() );//canonicalFile.name().c_str() );
    if ( !canonicalStream )
    {
        error = _ ( "Cannot write canonical XHTML file" );
        return false;   
    }
    canonicalStream << output.c_str() << std::endl;
    canonicalStream.close();
    
    // #2: convert to DTBook
#if wxCHECK_VERSION(2,9,0)
   	pd->GetEventHandler()->ProcessPendingEvents();
#else
   	pd->ProcessPendingEvents();
#endif
    while (wxTheApp->Pending())
        wxTheApp->Dispatch();

    if ( !pd->Update ( 40, _ ("Preparing DTBook...") ) )
    {
        error = _ ( "Cancelled" );
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
        _T(" --inputFile=\"") +
        canonicalFile +
        _T("\" --outputFile=\"") +
        dtbFilePath + _T("\"");

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
        
    // #2.5: create ePub version
#if wxCHECK_VERSION(2,9,0)
   	pd->GetEventHandler()->ProcessPendingEvents();
#else
   	pd->ProcessPendingEvents();
#endif
    while (wxTheApp->Pending())
        wxTheApp->Dispatch();

    if ( epub )
    {
        if ( !pd->Update ( 50, _T("Preparing ePub...") ) )
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
            canonicalFile + //canonicalFile.wideName() +
            _T("\" --output=\"") +
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
    }

    // #2.9: convert to RTF
    if ( rtf || doc )
    {
#if wxCHECK_VERSION(2,9,0)
    	pd->GetEventHandler()->ProcessPendingEvents();
#else
   		pd->ProcessPendingEvents();
#endif
        while (wxTheApp->Pending())
            wxTheApp->Dispatch();
    
        if ( !pd->Update ( 60, _T("Preparing RTF...") ) )
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
    
        wxString rtfFile, tempRtfFile, docFile, tempDocFile;
        rtfFile = folder + wxFileName::GetPathSeparator() + _T("document.rtf");
        tempRtfFile = folder + wxFileName::GetPathSeparator() + _T("html") +
            wxFileName::GetPathSeparator() + _T("document.rtf");
     
        docFile = rtfFile;
        tempDocFile = tempRtfFile;
        docFile.Replace ( _T(".rtf"), _T(".doc") );
        tempDocFile.Replace ( _T(".rtf"), _T(".doc") );
        
        cmd = baseCmd +
            _T("\"") + rtfScript + _T("\" --input=\"") +
            dtbFilePath + //dtbFile.wideName() +
            _T("\" --output=\"") + rtfFile + 
            _T("\" --inclTOC=\"true\" --inclPagenum=\"false\"");
            
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
    
        // #2.9.5: convert to binary Word
        // (Win only; otherwise create copy with *.doc extension)
    
#if wxCHECK_VERSION(2,9,0)
        pd->GetEventHandler()->ProcessPendingEvents();
#else
   		pd->ProcessPendingEvents();
#endif
        while (wxTheApp->Pending())
            wxTheApp->Dispatch();
        
        if ( !pd->Update ( 60, _T("Preparing Word document...") ) )
        {
            error = _T ( "Cancelled" );
            return false;   
        }
    
        //wxString docFile = rtfFile;
        //docFile.Replace ( _T(".rtf"), _T(".doc") );
    
    #ifdef __WXMSW__
        wxAutomationObject wordObject, documentObject;
    
        if ( wordObject.CreateInstance ( _T("Word.Application") ) )
        {
            wxVariant openParams[2];
            openParams[0] = rtfFile;//tempRtfFile
            openParams[1] = false;   
    
            wordObject.CallMethod(_("documents.open"), 2, openParams);
            if (!wordObject.GetObject(documentObject, _("ActiveDocument"))) 
            { 
                error = _("Cannot open ") + rtfFile;//tempRtfFile;
                return false;
            }
            wxVariant saveAsParams[2];
            saveAsParams[0] = docFile;//tempDocFile;//
            saveAsParams[1] = (long)0; //wdFormatDocument
            if ( !documentObject.CallMethod(_("SaveAs"), 2, saveAsParams) )
            {
                //error = _("Cannot save ") + docFile;
                //return false;
            }
            documentObject.CallMethod(_("Close"), 0, NULL );
            wordObject.CallMethod(_T("Quit"), 0, NULL );
        }
    #else
        //wxCopyFile ( tempRtfFile, tempDocFile );
        wxCopyFile ( rtfFile, docFile );
    #endif
    
        //wxCopyFile ( tempRtfFile, rtfFile );
        //wxCopyFile ( tempDocFile, docFile );
        //wxRemoveFile ( tempRtfFile );
        //wxRemoveFile ( tempDocFile );
    }
    
    // #3: convert to full DAISY book
#if wxCHECK_VERSION(2,9,0)
   	pd->GetEventHandler()->ProcessPendingEvents();
#else
   	pd->ProcessPendingEvents();
#endif
    while (wxTheApp->Pending())
        wxTheApp->Dispatch();

    if ( !fullDaisy )
        return true; // no full DAISY, no audio
        
    if ( !pd->Update ( 70, _T("Preparing DAISY books...") ) )
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
        _T("\"") + narratorScript + _T("\" --input=\"") +
        dtbFilePath + _T("\"") +//dtbFile.wideName() +
        _T(" --outputPath=") +
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

    if ( !mp3Album )
        return true;

    // #4: create MP3 album
#if wxCHECK_VERSION(2,9,0)
   	pd->GetEventHandler()->ProcessPendingEvents();
#else
   	pd->ProcessPendingEvents();
#endif
    while (wxTheApp->Pending())
        wxTheApp->Dispatch();

    if ( !pd->Update ( 80, _T("Preparing MP3 album...") ) )
    {
        error = _T ( "Cancelled" );
        return false;   
    }
    
    // identify path to input file
    wxString folderWith202SmilFile, fileWith202SmilAttribs;
    folderWith202SmilFile =
        folder + wxFileName::GetPathSeparator() +
        _T("daisy202") + wxFileName::GetPathSeparator();
    fileWith202SmilAttribs = folderWith202SmilFile + _T("ncc.html");

    wxString taggerScript;
    
    taggerScript += _T("scripts");
    taggerScript += wxFileName::GetPathSeparator();
    taggerScript += _T("modify_improve");
    taggerScript += wxFileName::GetPathSeparator();
    taggerScript += _T("multiformat");
    taggerScript += wxFileName::GetPathSeparator();
    taggerScript += _T("AudioTagger.taskScript");

    //create mp3 folder
    wxString albumDir = folder + wxFileName::GetPathSeparator() + _T("mp3");
    wxFileName dirTest ( albumDir );
    bool dirExists = dirTest.DirExists();
#ifdef __WXMSW__
    if ( !wxMkDir ( albumDir ) && !dirExists )
#else
    if ( !wxMkDir ( (const char *) albumDir.mb_str( wxConvUTF8 ), 0 ) && !dirExists )
#endif

    {
        error = _ ("Cannot create MP3 album folder [b]") + albumDir + _T("[/b]");
        return false;
    }
    
    cmd = baseCmd +
        _T("\"") + taggerScript + _T("\" --audioTaggerInputFile=\"") +
        fileWith202SmilAttribs + _T("\" --audioTaggerOutputPath=") + // see filesetAudioTagger transformer
        _T("\"") +
        albumDir +
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
            error += _T("");
        }
    }
*/

    if ( !error.empty() )
        return false;
        

#if wxCHECK_VERSION(2,9,0)
   	pd->GetEventHandler()->ProcessPendingEvents();
#else
   	pd->ProcessPendingEvents();
#endif
    while (wxTheApp->Pending())
        wxTheApp->Dispatch();

/*
    if ( !pd->Update ( 90, _T("Updating playlists...") ) )
    {
        error = _T ( "Cancelled" );
        return false;   
    }


    //rename mp3 playlists
    //albumDir += wxFileName::GetPathSeparator();
    //PlayListRenamer plr;
    //std::string stdAlbumDir = ( const char *) albumDir.mb_str ( wxConvUTF8 );
    //plr.run ( stdAlbumDir );

    //rename mp3 files in //z3986/
    wxFileName fn ( dtbFilePath );
    wxString folderWithSmilFile, fileWithSmilAttribs;
    folderWithSmilFile =
        folder + wxFileName::GetPathSeparator() +
        _T("z3986") + wxFileName::GetPathSeparator();
    fileWithSmilAttribs = folderWithSmilFile + fn.GetFullName();

	auto_ptr<Mp3Album> ma ( new Mp3Album() );
	
	BinaryFile *binaryfile;
	binaryfile = new BinaryFile ( fileWithSmilAttribs );
	if ( !binaryfile->getData() )
	{
		error.Printf ( _ ( "Cannot open %s" ), fileWithSmilAttribs.c_str() );
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

    // redefine albumDir - remove when PlayListRename is ready
    albumDir = folder;
    albumDir += wxFileName::GetPathSeparator();
    
    std::string albumTitle = ma->getAlbumTitle();
    wxString wideAlbumTitle = wxString ( albumTitle.c_str(), wxConvUTF8, albumTitle.size() );
    albumDir += wideAlbumTitle;
#ifdef __WXMSW__
    albumDir.Replace ( _T("."), wxEmptyString );
    albumDir.Replace ( _T("?"), wxEmptyString );
#endif

    wxFileName dir ( albumDir );
    bool albumDirExists = dir.DirExists();
#ifdef __WXMSW__
    if ( !wxMkDir ( albumDir ) && !albumDirExists )
#else
    if ( !wxMkDir ( (const char *) albumDir.mb_str( wxConvUTF8 ), 0 ) && !albumDirExists )
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
*/
    
    return true;
}

const wxString &WrapDaisy::getLastError()
{
    return error;   
}
