#ifndef DIALOG_MAIN
#define DIALOG_MAIN
#pragma once
#include <windows.h>
#include <TlHelp32.h>
#include <stdlib.h>
#include <Psapi.h>
#include "Resource.h"
#include <commdlg.h>
#include "Dialog_PE.h"
#include "Dialog_Shell.h"
#include "framework.h"
#include "PETOOL.h"
//静态导入，需要在项目文件夹下放置对应 lib 文件
#include <commctrl.h>				
#pragma comment(lib,"comctl32.lib")		
#define _CRT_SECURE_NO_WARNINGS



// 此代码模块中包含的函数的前向声明:
BOOL CALLBACK MainDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitProListView(HWND hwndDlg);
void InitMouListView(HWND hDlg);
void EnProcList(HWND hListProcess);
void EnMoudleList();
BOOL GetMainModuleInfo(DWORD dwPid, MODULEINFO* mi);
BOOL GetAllModules(DWORD dwPid, HMODULE* pModuel, int nSize, DWORD* dwNumOfModules);
void PECHECK(HWND hwndDlg);


#endif 