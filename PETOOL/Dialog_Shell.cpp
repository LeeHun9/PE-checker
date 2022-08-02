#include "Dialog_Shell.h"

//Shell����ȫ�ִ��ھ��
HWND g_hwndDialogShell;

void EncryptSrc(DWORD size, PBYTE p_src);

void SetEditTextShell(DWORD dwEditId, PCTSTR buf)
{
    HWND hEdit = GetDlgItem(g_hwndDialogShell, (DWORD)MAKEINTRESOURCE(dwEditId));
    SetWindowText(hEdit, buf);
}

//Shell���洦����
BOOL CALLBACK ShellDialogProc(HWND hwdShellDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = TRUE;

    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        g_hwndDialogShell = hwdShellDlg;
        TCHAR buf1[128] = TEXT("��ѡ��SHELL����");
        SetEditTextShell(IDC_EDIT_SHELL, buf1);
        TCHAR buf2[128] = TEXT("��ѡ����ӿ�SRC����");
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
            // �����ļ�ѡ����
            OPENFILENAME stOpenFile = { 0 };
            TCHAR szPeFileExt[100] = TEXT("*.exe;*.dll;*.sys;*.scr;*.drv");
            TCHAR szFileName[256];
            memset(szFileName, 0, 256);

            stOpenFile.lStructSize = sizeof(OPENFILENAME);
            stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;   //ָ���û�ֻ���ڡ��ļ������͡�·�����ơ������ֶ��м��������ļ�������
            stOpenFile.hwndOwner = hwdShellDlg;
            stOpenFile.lpstrFilter = szPeFileExt;
            stOpenFile.lpstrFile = szFileName;
            stOpenFile.nMaxFile = MAX_PATH;

            //��������һ�����򿪡��Ի��򣬸öԻ��������û�ָ����������Ŀ¼�Լ�Ҫ�򿪵��ļ����ļ���������
            GetOpenFileName(&stOpenFile);

            SetEditTextShell(IDC_EDIT_SHELL, stOpenFile.lpstrFile);

            break;
        }
        case IDC_BUTTON_SelectSrc:
        {
            // �����ļ�ѡ����
            OPENFILENAME stOpenFile = { 0 };
            TCHAR szPeFileExt[100] = TEXT("*.exe;*.dll;*.sys;*.scr;*.drv");
            TCHAR szFileName[256];
            memset(szFileName, 0, 256);

            stOpenFile.lStructSize = sizeof(OPENFILENAME);
            stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;   //ָ���û�ֻ���ڡ��ļ������͡�·�����ơ������ֶ��м��������ļ�������
            stOpenFile.hwndOwner = hwdShellDlg;
            stOpenFile.lpstrFilter = szPeFileExt;
            stOpenFile.lpstrFile = szFileName;
            stOpenFile.nMaxFile = MAX_PATH;

            //��������һ�����򿪡��Ի��򣬸öԻ��������û�ָ����������Ŀ¼�Լ�Ҫ�򿪵��ļ����ļ���������
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
    //��ȡ�ǳ����ԭ����·��
    TCHAR SzShellBuf[256];
    GetDlgItemText(hwdDlg, IDC_EDIT_SHELL, SzShellBuf, 256);
    TCHAR SzSrcBuf[256];
    GetDlgItemText(hwdDlg, IDC_EDIT_SRC, SzSrcBuf, 256);


    //����SRC�����ڴ棬�ò�����Ҫ��������
    PVOID p_SrcBuff;
    DWORD dwSrcSize;
    ReadPeFile(SzSrcBuf, &p_SrcBuff, &dwSrcSize);

    //����SRC����
    EncryptSrc(dwSrcSize, (PBYTE)p_SrcBuff);

    //����shell���򣬲�����½ڣ�p_NewShellBuffָ����ӽں���»�����
    PVOID p_ShellBuff;
    PVOID p_NewShellBuff;
    DWORD dwShellSize;
    ReadPeFile(SzShellBuf, &p_ShellBuff, &dwShellSize);
    if (AddCodeSection(p_ShellBuff, &p_NewShellBuff, dwShellSize, dwSrcSize) == 0)
    {
        MessageBox(NULL, TEXT("������ʧ�ܣ�������..."), TEXT("ERROR"), 0);
        return;
    }

    //�����ܵ�Src�ļ������¼ӽڴ�
    //�ȶ�λ�����Ľڱ�
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)p_NewShellBuff;
    PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + sizeof(IMAGE_FILE_HEADER));
    PIMAGE_SECTION_HEADER pSectionHeader = \
        (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
    DWORD NumberOfSections = pPEHeader->NumberOfSections;
    PIMAGE_SECTION_HEADER pLastSectionHeader = pSectionHeader + NumberOfSections - 1; 

    memcpy((PVOID)((DWORD)p_NewShellBuff + pLastSectionHeader->PointerToRawData), p_SrcBuff, dwSrcSize);
    DWORD SizeOfNewShell = dwShellSize + AlignedSize(dwSrcSize, pOptionHeader->FileAlignment);

    //д��Ӳ���ļ����ӿǳɹ�
    MemoryToFile((PVOID)p_NewShellBuff, SizeOfNewShell, SHELLPATH);
    MessageBox(NULL, TEXT("�ӿǳɹ�"), TEXT("��ʾ"), 0);
}

void EncryptSrc(DWORD size, PBYTE p_src)
{
    for (int i = 0; i < size; i++)
    {
        p_src[i] ^= 0;
    }
}
