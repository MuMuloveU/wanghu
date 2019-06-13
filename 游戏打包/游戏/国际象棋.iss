[Setup]
AppName=Íøºü6.6²âÊÔ
AppVerName=¹ú¼ÊÏóÆå
;²»ÄÜÑ¡Ôñ°²×°Â·¾¶
;°²×°Â·¾¶´Ó×¢²á±í¶ÁÈ¡
DisableDirPage        = yes
DefaultDirName={reg:HKCU\Software\WHQPGame\Plaza,Path}
OutputBaseFilename=Chess
Compression=zip
WizardImageFile=embedded\WizardImage.bmp
WizardSmallImageFile=embedded\WizardSmallImage.bmp

[Files]
Source: "{app}\Chess.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{app}\Chess\Locker.bmp"; DestDir: "{app}\Chess"; Flags: ignoreversion
Source: "{app}\Chess\TableFrame.DAT"; DestDir: "{app}\Chess"; Flags: ignoreversion
Source: "{app}\Chess\TableNormal.bmp"; DestDir: "{app}\Chess"; Flags: ignoreversion
Source: "{app}\Chess\TablePlaying.bmp"; DestDir: "{app}\Chess"; Flags: ignoreversion
Source: "{app}\Chess\UserReady.bmp"; DestDir: "{app}\Chess"; Flags: ignoreversion

[Run]
Filename: "{app}\GamePlaza.exe"; Description: "{cm:LaunchProgram,¹ú¼ÊÏóÆå}";

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
              PostMessage(hWnd, 1124, 201, 0);
              //MessageBox(0, 'PostMessage(hWnd, 1124, 5, 0);', 'MessageBox', MB_OK);
          end
     end
end;

//------------------------------------------------
//                   the end
//------------------------------------------------â
