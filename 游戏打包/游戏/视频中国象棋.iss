[Setup]
AppName=Íøºü6.6²âÊÔ
AppVerName=ÊÓÆµÖĞ¹úÏóÆå
;²»ÄÜÑ¡Ôñ°²×°Â·¾¶
;°²×°Â·¾¶´Ó×¢²á±í¶ÁÈ¡
DisableDirPage        = yes
DefaultDirName={reg:HKCU\Software\WHQPGame\Plaza,Path}
OutputBaseFilename=chinachessvd
Compression=zip
WizardImageFile=embedded\WizardImage.bmp
WizardSmallImageFile=embedded\WizardSmallImage.bmp

[Files]
Source: "{app}\chinachessvd.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{app}\chinachessvd\Locker.bmp"; DestDir: "{app}\chinachessvd"; Flags: ignoreversion
Source: "{app}\chinachessvd\TableFrame.DAT"; DestDir: "{app}\chinachessvd"; Flags: ignoreversion
Source: "{app}\chinachessvd\TableNormal.bmp"; DestDir: "{app}\chinachessvd"; Flags: ignoreversion
Source: "{app}\chinachessvd\TablePlaying.bmp"; DestDir: "{app}\chinachessvd"; Flags: ignoreversion
Source: "{app}\chinachessvd\UserReady.bmp"; DestDir: "{app}\chinachessvd"; Flags: ignoreversion

[Run]
Filename: "{app}\GamePlaza.exe"; Description: "{cm:LaunchProgram,ÊÓÆµÖĞ¹úÏóÆå}";

[CustomMessages]
default.NameAndVersion=%1 °æ±¾ %2
default.AdditionalIcons=¸½¼Ó¿ì½İ·½Ê½:
default.CreateDesktopIcon=´´½¨×ÀÃæ¿ì½İ·½Ê½(&D)
default.CreateQuickLaunchIcon=´´½¨¿ìËÙÔËĞĞÀ¸¿ì½İ·½Ê½(&Q)
default.ProgramOnTheWeb=%1 ÍøÕ¾
default.UninstallProgram=Ğ¶ÔØ %1
default.LaunchProgram=ÔËĞĞ %1
default.AssocFileExtension=½« %2 ÎÄ¼şÀ©Õ¹ÃûÓë %1 ½¨Á¢¹ØÁª(&A)
default.AssocingFileExtension=ÕıÔÚ½« %2 ÎÄ¼şÀ©Õ¹ÃûÓë %1 ½¨Á¢¹ØÁª...

[Languages]
; These files are stubs
; To achieve better results after recompilation, use the real language files
Name: "default"; MessagesFile: "embedded\default.isl"; 

[Code]
//µ¼ÈëÒ»¸öWindows API º¯Êı
function MessageBox(hWnd: Integer; lpText, lpCaption: String; uType: Cardinal): Integer;
external 'MessageBoxA@user32.dll stdcall';

//°²×°Íê³Éºó
//¸üĞÂÓÎÏ·ÁĞ±í
procedure CurStepChanged(CurStep: TSetupStep);
var
  hWnd: HWND;

begin
    if CurStep=ssDone   then
      begin
        hWnd := FindWindowByClassName('WHQPGamePlaza');

        if hWnd <> 0 then
          begin
              PostMessage(hWnd, 1124, 506, 0);
              //MessageBox(0, 'PostMessage(hWnd, 1124, 5, 0);', 'MessageBox', MB_OK);
          end
     end
end;

//------------------------------------------------
//                   the end
//------------------------------------------------â
