
#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <windows.h>
#include <windowsx.h>
#include <uxtheme.h>

#include "resource.h"
#include "version.h"

#define WIN_WIDTH  300
#define WIN_HEIGHT 100
/*
AW_ACTIVATE     0x00020000 Activates the window. Do not use this value with AW_HIDE.
AW_BLEND        0x00080000 Uses a fade effect. This flag can be used only if hwnd is a top-level window.
AW_CENTER       0x00000010 Makes the window appear to collapse inward if AW_HIDE is used or expand outward if the AW_HIDE is not used. The various direction flags have no effect.
AW_HIDE         0x00010000 Hides the window. By default, the window is shown.

AW_HOR_POSITIVE 0x00000001 Animates the window from left to right. This flag can be used with roll or slide animation. It is ignored when used with AW_CENTER or AW_BLEND.
AW_HOR_NEGATIVE 0x00000002 Animates the window from right to left. This flag can be used with roll or slide animation. It is ignored when used with AW_CENTER or AW_BLEND.
AW_SLIDE        0x00040000 Uses slide animation. By default, roll animation is used. This flag is ignored when used with AW_CENTER.
AW_VER_POSITIVE 0x00000004 Animates the window from top to bottom. This flag can be used with roll or slide animation. It is ignored when used with AW_CENTER or AW_BLEND.
AW_VER_NEGATIVE 0x00000008 Animates the window from bottom to top. This flag can be used with roll or slide animation. It is ignored when used with AW_CENTER or AW_BLEND.
*/

// To appear
std::vector<std::pair<std::string, int>> anim_show_list = {
//  { "SLIDE_IN_LEFT",             0x00040001 },
  { "SLIDE_IN_RIGHT",            0x00040002 },
//  { "SLIDE_IN_TOP",              0x00040004 },
//  { "DIAG_SLIDE_IN_TOPLEFT",     0x00040005 },
//  { "DIAG_SLIDE_IN_TOPRIGHT",    0x00040006 },
  { "SLIDE_IN_BOTTOM",           0x00040008 },
//  { "DIAG_SLIDE_IN_BOTTOMLEFT",  0x00040009 },
  { "DIAG_SLIDE_IN_BOTTOMRIGHT", 0x0004000a },
  { "EXPLODE",                   0x00040010 },
  { "FADE_IN",                   0x00080000 },
};

// To disappear
std::vector<std::pair<std::string, int>> anim_hide_list = {
  { "SLIDE_OUT_RIGHT",            0x00050001 },
//  { "SLIDE_OUT_LEFT",             0x00050002 },
  { "SLIDE_OUT_BOTTOM",           0x00050004 },
  { "DIAG_SLIDE_OUT_BOTTOMRIGHT", 0x00050005 },
//  { "DIAG_SLIDE_OUT_BOTTOMLEFT",  0x00050006 },
//  { "SLIDE_OUT_TOP",              0x00050008 },
//  { "DIAG_SLIDE_OUT_TOPRIGHT",    0x00050009 },
//  { "DIAG_SLIDE_OUT_TOPLEFT",     0x0005000a },
  { "IMPLODE",                    0x00050010 },
  { "FADE_OUT",                   0x00090000 },
};


const char szClassName[] = "Hello World !";
COLORREF bgColor=RGB(0Xd6, 0xd9, 0x27);
HBRUSH bgColorBrush=CreateSolidBrush(bgColor);

void WinError(const char *fmt, ...) {
  CHAR *lpMsgBuf;
  char title[1024];
  DWORD len;
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(title, 1024, fmt, ap);
  va_end(ap);

  len=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
  lpMsgBuf[len-2]='\0';
  MessageBox(NULL, lpMsgBuf, title, MB_OK|MB_ICONERROR);
  LocalFree(lpMsgBuf);
}

void SysInfo () {
  std::cout << "SM_CXYFULLSCREEN    " << GetSystemMetrics(SM_CXFULLSCREEN)    << ", " << GetSystemMetrics(SM_CYFULLSCREEN) << std::endl;
  std::cout << "SM_CXYSCREEN        " << GetSystemMetrics(SM_CXSCREEN)        << ", " << GetSystemMetrics(SM_CYSCREEN) << std::endl;
  std::cout << "SM_CXYVIRTUALSCREEN " << GetSystemMetrics(SM_CXVIRTUALSCREEN) << ", " << GetSystemMetrics(SM_CYVIRTUALSCREEN) << std::endl;
  std::cout << "SM_XYVIRTUALSCREEN  " << GetSystemMetrics(SM_XVIRTUALSCREEN)  << ", " << GetSystemMetrics(SM_YVIRTUALSCREEN) << std::endl;

  std::cout << "Width of a border : " << GetThemeSysSize(NULL, SM_CXBORDER) << std::endl;
  std::cout << "Width of a scroll bar : " << GetThemeSysSize(NULL, SM_CXVSCROLL) << std::endl;
  std::cout << "Height of a scroll bar : " << GetThemeSysSize(NULL, SM_CXHSCROLL) << std::endl;
  std::cout << "Width of a caption : " << GetThemeSysSize(NULL, SM_CXSIZE) << std::endl;
  std::cout << "Height of a caption : " << GetThemeSysSize(NULL, SM_CYSIZE) << std::endl;
  std::cout << "Width of a small caption : " << GetThemeSysSize(NULL, SM_CXSMSIZE) << std::endl;
  std::cout << "Height of a small caption : " << GetThemeSysSize(NULL, SM_CYSMSIZE) << std::endl;
  std::cout << "Width of a menu bar : " << GetThemeSysSize(NULL, SM_CXMENUSIZE) << std::endl;
  std::cout << "Height of a menu bar : " << GetThemeSysSize(NULL, SM_CYMENUSIZE) << std::endl;
  std::cout << "Amount of border padding for captioned windows : " << GetThemeSysSize(NULL, SM_CXPADDEDBORDER) << std::endl;
  
  int winTitleHeight=(GetSystemMetrics(SM_CYFRAME)+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CXPADDEDBORDER));
  std::cout << "Window title bar height : " << winTitleHeight << std::endl;

  std::cout << "To appear\n";
  for(auto a:anim_show_list) {
    std::cout << a.first << ", " << a.second << std::endl << std::flush;
  }

  std::cout << "To disappear\n";
  for(auto a:anim_hide_list) {
    std::cout << a.first << ", " << a.second << std::endl << std::flush;
  }
}

void BringMeToTop (HWND hwnd) {
  SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  SetForegroundWindow(hwnd);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch(msg) {
    case WM_CREATE: {
      HWND hw=CreateWindow("STATIC", szClassName, WS_VISIBLE | WS_CHILD, 0, 0, 2*WIN_WIDTH, WIN_HEIGHT, hwnd, (HMENU)100, GetModuleHandle(NULL), (LPVOID)0); 
      HFONT hFont=CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,  CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,  VARIABLE_PITCH, TEXT("Impact"));
      SendMessage(hw, WM_SETFONT, (WPARAM)hFont, 0);

      SetTimer(hwnd, 1, 2000, NULL);
      return 0;
    }
    case WM_CTLCOLORSTATIC: {
      HDC hdcStatic=(HDC)wParam;
      //SetTextColor(hdcStatic, RGB(255,255,255));
      SetBkColor(hdcStatic, bgColor);
      return (INT_PTR)bgColorBrush;
    }
    case WM_TIMER:
      if ((UINT_PTR)wParam == 1) {
        static bool doShow=true;
        static size_t iVec;
        BringMeToTop(hwnd);

        if (doShow) {
          iVec=rand()%anim_show_list.size();
          std::cout << "Showing effect number " << iVec << " of " << anim_show_list.size() << " : " << anim_show_list[iVec].first << std::endl;
          AnimateWindow(hwnd, 1000, anim_show_list[iVec].second);
        } else {
          std::cout << "Hiding  effect number " << iVec << " of " << anim_hide_list.size() << " : " << anim_hide_list[iVec].first << std::endl;
          AnimateWindow(hwnd, 1000, anim_hide_list[iVec].second);
          if (iVec == anim_hide_list.size()-1) DestroyWindow(hwnd);
        }

        doShow=!doShow;
      }
      return 0;
    case WM_CLOSE:
      DestroyWindow(hwnd);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
  WNDCLASS wc;
  wc.style         = 0;
  wc.lpfnWndProc   = WndProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = hInstance;
  wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = bgColorBrush; // (COLOR_WINDOW+1);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = szClassName;
  wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINFRAME));

  if(!RegisterClass(&wc)) { WinError("RegisterClass"); return 0; }

  // Get typical windows title bar height
  int winTitleHeight=(GetSystemMetrics(SM_CYFRAME)+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CXPADDEDBORDER));
  HWND hwnd=CreateWindow(szClassName, 0, WS_POPUPWINDOW, GetSystemMetrics(SM_CXFULLSCREEN)-WIN_WIDTH, GetSystemMetrics(SM_CYFULLSCREEN)+winTitleHeight-WIN_HEIGHT, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL); 
  if (hwnd == NULL) { WinError("CreateWindow"); return 0; }
  SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)bgColorBrush);

  //UpdateWindow(hwnd);

  MSG Msg;
  while(GetMessage(&Msg, NULL, 0, 0)) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }

  return 0;
}

