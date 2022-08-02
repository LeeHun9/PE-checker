#include <windows.h>
#include "Dialog_Main.h"
#include<string.h>

#define SHELLPATH "./aftershell.exe"

BOOL CALLBACK ShellDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void StartShell(HWND hwdDlg);