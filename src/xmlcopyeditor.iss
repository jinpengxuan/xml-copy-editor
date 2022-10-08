;
; Copyright 2012-2020 Zane U. Ji.
;
; This file is part of Xml Copy Editor.
;
; Xml Copy Editor is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
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

#define OSArchitecture "x64"
;#define OSArchitecture "x86"

#define MyAppName "XML Copy Editor"
#define MyAppPublisher "Zane U. Ji"
#define MyAppURL "https://sourceforge.net/projects/xml-copy-editor/"
#define MyAppExeName "xmlcopyeditor.exe"
#if OSArchitecture == "x64"
  #define MyAppVersion GetFileVersion(".\ReleaseWx-x64\XmlCopyEditor.exe")
  #define MyAppDir "ReleaseWx-x64"
  #define MinGW "D:\msys64\mingw64"
  #define GccExceptionHandling "seh"
  #define DllNameSuffix "-x64"
#else
  #define MyAppVersion GetFileVersion(".\ReleaseWx-x86\XmlCopyEditor.exe")
  #define MyAppDir "ReleaseWx-x86"
  #define MinGW "D:\msys64\mingw32"
  #define GccExceptionHandling "dw2"
  #define DllNameSuffix ""
#endif

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
;SetupIconFile=.\res\appicon.ico
Compression=lzma
SolidCompression=yes
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
#if "x64" == OSArchitecture
  ; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
  ; done in "64-bit mode" on x64, meaning it should use the native
  ; 64-bit Program Files directory and the 64-bit view of the registry.
  ; On all other architectures it will install in "32-bit mode".
  ArchitecturesInstallIn64BitMode=x64
  ArchitecturesAllowed=x64
#endif
OutputDir=..
OutputBaseFilename=xmlcopyeditor-{#MyAppVersion}-{#OSArchitecture}-install

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
Source: ".\{#MyAppDir}\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGW}\bin\libaspell-15.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libbrotlicommon.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libbrotlidec.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libcrypto-1_1{#DllNameSuffix}.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libcurl-4.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libexpat-1.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libgcc_s_{#GccExceptionHandling}-1.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libiconv-2.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libicudt71.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libicuuc71.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libidn2-0.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libintl-8.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\liblzma-5.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libnghttp2-14.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libpcre2-8-0.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libpng16-16.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libpsl-5.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libssh2-1.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libssl-1_1{#DllNameSuffix}.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libstdc++-6.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libunistring-2.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libwinpthread-1.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libxerces-c-3-2.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libxml2-2.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libxslt-1.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\libzstd.dll"; DestDir: "{app}"
Source: "{#MinGW}\bin\zlib1.dll"; DestDir: "{app}"
Source: "{#MinGW}\ssl\certs\ca-bundle.trust.crt"; DestDir: "{app}"; DestName: "curl-ca-bundle.crt"; Flags: ignoreversion
Source: ".\aspell\*"; DestDir: "{app}\aspell"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\bin\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\catalog\*"; DestDir: "{app}\catalog"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\copying\*"; DestDir: "{app}\copying"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\dtd\*"; DestDir: "{app}\dtd"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\help\xmlcopyeditor.chm"; DestDir: "{app}\help"; Flags: recursesubdirs createallsubdirs
Source: ".\png\*png"; DestDir: "{app}\png"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\po\ca.gmo"; DestDir: "{app}\po\ca"; DestName: "xmlcopyeditor.mo"; Flags: ignoreversion
Source: "..\po\de.gmo"; DestDir: "{app}\po\de"; DestName: "xmlcopyeditor.mo"; Flags: ignoreversion
Source: "..\po\en_US.gmo"; DestDir: "{app}\po\en_US"; DestName: "xmlcopyeditor.mo"; Flags: ignoreversion
Source: "..\po\es.gmo"; DestDir: "{app}\po\es"; DestName: "xmlcopyeditor.mo"; Flags: ignoreversion
Source: "..\po\fr.gmo"; DestDir: "{app}\po\fr"; DestName: "xmlcopyeditor.mo"; Flags: ignoreversion
Source: "..\po\it.gmo"; DestDir: "{app}\po\it"; DestName: "xmlcopyeditor.mo"; Flags: ignoreversion
Source: "..\po\km.gmo"; DestDir: "{app}\po\km"; DestName: "xmlcopyeditor.mo"; Flags: ignoreversion
Source: "..\po\nl.gmo"; DestDir: "{app}\po\nl"; DestName: "xmlcopyeditor.mo"; Flags: ignoreversion
Source: "..\po\ru.gmo"; DestDir: "{app}\po\ru"; DestName: "xmlcopyeditor.mo"; Flags: ignoreversion
Source: "..\po\sk.gmo"; DestDir: "{app}\po\sk"; DestName: "xmlcopyeditor.mo"; Flags: ignoreversion
Source: "..\po\sv.gmo"; DestDir: "{app}\po\sv"; DestName: "xmlcopyeditor.mo"; Flags: ignoreversion
Source: "..\po\uk_UA.gmo"; DestDir: "{app}\po\uk_UA"; DestName: "xmlcopyeditor.mo"; Flags: ignoreversion
Source: "..\po\zh_CN.gmo"; DestDir: "{app}\po\zh_CN"; DestName: "xmlcopyeditor.mo"; Flags: ignoreversion
Source: "..\po\zh_TW.gmo"; DestDir: "{app}\po\zh_TW"; DestName: "xmlcopyeditor.mo"; Flags: ignoreversion
Source: "..\po\*.png"; DestDir: "{app}"
Source: "..\po\translate.txt"; DestDir: "{app}"
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
	Result := IsAdminInstallMode or IsPowerUserLoggedOn;
end;

function DefDirRoot(Param: String): String;
begin
	if not IsAdmin then
		Result := ExpandConstant('{localappdata}')
	else
		Result := ExpandConstant('{pf}')
end;