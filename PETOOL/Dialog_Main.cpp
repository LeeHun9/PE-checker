// PETOOL.cpp : 定义应用程序的入口点。
#include "Dialog_Main.h"

#define _CRT_SECURE_NO_WARNINGS
#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
HWND hwndMain;
HWND g_hwndListProcess;
HWND g_hwndListModules;
PVOID g_pFileBuffer;
DWORD g_dwFileSize;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    hInst = hInstance;
    // TODO: 在此处放置代码。

    //告诉加载器用 DLL 中哪些类
    INITCOMMONCONTROLSEX icex;      //该结构用于从 dll 中加载公共空间信息，配合 InitCommonControlsEx 一起使用
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);

    //打开主对话框
    DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, MainDialogProc);

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}


// MaindlgProc
BOOL CALLBACK MainDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)		
{
    
    switch (uMsg)
    {
    case  WM_INITDIALOG:
        //在此写加载图标代码
        //.....
        InitProListView(hwndDlg);
        InitMouListView(hwndDlg);
        hwndMain = hwndDlg;
        break;

    case    WM_CLOSE:
        ExitProcess(0);
        break;

    // NOTIFY 用于父窗口接收 WM 控件消息
    case    WM_NOTIFY:
    {
        LPNMHDR pNMH = (LPNMHDR)lParam;
        if (wParam == IDC_LIST_PROCESS && pNMH->code == NM_CLICK) //点击对象是list，且是左键
        {
            EnMoudleList();
        }
        break;
    }
    
    // COMMAND 接收子窗体
    case  WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case   IDC_BUTTON_PE:
            PECHECK(hwndDlg);
            return TRUE;

        case   IDC_BUTTON_ADDSHELL:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_SHELL), hwndMain, ShellDialogProc);
            return TRUE;

        case   IDC_BUTTON_ABOUT:

            return TRUE;

        case   IDC_BUTTON_EXIT:
            ExitProcess(0);
            return TRUE;
        }
        break;
    }

    return FALSE;
}

//初始化 ProListControl
void InitProListView(HWND hDlg)
{
    LV_COLUMN lv;
    HWND hListProcess;

    //初始化								
    memset(&lv, 0, sizeof(LV_COLUMN));
    //获取IDC_LIST_PROCESS句柄								
    hListProcess = GetDlgItem(hDlg, IDC_LIST_PROCESS);

    //设置整行选中								
    SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //第一列								
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lv.pszText = (LPSTR)TEXT("进程");				//列标题				
    lv.cx = 200;								//列宽
    lv.iSubItem = 0;
    //ListView_InsertColumn(hListProcess, 0, &lv);								
    SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);     //插入列

    //第二列								
    lv.pszText = (LPSTR)TEXT("PID");
    lv.cx = 100;
    lv.iSubItem = 1;
    //ListView_InsertColumn(hListProcess, 1, &lv);								
    SendMessage(hListProcess, LVM_INSERTCOLUMN, 1, (DWORD)&lv);

    //第三列								
    lv.pszText = (LPSTR)TEXT("镜像基址");
    lv.cx = 100;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListProcess, 2, &lv);
    //第四列								
    lv.pszText = (LPSTR)TEXT("镜像大小");
    lv.cx = 100;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListProcess, 3, &lv);

    EnProcList(hListProcess);

    g_hwndListProcess = hListProcess;
}

//初始化 MouListControl
void InitMouListView(HWND hDlg)
{
    LV_COLUMN lv;
    HWND hListProcess;

    //初始化								
    memset(&lv, 0, sizeof(LV_COLUMN));
    //获取IDC_LIST_PROCESS句柄								
    hListProcess = GetDlgItem(hDlg, IDC_LIST_MOUDLE);

    //设置整行选中								
    SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //第一列								
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM; 
    lv.pszText = (LPSTR)TEXT("模块名称");				//列标题				
    lv.cx = 200;								        //列宽
    lv.iSubItem = 0;
    ListView_InsertColumn(hListProcess, 0, &lv);								

    //第二列								
    lv.pszText = (LPSTR)TEXT("模块位置");
    lv.cx = 300;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListProcess, 1, &lv);								
    
    g_hwndListModules = hListProcess;
}

//显示ProcList内容，InitProListView中调用
void EnProcList(HWND hListProcess)
{
    LV_ITEM vitem;  //ListView

    //初始化						
    memset(&vitem, 0, sizeof(LV_ITEM));
    vitem.mask = LVIF_TEXT;

    // call api 得到所有进程信息
    HANDLE hProcessSnap = NULL;
    PROCESSENTRY32 pe32 = { 0 };
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == (HANDLE)-1)
    {
        MessageBox(NULL, TEXT("CreateToolhelp32Snapshot failed"), TEXT("ERROR"),MB_OK);
        return;
    }
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hProcessSnap, &pe32))
    {
        do
        {
            TCHAR buffer[16];

            // 主模块信息
            MODULEINFO mi = { 0 };
            BOOL miResult = FALSE;
            miResult = GetMainModuleInfo(pe32.th32ProcessID, &mi);          //引用传参mi

            //第一列 进程名
            vitem.pszText = (LPSTR)TEXT(pe32.szExeFile);
            vitem.iItem = 0;
            vitem.iSubItem = 0;
            ListView_InsertItem(hListProcess, &vitem);

            // 第二列 PID
            _itoa_s(pe32.th32ProcessID, vitem.pszText,0x20,10);
            vitem.iItem = 0;
            vitem.iSubItem = 1;
            ListView_SetItem(hListProcess, &vitem);

            if (miResult)
            {
                // 第三列(主模块基地址)
                wsprintf(buffer, TEXT("%p"), mi.lpBaseOfDll);
                vitem.pszText = buffer;
                vitem.iSubItem = 2;
                ListView_SetItem(hListProcess, &vitem);

                // 第四列(镜像大小)
                wsprintf(buffer, TEXT("%p"), mi.SizeOfImage);
                vitem.pszText = buffer;
                vitem.iSubItem = 3;
                ListView_SetItem(hListProcess, &vitem);
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }
    else
    {
        MessageBox(NULL, TEXT("Unexpected ERROR"), TEXT("ERROR"), MB_OK);
        CloseHandle(hProcessSnap);
        return;
    }
    CloseHandle(hProcessSnap);
}

//显示ModList内容，点击ProcList时调用
void EnMoudleList()
{
    DWORD dwRowId;
    TCHAR szPid[0x20];
    LV_ITEM lv;

    //初始化
    memset(&lv, 0, sizeof(LV_ITEM));
    memset(szPid, 0, 0x20);

    //获取选中行ID
    dwRowId = SendMessage(g_hwndListProcess, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    if (dwRowId == -1) {
        MessageBox(NULL, TEXT("请选择进程"), TEXT("出错了"), MB_OK);
    }

    lv.iSubItem = 1;            //要获取的列
    lv.pszText = szPid;         //存储结果的缓冲区
    lv.cchTextMax = 0x20;       //指定缓冲区大小

    //获取PID，其中szPid表示选中进程的PID
    SendMessage(g_hwndListProcess, LVM_GETITEMTEXT, dwRowId, (DWORD)&lv);    

    DWORD dwPid = atoi(szPid);

    // 使用获取的PID遍历模块
    DWORD dwNumOfModules = 0;
    BOOL bResult = FALSE;
    HMODULE hModules[1024];
    HANDLE hProcess;

    bResult = GetAllModules(dwPid, hModules, 1024, &dwNumOfModules);
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
    if (bResult == FALSE || hProcess == NULL || dwNumOfModules == 0)
    {
        return;
    }

    // 填充所有行数据
    ListView_DeleteAllItems(g_hwndListModules);
    for (int row = 0; row < dwNumOfModules; row++)
    {
        // 获取模块数据: 模块名称,模块地址
        TCHAR strBaseName[1024] = { 0 };
        TCHAR strFileName[1024] = { 0 };

        GetModuleBaseName(hProcess, hModules[row], strBaseName, 1024);
        GetModuleFileNameEx(hProcess, hModules[row], strFileName, 1024);

        LV_ITEM vitem = { 0 };
        vitem.mask = LVIF_TEXT;
        vitem.iItem = row;

        // 第一列(模块名称)
        vitem.pszText = strBaseName;
        vitem.iSubItem = 0;
        ListView_InsertItem(g_hwndListModules, &vitem);

        // 第二列(模块位置)
        vitem.pszText = strFileName;
        vitem.iSubItem = 1;
        ListView_SetItem(g_hwndListModules, &vitem); 
    }
    CloseHandle(hProcess);
}

//利用GetModuleInformation获取进程模块信息：imagebase和imagesize，在EnProcList调用
BOOL GetMainModuleInfo(DWORD dwPid, MODULEINFO* mi)
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);    //pid获取当前进程句柄
    if (hProcess == NULL)
    {
        return FALSE;
    }

    HMODULE hModules[1];
    DWORD cbNeeded;
    if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded) == FALSE)
    {
        CloseHandle(hProcess);
        return FALSE;
    }

    MODULEINFO info;
    DWORD cb;
    if (GetModuleInformation(hProcess, hModules[0], (LPMODULEINFO)&info, (DWORD)(&cb)) == FALSE)
    {
        CloseHandle(hProcess);
        return FALSE;
    }

    *mi = info;
    CloseHandle(hProcess);
    return TRUE;
}

//获取当前进程所有模块信息，模块数目、模块指针列表
BOOL GetAllModules(DWORD dwPid, HMODULE* pModuel, int nSize, DWORD* dwNumOfModules)
{

    HANDLE hProcess;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);   //打开一个现有的进程对象，返回当前进程句柄
    if (hProcess == NULL)
    {
        return FALSE;
    }

    DWORD cbNeeded;
    if (EnumProcessModules(hProcess, pModuel, nSize, &cbNeeded) == FALSE) //EnumProcessModules 检索指定进程中每个模块的句柄
    {
        CloseHandle(hProcess);
        return FALSE;
    }
    *dwNumOfModules = cbNeeded / sizeof(DWORD);
    return TRUE;
}

//点击PE查看按钮时调用
void PECHECK(HWND hwndDlg)
{
    // 弹出文件选择器
    OPENFILENAME stOpenFile = {0};
    TCHAR szPeFileExt[100] = TEXT("*.exe;*.dll;*.sys;*.scr;*.drv");
    TCHAR szFileName[256];
    memset(szFileName, 0, 256);
    
    stOpenFile.lStructSize = sizeof(OPENFILENAME);
    stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;   //指定用户只能在“文件名”和“路径名称”输入字段中键入现有文件的名称
    stOpenFile.hwndOwner = hwndDlg;
    stOpenFile.lpstrFilter = szPeFileExt;
    stOpenFile.lpstrFile = szFileName;
    stOpenFile.nMaxFile = MAX_PATH;

    //函数创建一个“打开”对话框，该对话框允许用户指定驱动器，目录以及要打开的文件或文件集的名称
    GetOpenFileName(&stOpenFile);

    MessageBox(0, TEXT(stOpenFile.lpstrFile), TEXT("tips"), MB_OK);

    ReadPeFile(szFileName, &g_pFileBuffer, &g_dwFileSize);

    DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_PE), hwndMain, PEDialogProc);
}
