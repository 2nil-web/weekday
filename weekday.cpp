
#include <windows.h>
#include <windowsx.h>
#include <uxtheme.h>

#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "version.h"
#include "resource.h"
#include "winapi.h"
#include "calendar.h"
#include "registry.h"

int win_w = 150;
int win_h = 50;
int win_x, win_y;

const std::string REGKEY = "Software\\weekday";

#define trc std::cout << "File " << __FILE__ << ", LINE " << __LINE__ << std::endl;

std::string szClassName;

bool ctrl_pressed()
{
  return (GetKeyState(VK_CONTROL) & 0x80);
}

void BringMeToTop(HWND hwnd)
{
  SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  SetForegroundWindow(hwnd);
}

void GetPos(HWND hwnd)
{
  RECT rc;
  GetWindowRect(hwnd, &rc);
  win_x = rc.left;
  win_y = rc.top;
}

void ReadConf()
{
  win_x = GetRegInt(REGKEY, "x", GetSystemMetrics(SM_CXFULLSCREEN) - win_w);
  win_y = GetRegInt(REGKEY, "y", GetSystemMetrics(SM_CYFULLSCREEN) - win_h);
}

void SaveConf()
{
  PutRegInt(REGKEY, "x", win_x);
  PutRegInt(REGKEY, "y", win_y);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_CREATE: {
    std::string dw = getDayWeek();
    HWND hw = CreateWindow("STATIC", dw.c_str(), WS_VISIBLE | WS_CHILD, 0, 0, win_w, win_h, hwnd, (HMENU)100,
                           GetModuleHandle(NULL), (LPVOID)0);
    HFONT hFont = CreateFont(24, 12, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Consolas"));
    SendMessage(hw, WM_SETFONT, (WPARAM)hFont, 0);
    return 0;
  }
  case WM_LBUTTONDOWN: {
    if (ctrl_pressed())
    {
      ReleaseCapture();
      SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
      GetPos(hwnd);
    }
  }
  break;
  case WM_CONTEXTMENU: {
    if (ctrl_pressed())
    {
      static HMENU hPopupMenu = NULL;
      if (hPopupMenu == NULL)
      {
        hPopupMenu = CreatePopupMenu();
        // AppendMenu(hPopupMenu, MF_BYPOSITION | MF_STRING, MENU_CONF, "Configurer");
        AppendMenu(hPopupMenu, MF_BYPOSITION | MF_STRING, MENU_QUIT, "Quitter");
      }
      POINT cp;
      GetCursorPos(&cp);
      TrackPopupMenu(hPopupMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, cp.x, cp.y, 0, hwnd, NULL);
    }
  }
  break;
  case WM_DESTROY:
    SaveConf();
    PostQuitMessage(0);
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam))
    {
    case MENU_CONF:
      break;
    case MENU_QUIT:
      // SaveConf();
      DestroyWindow(hwnd);
      break;
    }
    return 0;
  default:
    break;
  }
  return (LONG)DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
  ReadConf();

  szClassName = "weekday";

  WNDCLASSEX wc;
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = 0;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = DLGWINDOWEXTRA;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName = (szClassName + "Menu").c_str();
  wc.lpszClassName = szClassName.c_str();
  wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINFRAME));
  wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINFRAME));

  if (!RegisterClassEx(&wc))
  {
    WinError("RegisterClass");
    return 0;
  }

  // Get typical windows title bar height
  HWND hwnd = CreateWindowEx(WS_EX_TOOLWINDOW, szClassName.c_str(), "", WS_BORDER, win_x, win_y, win_w, win_h, NULL,
                             NULL, hInstance, NULL);

  if (hwnd == NULL)
  {
    WinError("CreateWindow");
    return 0;
  }

  SetWindowLong(hwnd, GWL_STYLE, 0);
  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  MSG Msg;
  while (GetMessage(&Msg, NULL, 0, 0))
  {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }

  return 0;
}
