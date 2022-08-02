#ifndef DIALOG_PE_20191212
#define DIALOG_PE_20191212
#pragma once
#include <windows.h>
#include "PETOOL.h"
#include "Resource.h"
extern PVOID g_pFileBuffer;
extern DWORD g_dwFileSize;


//º¯ÊýÉùÃ÷
BOOL CALLBACK PEDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif // !DIALOG_PE_20191212
