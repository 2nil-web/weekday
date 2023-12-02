
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

int WIN_WIDTH=150;
int WIN_HEIGHT=50;


std::string szClassName;

bool ctrl_pressed() {
  return (GetKeyState(VK_CONTROL) & 0x80);
}

void BringMeToTop(HWND hwnd)
{
  SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  SetForegroundWindow(hwnd);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_CREATE: {
    std::string dw=getDayWeek();
    HWND hw = CreateWindow("STATIC", dw.c_str(), WS_VISIBLE | WS_CHILD, 0, 0, WIN_WIDTH, WIN_HEIGHT, hwnd,
                           (HMENU)100, GetModuleHandle(NULL), (LPVOID)0);
    HFONT hFont = CreateFont(24, 12, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Consolas"));
    SendMessage(hw, WM_SETFONT, (WPARAM)hFont, 0);
    return 0;
  }
  case WM_LBUTTONDOWN: {
    if (ctrl_pressed()) {
      ReleaseCapture();
      SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
    }
  } break;
  case WM_CONTEXTMENU: {
    if (ctrl_pressed()) {
      static HMENU hPopupMenu = NULL;
      if (hPopupMenu == NULL)
      {
        hPopupMenu = CreatePopupMenu();
        // AppendMenu(hPopupMenu, MF_BYPOSITION | MF_STRING, 101, "Configurer");
        AppendMenu(hPopupMenu, MF_BYPOSITION | MF_STRING, 102, "Quitter");
      }
      POINT cp;
      GetCursorPos(&cp);
      TrackPopupMenu(hPopupMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, cp.x, cp.y, 0, hwnd, NULL);
    }
  }
  break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam))
    {
    case 101:
      break;
    case 102:
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
  szClassName = "weekday";

  WNDCLASSEX wc;
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = 0; /* CS_HREDRAW | CS_VREDRAW ; */
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = DLGWINDOWEXTRA;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wc.lpszClassName = szClassName.c_str();
  wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINFRAME));
  wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINFRAME));

  if (!RegisterClassEx(&wc))
  {
    WinError("RegisterClass");
    return 0;
  }

  // Get typical windows title bar height
  HWND hwnd = CreateWindowEx(WS_EX_TOOLWINDOW, szClassName.c_str(), "", WS_BORDER,
                             GetSystemMetrics(SM_CXFULLSCREEN) - WIN_WIDTH,
                             GetSystemMetrics(SM_CYFULLSCREEN)-WIN_HEIGHT,
                             WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
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
