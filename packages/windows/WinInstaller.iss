; Distributing "Raging MIDI"
;==================================
; 1 - place RagingMidi.exe and --THESE DLLS-- <repo_root>\binary
;     D3DCompiler_43.dll
;     icudt49.dll
;     icuin49.dll
;     icuuc49.dll
;     libEGL.dll
;     libgcc_s_sjlj-1.dll
;     libGLESv2.dll
;     libstdc++-6.dll
;     libwinpthread-1.dll
;     Qt5Core.dll
;     Qt5Gui.dll
;     Qt5OpenGL.dll
;     Qt5Widgets.dll
;   --AND ALSO THESE QT PLUGIN DLLs--
;     [[all NON-DEBUG "imageformats" plugins]]
;     accessible/qtaccessiblewidgets.dll
;     platforms/qwindows.dll
; 2 - run the Inno Setup compiler
; 3 - create a .zip file for those allergic to installers

#define AppName "Raging MIDI"
#define AppVersion "0.0.1"
#define AppPublisher "waddlesplash"
#define AppURL "http://sourceforge.net/projects/ragingmidi/"
#define AppExeName "RagingMidi.exe"
#define AppCopyright "© 2012-2013 WaddleSplash & contributors."

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{C9946E0B-DCCA-4385-8724-6B4FBE906D5E}
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion}
AppPublisher={#AppPublisher}
AppPublisherURL={#AppURL}
AppSupportURL={#AppURL}
AppUpdatesURL={#AppURL}
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
LicenseFile=Installer_License.txt
OutputDir=..\..\dist
OutputBaseFilename=ragingmidi-{#AppVersion}-setup
Compression=lzma
SolidCompression=True
SetupLogging=False
AlwaysShowGroupOnReadyPage=True
AlwaysShowDirOnReadyPage=True
AppCopyright={#AppCopyright}
VersionInfoCompany={#AppPublisher}
VersionInfoCopyright={#AppCopyright}
VersionInfoVersion={#AppVersion}
VersionInfoTextVersion={#AppVersion}
VersionInfoProductName={#AppName}
UninstallDisplayIcon={app}\{#AppExeName}.exe
WizardImageFile=InstallerFront.bmp
WizardSmallImageFile=InstallerSmall.bmp
SetupIconFile=InstallerIcon.ico

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
;Name: "basque"; MessagesFile: "compiler:Languages\Basque.isl"
;Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
;Name: "catalan"; MessagesFile: "compiler:Languages\Catalan.isl"
;Name: "czech"; MessagesFile: "compiler:Languages\Czech.isl"
;Name: "danish"; MessagesFile: "compiler:Languages\Danish.isl"
;Name: "dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
;Name: "finnish"; MessagesFile: "compiler:Languages\Finnish.isl"
;Name: "french"; MessagesFile: "compiler:Languages\French.isl"
;Name: "german"; MessagesFile: "compiler:Languages\German.isl"
;Name: "hebrew"; MessagesFile: "compiler:Languages\Hebrew.isl"
;Name: "hungarian"; MessagesFile: "compiler:Languages\Hungarian.isl"
;Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"
;Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"
;Name: "norwegian"; MessagesFile: "compiler:Languages\Norwegian.isl"
;Name: "polish"; MessagesFile: "compiler:Languages\Polish.isl"
;Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
;Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
;Name: "serbiancyrillic"; MessagesFile: "compiler:Languages\SerbianCyrillic.isl"
;Name: "serbianlatin"; MessagesFile: "compiler:Languages\SerbianLatin.isl"
;Name: "slovak"; MessagesFile: "compiler:Languages\Slovak.isl"
;Name: "slovenian"; MessagesFile: "compiler:Languages\Slovenian.isl"
;Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
;Name: "ukrainian"; MessagesFile: "compiler:Languages\Ukrainian.isl"

[Components]
Name: "core"; Description: "Core"; Types: compact custom full; Flags: fixed

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "..\..\binary\{#AppExeName}"; DestDir: "{app}"; Flags: ignoreversion; Components: core
Source: "..\..\binary\*.dll"; DestDir: "{app}"; Flags: ignoreversion; Components: core
Source: "..\..\binary\plugins\*"; DestDir: "{app}"; Flags: recursesubdirs; Components: core
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#AppExeName}"; WorkingDir: "{app}"
Name: "{group}\{cm:ProgramOnTheWeb,{#AppName}}"; Filename: "{#AppURL}"; Flags: preventpinning excludefromshowinnewinstall
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"; Flags: preventpinning excludefromshowinnewinstall
Name: "{commondesktop}\{#AppName}"; Filename: "{app}\{#AppExeName}"; WorkingDir: "{app}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#AppName}"; Filename: "{app}\{#AppExeName}"; WorkingDir: "{app}"; Tasks: quicklaunchicon
