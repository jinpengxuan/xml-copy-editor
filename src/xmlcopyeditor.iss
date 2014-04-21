;
; Copyright 2012-2013 Zane U. Ji.
;
; This file is part of Xml Copy Editor.
;
; Xml Copy Editor is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; version 2 of the License.
;
; Xml Copy Editor is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with Xml Copy Editor; if not, write to the Free Software
; Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
;

#define MyAppName "XML Copy Editor"
#define MyAppVersion GetFileVersion(".\ReleaseWx28-x86\XmlCopyEditor.exe")
#define MyAppPublisher "Zane U. Ji"
#define MyAppURL "https://sourceforge.net/projects/xml-copy-editor/"
#define MyAppDir32 "ReleaseWx28-x86"
#define MyAppDir64 "ReleaseWx28-x64"
#define MyAppExeName "xmlcopyeditor.exe"
#define MinGW32 "D:\MinGW32"
#define MinGW64 "D:\MinGW64"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={#MyAppName}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={code:DefDirRoot}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=.\copying\xmlcopyeditor\copying.txt
OutputDir=..
OutputBaseFilename=xmlcopyeditor-{#MyAppVersion}-install
;SetupIconFile=.\res\appicon.ico
Compression=lzma
SolidCompression=yes
PrivilegesRequired=none
; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on x64, meaning it should use the native
; 64-bit Program Files directory and the 64-bit view of the registry.
; On all other architectures it will install in "32-bit mode".
ArchitecturesInstallIn64BitMode=x64
; Note: We don't set ProcessorsAllowed because we want this
; installation to run on all architectures (including Itanium,
; since it's capable of running 32-bit code too).

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1
Name: "disable_registry"; Description: "Install without administrator privileges"; GroupDescription: "Other tasks"; Flags: unchecked exclusive
Name: "create_registry_entry"; Description: "Full desktop installation"; GroupDescription: "Other tasks"; Flags: exclusive; Check: IsAdmin
Name: "create_registry_entry\main"; Description: "Create Windows registry entry"
Name: "create_registry_entry\associate_xml"; Description: "Associate XML documents (*.xml)"
Name: "create_registry_entry\associate_dtd"; Description: "Associate DTDs (*.dtd)"
Name: "create_registry_entry\associate_ent"; Description: "Associate entity sets (*.ent)"
Name: "create_registry_entry\associate_rss"; Description: "Associate RSS (*.rss)"
Name: "create_registry_entry\associate_xsl"; Description: "Associate XSL stylesheets (*.xsl)"
Name: "create_registry_entry\associate_xsd"; Description: "Associate XML Schema definitions (*.xsd)"
Name: "create_registry_entry\associate_rng"; Description: "Associate RELEAX NG grammars (*.rng)"
Name: "create_registry_entry\associate_rnc"; Description: "Associate compact RELAX NG grammars (*.rnc)"
Name: "create_registry_entry\associate_lzx"; Description: "Associate OpenLaszlo (*.lzx)"
Name: "create_registry_entry\associate_xtm"; Description: "Associate XML Topic Maps (*.xtm, *xtmm)"
Name: "create_registry_entry\associate_xlf"; Description: "Associate XLIFF documents (*.xlf)"

[Registry]
Root: HKCR; Subkey: "Software\SourceForge Project"; Flags: uninsdeletekeyifempty; Tasks: create_registry_entry
Root: HKCR; Subkey: "Software\SourceForge Project\XML Copy Editor"; Flags: uninsdeletekey; Tasks: create_registry_entry
Root: HKCR; Subkey: "Software\SourceForge Project\XML Copy Editor\DefaultIcon"; ValueType: string; ValueData: "{app}\{#MyAppExeName},1"; Tasks: create_registry_entry
Root: HKCR; Subkey: "Software\SourceForge Project\XML Copy Editor\shell\open\command"; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: create_registry_entry
Root: HKLM; Subkey: "SOFTWARE\SourceForge Project"; Flags: uninsdeletekeyifempty; Tasks: create_registry_entry\main
Root: HKLM; Subkey: "SOFTWARE\SourceForge Project\XML Copy Editor"; Flags: uninsdeletekey; Tasks: create_registry_entry\main
Root: HKLM; Subkey: "SOFTWARE\SourceForge Project\XML Copy Editor"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Tasks: create_registry_entry\main
Root: HKCR; Subkey: ".xml"; ValueType: string; ValueData: "Software\SourceForge Project\XML Copy Editor"; Flags: uninsdeletevalue; Tasks: create_registry_entry\associate_xml
Root: HKCR; Subkey: ".dtd"; ValueType: string; ValueData: "Software\SourceForge Project\XML Copy Editor"; Flags: uninsdeletevalue; Tasks: create_registry_entry\associate_dtd
Root: HKCR; Subkey: ".ent"; ValueType: string; ValueData: "Software\SourceForge Project\XML Copy Editor"; Flags: uninsdeletevalue; Tasks: create_registry_entry\associate_ent
Root: HKCR; Subkey: ".rss"; ValueType: string; ValueData: "Software\SourceForge Project\XML Copy Editor"; Flags: uninsdeletevalue; Tasks: create_registry_entry\associate_rss
Root: HKCR; Subkey: ".xsl"; ValueType: string; ValueData: "Software\SourceForge Project\XML Copy Editor"; Flags: uninsdeletevalue; Tasks: create_registry_entry\associate_xsl
Root: HKCR; Subkey: ".xsd"; ValueType: string; ValueData: "Software\SourceForge Project\XML Copy Editor"; Flags: uninsdeletevalue; Tasks: create_registry_entry\associate_xsd
Root: HKCR; Subkey: ".rng"; ValueType: string; ValueData: "Software\SourceForge Project\XML Copy Editor"; Flags: uninsdeletevalue; Tasks: create_registry_entry\associate_rng
Root: HKCR; Subkey: ".rnc"; ValueType: string; ValueData: "Software\SourceForge Project\XML Copy Editor"; Flags: uninsdeletevalue; Tasks: create_registry_entry\associate_rnc
Root: HKCR; Subkey: ".lzx"; ValueType: string; ValueData: "Software\SourceForge Project\XML Copy Editor"; Flags: uninsdeletevalue; Tasks: create_registry_entry\associate_lzx
Root: HKCR; Subkey: ".xtm"; ValueType: string; ValueData: "Software\SourceForge Project\XML Copy Editor"; Flags: uninsdeletevalue; Tasks: create_registry_entry\associate_xtm
Root: HKCR; Subkey: ".xlf"; ValueType: string; ValueData: "Software\SourceForge Project\XML Copy Editor"; Flags: uninsdeletevalue; Tasks: create_registry_entry\associate_xlf

[Files]
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: ".\{#MyAppDir64}\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion; Check: Is64BitInstallMode
Source: "{#MinGW64}\bin\libgcc_s_seh-1.dll"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "{#MinGW64}\bin\libstdc++-6.dll"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "{#MinGW64}\bin\libwinpthread-1.dll"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "{#MinGW64}\x86_64-w64-mingw32\bin\libaspell-15.dll"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "{#MinGW64}\x86_64-w64-mingw32\bin\libcurl-4.dll"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "{#MinGW64}\x86_64-w64-mingw32\bin\libeay32.dll"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "{#MinGW64}\x86_64-w64-mingw32\bin\libexpat-1.dll"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "{#MinGW64}\x86_64-w64-mingw32\bin\libiconv-2.dll"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "{#MinGW64}\x86_64-w64-mingw32\bin\libpcre-1.dll"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "{#MinGW64}\x86_64-w64-mingw32\bin\libxml2-2.dll"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "{#MinGW64}\x86_64-w64-mingw32\bin\libxslt-1.dll"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "{#MinGW64}\x86_64-w64-mingw32\bin\ssleay32.dll"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "{#MinGW64}\x86_64-w64-mingw32\bin\zlib1.dll"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: ".\{#MyAppDir32}\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion; Check: not Is64BitInstallMode
Source: "{#MinGW32}\bin\libgcc_s_sjlj-1.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode
Source: "{#MinGW32}\bin\libstdc++-6.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode
Source: "{#MinGW32}\bin\libwinpthread-1.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode
Source: "{#MinGW32}\i686-w64-mingw32\bin\libaspell-15.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode
Source: "{#MinGW32}\i686-w64-mingw32\bin\libcurl-4.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode
Source: "{#MinGW32}\i686-w64-mingw32\bin\libeay32.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode
Source: "{#MinGW32}\i686-w64-mingw32\bin\libexpat-1.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode
Source: "{#MinGW32}\i686-w64-mingw32\bin\libiconv-2.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode
Source: "{#MinGW32}\i686-w64-mingw32\bin\libpcre-1.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode
Source: "{#MinGW32}\i686-w64-mingw32\bin\libxml2-2.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode
Source: "{#MinGW32}\i686-w64-mingw32\bin\libxslt-1.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode
Source: "{#MinGW32}\i686-w64-mingw32\bin\ssleay32.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode
Source: "{#MinGW32}\i686-w64-mingw32\bin\zlib1.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode
Source: "{#MinGW64}\bin\curl-ca-bundle.crt"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\aspell\*"; DestDir: "{app}\aspell"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\bin\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\catalog\*"; DestDir: "{app}\catalog"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\copying\*"; DestDir: "{app}\copying"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\dtd\*"; DestDir: "{app}\dtd"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\help\xmlcopyeditor.chm"; DestDir: "{app}\help"; Flags: recursesubdirs createallsubdirs
Source: ".\png\*png"; DestDir: "{app}\png"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\po\*.mo"; DestDir: "{app}\po"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\po\*.png"; DestDir: "{app}"
Source: ".\po\translate.txt"; DestDir: "{app}"
Source: ".\rng\*"; DestDir: "{app}\rng"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\rulesets\*"; DestDir: "{app}\rulesets"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\templates\*"; DestDir: "{app}\templates"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\xsl\*"; DestDir: "{app}\xsl"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{group}\XML Copy Editor Help"; Filename: "{app}\help\xmlcopyeditor.chm"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; Check: IsAdmin
Name: "{userdesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; Check: not IsAdmin
Name: "{commonappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon; Check: IsAdmin
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon; Check: not IsAdmin

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Code]
function GetUninstallString(): String;
var
  sUnInstPath: String;
  sUnInstallString: String;
begin
  sUnInstPath := ExpandConstant('Software\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppName}_is1');
  sUnInstallString := '';
  if RegQueryStringValue(HKLM, sUnInstPath, 'UninstallString', sUnInstallString) then
  else if RegQueryStringValue(HKCU, sUnInstPath, 'UninstallString', sUnInstallString) then
  else if Is64BitInstallMode then begin
    sUnInstPath := ExpandConstant('Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppName}_is1');
    if not RegQueryStringValue(HKLM, sUnInstPath, 'UninstallString', sUnInstallString) then
      RegQueryStringValue(HKCU, sUnInstPath, 'UninstallString', sUnInstallString);
  end;
  Result := sUnInstallString;
end;

// http://stackoverflow.com/questions/2000296/innosetup-how-to-automatically-uninstall-previous-installed-version
procedure CurStepChanged(CurStep: TSetupStep);
var
  iResult: Integer;
  sUninstall: String;
begin
  if (CurStep = ssInstall) then begin
    sUninstall := GetUninstallString();
    if sUninstall <> '' then begin
      sUninstall := RemoveQuotes(sUnInstall);
      //iResult := MsgBox('Warning: Old Version will be removed!', mbInformation, MB_OKCANCEL);
      //if iResult = IDOK then
        Exec(sUninstall, '/SILENT /NORESTART /SUPPRESSMSGBOXES','', SW_SHOWNORMAL, ewWaitUntilTerminated, iResult);
    end;
  end;
end;

function IsAdmin(): Boolean;
begin
	Result := IsAdminLoggedOn or IsPowerUserLoggedOn;
end;

function DefDirRoot(Param: String): String;
begin
	if not IsAdmin then
		Result := ExpandConstant('{localappdata}')
	else
		Result := ExpandConstant('{pf}')
end;