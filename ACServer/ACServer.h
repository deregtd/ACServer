
#if !defined(AFX_ACServer_H__F7DA1A92_E9A7_4AAA_B87C_D8C973E52505__INCLUDED_)
#define AFX_ACServer_H__F7DA1A92_E9A7_4AAA_B87C_D8C973E52505__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

int CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID CALLBACK IncTimer( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime );
long CALLBACK InvisWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID CALLBACK DirTimer( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime );
int CALLBACK OptionsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int CALLBACK SrvMsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif // !defined(AFX_ACServer_H__F7DA1A92_E9A7_4AAA_B87C_D8C973E52505__INCLUDED_)
