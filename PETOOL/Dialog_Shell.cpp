#include "Dialog_Shell.h"

//Shell界面全局窗口句柄
HWND g_hwndDialogShell;

void EncryptSrc(DWORD size, PBYTE p_src);

void SetEditTextShell(DWORD dwEditId, PCTSTR buf)
{
    HWND hEdit = GetDlgItem(g_hwndDialogShell, (DWORD)MAKEINTRESOURCE(dwEditId));
    SetWindowText(hEdit, buf);
}

//Shell界面处理函数
BOOL CALLBACK ShellDialogProc(HWND hwdShellDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = TRUE;

    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        g_hwndDialogShell = hwdShellDlg;
        TCHAR buf1[128] = TEXT("请选择SHELL程序");
        SetEditTextShell(IDC_EDIT_SHELL, buf1);
        TCHAR buf2[128] = TEXT("请选择待加壳SRC程序");
        SetEditTextShell(IDC_EDIT_SRC, buf2);
        break;
    }
    case WM_CLOSE:
        EndDialog(hwdShellDlg, 0);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_SelectShell:
        {
            // 弹出文件选择器
            OPENFILENAME stOpenFile = { 0 };
            TCHAR szPeFileExt[100] = TEXT("*.exe;*.dll;*.sys;*.scr;*.drv");
            TCHAR szFileName[256];
            memset(szFileName, 0, 256);

            stOpenFile.lStructSize = sizeof(OPENFILENAME);
            stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;   //指定用户只能在“文件名”和“路径名称”输入字段中键入现有文件的名称
            stOpenFile.hwndOwner = hwdShellDlg;
            stOpenFile.lpstrFilter = szPeFileExt;
            stOpenFile.lpstrFile = szFileName;
            stOpenFile.nMaxFile = MAX_PATH;

            //函数创建一个“打开”对话框，该对话框允许用户指定驱动器，目录以及要打开的文件或文件集的名称
            GetOpenFileName(&stOpenFile);

            SetEditTextShell(IDC_EDIT_SHELL, stOpenFile.lpstrFile);

            break;
        }
        case IDC_BUTTON_SelectSrc:
        {
            // 弹出文件选择器
            OPENFILENAME stOpenFile = { 0 };
            TCHAR szPeFileExt[100] = TEXT("*.exe;*.dll;*.sys;*.scr;*.drv");
            TCHAR szFileName[256];
            memset(szFileName, 0, 256);

            stOpenFile.lStructSize = sizeof(OPENFILENAME);
            stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;   //指定用户只能在“文件名”和“路径名称”输入字段中键入现有文件的名称
            stOpenFile.hwndOwner = hwdShellDlg;
            stOpenFile.lpstrFilter = szPeFileExt;
            stOpenFile.lpstrFile = szFileName;
            stOpenFile.nMaxFile = MAX_PATH;

            //函数创建一个“打开”对话框，该对话框允许用户指定驱动器，目录以及要打开的文件或文件集的名称
            GetOpenFileName(&stOpenFile);
            SetEditTextShell(IDC_EDIT_SRC, stOpenFile.lpstrFile);

            break;
        }
        case IDC_BUTTON_SHELL:
            //DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_SECTIONS), g_hwndDialogPe, DialogSectionsProc);
            StartShell(hwdShellDlg);
            break;
        default:
            result = FALSE;
        }
    default:
        result = FALSE;
    }
    return result;
}



void StartShell(HWND hwdDlg)
{
    //获取壳程序和原程序路径
    TCHAR SzShellBuf[256];
    GetDlgItemText(hwdDlg, IDC_EDIT_SHELL, SzShellBuf, 256);
    TCHAR SzSrcBuf[256];
    GetDlgItemText(hwdDlg, IDC_EDIT_SRC, SzSrcBuf, 256);


    //读入SRC程序到内存，该步不需要进行拉伸
    PVOID p_SrcBuff;
    DWORD dwSrcSize;
    ReadPeFile(SzSrcBuf, &p_SrcBuff, &dwSrcSize);

    //加密SRC程序
    EncryptSrc(dwSrcSize, (PBYTE)p_SrcBuff);

    //读入shell程序，并添加新节，p_NewShellBuff指向添加节后的新缓冲区
    PVOID p_ShellBuff;
    PVOID p_NewShellBuff;
    DWORD dwShellSize;
    ReadPeFile(SzShellBuf, &p_ShellBuff, &dwShellSize);
    if (AddCodeSection(p_ShellBuff, &p_NewShellBuff, dwShellSize, dwSrcSize) == 0)
    {
        MessageBox(NULL, TEXT("新增节失败，请重试..."), TEXT("ERROR"), 0);
        return;
    }

    //将加密的Src文件贴到新加节处
    //先定位新增的节表
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)p_NewShellBuff;
    PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + sizeof(IMAGE_FILE_HEADER));
    PIMAGE_SECTION_HEADER pSectionHeader = \
        (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
    DWORD NumberOfSections = pPEHeader->NumberOfSections;
    PIMAGE_SECTION_HEADER pLastSectionHeader = pSectionHeader + NumberOfSections - 1; 

    memcpy((PVOID)((DWORD)p_NewShellBuff + pLastSectionHeader->PointerToRawData), p_SrcBuff, dwSrcSize);
    DWORD SizeOfNewShell = dwShellSize + AlignedSize(dwSrcSize, pOptionHeader->FileAlignment);

    //写入硬盘文件，加壳成功
    MemoryToFile((PVOID)p_NewShellBuff, SizeOfNewShell, SHELLPATH);
    MessageBox(NULL, TEXT("加壳成功"), TEXT("提示"), 0);
}

void EncryptSrc(DWORD size, PBYTE p_src)
{
    for (int i = 0; i < size; i++)
    {
        p_src[i] ^= 0;
    }
}
