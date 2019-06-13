; 脚本由 Inno Setup 脚本向导 生成！
; 有关创建 Inno Setup 脚本文件的详细资料请查阅帮助文档！

[Setup]
; 注: AppId的值为单独标识该应用程序。
; 不要为其他安装程序使用相同的AppId值。
; (生成新的GUID，点击 工具|在IDE中生成GUID。)
AppName=棋牌游戏中心
AppVerName=棋牌游戏中心
DefaultDirName={pf}\foxqp
DefaultGroupName=棋牌游戏中心
OutputBaseFilename=GamePlaza
Compression=zip
WizardImageFile=embedded\WizardImage.bmp
WizardSmallImageFile=embedded\WizardSmallImage.bmp

[Languages]
Name: "chinesesimp"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "D:\fox66\演示打包\游戏大厅\{app}\gameplaza.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\fox66\演示打包\游戏大厅\{app}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; 注意: 不要在任何共享系统文件上使用“Flags: ignoreversion”


[Registry]
Root: HKCU; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "msacm.eqa"; ValueType: String; ValueData: "audio.acm"; Flags: createvalueifdoesntexist uninsdeletevalue deletevalue
Root: HKCU; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "vidc.MVE4"; ValueType: String; ValueData: "video.dll"; Flags: createvalueifdoesntexist uninsdeletevalue deletevalue
Root: HKLM; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "msacm.eqa"; ValueType: String; ValueData: "audio.acm"; Flags: createvalueifdoesntexist uninsdeletevalue deletevalue
Root: HKLM; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "vidc.MVE4"; ValueType: String; ValueData: "video.dll"; Flags: createvalueifdoesntexist uninsdeletevalue deletevalue
Root: HKCU; Subkey: "Software\WHQPGame\Plaza"; ValueName: "Path"; ValueType: String; ValueData: "{app}"; Flags: createvalueifdoesntexist uninsdeletevalue deletevalue
Root: HKCU; Subkey: "Software\WHQPGame\GamePlaza\OptionLogon"; ValueName: "LogonServer"; ValueType: String; ValueData: "218.26.89.169"; Flags: createvalueifdoesntexist uninsdeletevalue deletevalue
Root: HKCU; Subkey: "Software\WHQPGame\GamePlaza\LogonServer"; ValueName: "ServerAddr"; ValueType: String; ValueData: "218.26.89.169"; Flags: createvalueifdoesntexist uninsdeletevalue deletevalue
Root: HKCU; Subkey: "Software\WHQPGame\GamePlaza\LogonServer\网通用户"; ValueName: "ServerAddr"; ValueType: String; ValueData: "0d3f291b6b5bd6f8ebda0d3b291f6b45d6e7ebdc0d3f29076b5dd6faeb42"; Flags: createvalueifdoesntexist uninsdeletevalue deletevalue

[Icons]
Name: "{group}\棋牌游戏中心"; Filename: "{app}\gameplaza.exe"
Name: "{commondesktop}\棋牌游戏中心"; Filename: "{app}\gameplaza.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\棋牌游戏中心"; Filename: "{app}\gameplaza.exe"; Tasks: quicklaunchicon
Name: "{group}\{cm:UninstallProgram,视频棋牌}"; Filename: "{uninstallexe}";


[Run]
Filename: "{app}\gameplaza.exe"; Description: "{cm:LaunchProgram,棋牌游戏中心}"; Flags: nowait postinstall skipifsilent

