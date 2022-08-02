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
//��̬���룬��Ҫ����Ŀ�ļ����·��ö�Ӧ lib �ļ�
#include <commctrl.h>				
#pragma comment(lib,"comctl32.lib")		
#define _CRT_SECURE_NO_WARNINGS



// �˴���ģ���а����ĺ�����ǰ������:
BOOL CALLBACK MainDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitProListView(HWND hwndDlg);
void InitMouListView(HWND hDlg);
void EnProcList(HWND hListProcess);
void EnMoudleList();
BOOL GetMainModuleInfo(DWORD dwPid, MODULEINFO* mi);
BOOL GetAllModules(DWORD dwPid, HMODULE* pModuel, int nSize, DWORD* dwNumOfModules);
void PECHECK(HWND hwndDlg);


#endif 