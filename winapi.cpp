
#include <iostream>
#include <filesystem>

#include "winapi.h"

int MessageBox(HWND hwnd, LPCWSTR text, LPCWSTR caption, UINT type) {
  return MessageBox(hwnd, text, caption, type);
}

int MessageBox(LPCWSTR text, LPCWSTR caption, UINT type) {
  return MessageBox(NULL, text, caption, type);
}

int MessageBox(HWND hwnd, const std::string text, const std::string caption, UINT type) {
  return MessageBox(hwnd, text.c_str(), caption.c_str(), type);
}

int MessageBox(const std::string text, const std::string caption, UINT type) {
  return MessageBox(NULL, text, caption, type);
}

int MessageBox(HWND hwnd, const std::wstring text, const std::wstring caption, UINT type) {
  return MessageBoxW(hwnd, text.c_str(), caption.c_str(), type);
}

int MessageBox(const std::wstring text, const std::wstring caption, UINT type) {
  return MessageBox(NULL, text, caption, type);
}


std::string ExpandEnvironmentStrings(const std::string s) {
  DWORD l=ExpandEnvironmentStrings(s.c_str(), NULL, 0);
  std::string ret("");

  if (l == 0) WinError("ExpandEnvironmentStrings");
  else {
    char *d=new char[l+1];
    if (ExpandEnvironmentStrings(s.c_str(), d, l) > 0) ret=d;
  }

  return ret;
}

char *ExpandEnvironmentStrings(const char *src) {
  return _strdup(ExpandEnvironmentStrings(std::string(src)).c_str());
}

std::string GetCurrentDirectory() {
#ifndef PURE_WINAPI
  return std::filesystem::current_path().string();
#else
  std::string ret="";
  DWORD l=GetCurrentDirectory(0, NULL);

  if (l > 0) {
    char buf[l+1];
    GetCurrentDirectory(l, buf);
    ret=buf;
  }

  return ret;
#endif
}


std::string GetWindowText(HWND hwnd) {
  std::string ret("");
  int l=GetWindowTextLength(hwnd);

  if (l > 0) {
    auto s=new char[l+2];
    if (GetWindowText(hwnd, s, l+1) > 0) {
      ret=std::string(s);
    }
  }

  return ret;
}

std::string GetDlgItemText(HWND hDlg, int nIDDlgItem) {
  return GetWindowText(GetDlgItem(hDlg, nIDDlgItem));
}

std::string GetDlgItemTextRES(HWND hDlg, int nIDDlgItem) {
  std::string ret("");
  int l=(int)SendDlgItemMessage(hDlg, nIDDlgItem, WM_GETTEXTLENGTH, 0, 0 );

  if (l > 0) {
    auto s=new char[l+2];
    GetDlgItemText(hDlg, nIDDlgItem, s, l+1);
    ret=std::string(s);
  }

  return ret;
}

bool SetWindowText(HWND hWnd, std::string text) {
  return SetWindowText(hWnd, text.c_str());
}

bool SetDlgItemText(HWND hDlg, int nIDDlgItem, std::string text) {
  return SetDlgItemText(hDlg, nIDDlgItem, text.c_str());
}

void WinError(const char* fmt, ...)
{
    CHAR* lpMsgBuf;
    char title[1024];
    DWORD len;
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(title, 1024, fmt, ap);
    va_end(ap);

    len = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
    lpMsgBuf[len - 2] = '\0';
    MessageBox(NULL, lpMsgBuf, title, MB_OK | MB_ICONERROR);
    LocalFree(lpMsgBuf);
}
