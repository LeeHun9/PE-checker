#include "Dialog_Directory.h"
#include "Dialog_Detail.h"


HWND g_hwndDialogDirectory;
extern HINSTANCE hInst;                                // µ±Ç°ÊµÀý
extern DWORD g_dwShowID;

void SetDirEditText(DWORD dwEditId, PCTSTR buf)
{
    HWND hEdit = GetDlgItem(g_hwndDialogDirectory, (DWORD)MAKEINTRESOURCE(dwEditId));
    SetWindowText(hEdit, buf);
}


void InitDirectoryDialog()
{
    PIMAGE_OPTIONAL_HEADER pOptionalHeader = GetOptionalHeader(g_pFileBuffer);
    
    DWORD rvaIds[16] =
    {
        IDC_EDIT_OPRVA,
        IDC_EDIT_INPUTRVA,
        IDC_EDIT_SOURCERVA,
        IDC_EDIT_EXCEPTIONRVA,
        IDC_EDIT_SECURERVA,
        IDC_EDIT_RELOCRVA,
        IDC_EDIT_DBGRVA,
        IDC_EDIT_CRRVA,
        IDC_EDIT_GPRVA,
        IDC_EDIT_TLSRVA,
        IDC_EDIT_ICRVA,
        IDC_EDIT_BIRVA,
        IDC_EDIT_IATRVA,
        IDC_EDIT_DIRVA,
        IDC_EDIT_COMRVA,
        IDC_EDIT_RETAINRVA
    };

    DWORD sizeIds[16] =
    {
        IDC_EDIT_OPSIZE,
        IDC_EDIT_INPUTSIZE,
        IDC_EDIT_SOURCESIZE,
        IDC_EDIT_EXCEPTIONSIZE,
        IDC_EDIT_SECURESIZE,
        IDC_EDIT_RELOCSIZE,
        IDC_EDIT_DBGSIZE,
        IDC_EDIT_CRSIZE,
        IDC_EDIT_GPSIZE,
        IDC_EDIT_TLSSIZE,
        IDC_EDIT_ICSIZE,
        IDC_EDIT_BISIZE,
        IDC_EDIT_IATSIZE,
        IDC_EDIT_DISIZE,
        IDC_EDIT_COMSIZE,
        IDC_EDIT_RETAINSIZE
    };

    HWND hwndEditRva;
    HWND hwndEditSize;
    TCHAR buffer[16];

    for (int row = 0; row < 16; row++)
    {
        PIMAGE_DATA_DIRECTORY p_DataDirectory = GetDataDirectory(g_pFileBuffer, row);

        wsprintf(buffer, TEXT("%p"), p_DataDirectory->VirtualAddress);
        SetDirEditText(rvaIds[row], buffer);
        /*hwndEditRva = GetDlgItem(g_hwndDialogDirectory, rvaIds[row]);
        SetWindowText(hwndEditRva, buffer);*/

        wsprintf(buffer, TEXT("%p"), p_DataDirectory->Size);
        SetDirEditText(sizeIds[row], buffer);
        /*hwndEditSize = GetDlgItem(g_hwndDialogDirectory, sizeIds[row]);
        SetWindowText(hwndEditSize, buffer);*/
    }
}


BOOL CALLBACK DialogDirectoryProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = TRUE;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hwndDialogDirectory = hwndDlg;
        InitDirectoryDialog();
        break;
    case WM_CLOSE:
        EndDialog(hwndDlg, 0);
        break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_DIRCLOSE:
            EndDialog(hwndDlg, 0);
            break;

        case IDC_BUTTON_EXPDETAIL:
            g_dwShowID = IDC_BUTTON_EXPDETAIL;
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), g_hwndDialogDirectory, DialogDetailProc);
            break;

        case IDC_BUTTON_IMPDETAIL:
            g_dwShowID = IDC_BUTTON_IMPDETAIL;
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), g_hwndDialogDirectory, DialogDetailProc);
            break;

        case IDC_BUTTON_RESDETAIL:
            g_dwShowID = IDC_BUTTON_RESDETAIL;
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), g_hwndDialogDirectory, DialogDetailProc);
            break;

        case IDC_BUTTON_RELOCDETAIL:
            g_dwShowID = IDC_BUTTON_RELOCDETAIL;
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), g_hwndDialogDirectory, DialogDetailProc);
            break;

        case IDC_BUTTON_BINDIPDETAIL:
            g_dwShowID = IDC_BUTTON_BINDIPDETAIL;
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), g_hwndDialogDirectory, DialogDetailProc);
            break;

        case IDC_BUTTON_IATDETAIL:
            g_dwShowID = IDC_BUTTON_IATDETAIL;
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), g_hwndDialogDirectory, DialogDetailProc);
            break;

        default:
            result = FALSE;
            break;
        }

    }
    default:
        result = FALSE;
    }
    return result;
}