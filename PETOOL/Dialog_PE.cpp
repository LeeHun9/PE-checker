#include "Dialog_PE.h"
#include "Dialog_Main.h"
#include "Dialog_Section.h"
#include "Dialog_Directory.h"

HWND g_hwndDialogPe;
extern HINSTANCE hInst;
void SetEditText(DWORD dwEditId, PCTSTR buf)
{
    HWND hEdit = GetDlgItem(g_hwndDialogPe, (DWORD)MAKEINTRESOURCE(dwEditId));
    SetWindowText(hEdit, buf);
}

void InitPeDialog()
{

    PIMAGE_FILE_HEADER pFileHeader = GetFileHeader(g_pFileBuffer);

    if (pFileHeader->SizeOfOptionalHeader == 0x00E0)
    {
        PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = GetOptionalHeader(g_pFileBuffer);

        TCHAR buf[128];
        wsprintf(buf, TEXT("%p"), pOptionalHeader->AddressOfEntryPoint);
        SetEditText(IDC_EDIT_EP, buf);

        wsprintf(buf, TEXT("%p"), pOptionalHeader->ImageBase);
        SetEditText(IDC_EDIT_IB, buf);

        wsprintf(buf, TEXT("%p"), pOptionalHeader->SizeOfImage);
        SetEditText(IDC_EDIT_IS, buf);

        wsprintf(buf, TEXT("%p"), pOptionalHeader->BaseOfCode);
        SetEditText(IDC_EDIT_CB, buf);

        wsprintf(buf, TEXT("%p"), pOptionalHeader->BaseOfData);
        SetEditText(IDC_EDIT_DB, buf);

        wsprintf(buf, TEXT("%p"), pOptionalHeader->SectionAlignment);
        SetEditText(IDC_EDIT_IA, buf);

        wsprintf(buf, TEXT("%p"), pOptionalHeader->FileAlignment);
        SetEditText(IDC_EDIT_FA, buf);

        wsprintf(buf, TEXT("%p"), pOptionalHeader->Magic);
        SetEditText(IDC_EDIT_FALGS, buf);

        wsprintf(buf, TEXT("%p"), pOptionalHeader->Subsystem);
        SetEditText(IDC_EDIT_SubS, buf);

        wsprintf(buf, TEXT("%p"), pFileHeader->NumberOfSections);
        SetEditText(IDC_EDIT_SecNum, buf);

        wsprintf(buf, TEXT("%p"), pFileHeader->TimeDateStamp);
        SetEditText(IDC_EDIT_TimeS, buf);

        wsprintf(buf, TEXT("%p"), pOptionalHeader->SizeOfHeaders);
        SetEditText(IDC_EDIT_PEHead, buf);

        wsprintf(buf, TEXT("%p"), pFileHeader->Characteristics);
        SetEditText(IDC_EDIT_FEATURE, buf);

        wsprintf(buf, TEXT("%p"), pOptionalHeader->CheckSum);
        SetEditText(IDC_EDIT_ChkSum, buf);

        wsprintf(buf, TEXT("%p"), pFileHeader->SizeOfOptionalHeader);
        SetEditText(IDC_EDIT_PEOptHead, buf);

        wsprintf(buf, TEXT("%p"), pOptionalHeader->NumberOfRvaAndSizes);
        SetEditText(IDC_EDIT_DirNum, buf);
    }
    else if (pFileHeader->SizeOfOptionalHeader == 0x00f0)
    {
        PIMAGE_OPTIONAL_HEADER64 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER64)GetOptionalHeader(g_pFileBuffer);

        TCHAR buf[128];
        wsprintf(buf, TEXT("%p"), pOptionalHeader->AddressOfEntryPoint);
        SetEditText(IDC_EDIT_EP, buf);
        wsprintf(buf, TEXT("%I64X\0"), pOptionalHeader->ImageBase);
        SetEditText(IDC_EDIT_IB, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->SizeOfImage);
        SetEditText(IDC_EDIT_IS, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->BaseOfCode);
        SetEditText(IDC_EDIT_CB, buf);
        wsprintf(buf, TEXT("%p"), 0);
        SetEditText(IDC_EDIT_DB, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->SectionAlignment);
        SetEditText(IDC_EDIT_IA, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->FileAlignment);
        SetEditText(IDC_EDIT_FA, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->Magic);
        SetEditText(IDC_EDIT_FALGS, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->Subsystem);
        SetEditText(IDC_EDIT_SubS, buf);
        wsprintf(buf, TEXT("%p"), pFileHeader->NumberOfSections);
        SetEditText(IDC_EDIT_SecNum, buf);
        wsprintf(buf, TEXT("%p"), pFileHeader->TimeDateStamp);
        SetEditText(IDC_EDIT_TimeS, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->SizeOfHeaders);
        SetEditText(IDC_EDIT_PEHead, buf);
        wsprintf(buf, TEXT("%p"), pFileHeader->Characteristics);
        SetEditText(IDC_EDIT_FEATURE, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->CheckSum);
        SetEditText(IDC_EDIT_ChkSum, buf);
        wsprintf(buf, TEXT("%p"), pFileHeader->SizeOfOptionalHeader);
        SetEditText(IDC_EDIT_PEOptHead, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->NumberOfRvaAndSizes);
        SetEditText(IDC_EDIT_DirNum, buf);
    }

}

BOOL CALLBACK PEDialogProc(HWND hwndPeDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = TRUE;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hwndDialogPe = hwndPeDlg;
        InitPeDialog();
        break;
    case WM_CLOSE:
        EndDialog(hwndPeDlg, 0);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_CLOSE:
            EndDialog(hwndPeDlg, 0);
            break;
        case IDC_BUTTON_catalog:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY), g_hwndDialogPe, DialogDirectoryProc);
            break;
        case IDC_BUTTON_Section:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_SECTIONS), g_hwndDialogPe, DialogSectionsProc);
            break;
        default:
            result = FALSE;
        }
    default:
        result = FALSE;
    }
    return result;
}
