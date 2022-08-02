#include "Dialog_Section.h"

HWND g_hwndDialogSections;

void InitSectionsDialog()
{
    //初始化List控件
    LV_COLUMN lv = { 0 };
    HWND hwndList;

    hwndList = GetDlgItem(g_hwndDialogSections, IDC_LIST_SECTIONS);

    //设置整行选中	
    SendMessage(hwndList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCFMT_CENTER;

    lv.pszText = (LPSTR)TEXT("节名");
    lv.cx = 60;
    lv.iSubItem = 0;
    ListView_InsertColumn(hwndList, 0, &lv);

    lv.pszText = (LPSTR)TEXT("文件偏移");
    lv.cx = 80;
    lv.iSubItem = 1;
    ListView_InsertColumn(hwndList, 1, &lv);

    lv.pszText = (LPSTR)TEXT("文件大小");
    lv.cx = 80;
    lv.iSubItem = 2;
    ListView_InsertColumn(hwndList, 2, &lv);

    lv.pszText = (LPSTR)TEXT("内存偏移");
    lv.cx = 80;
    lv.iSubItem = 3;
    ListView_InsertColumn(hwndList, 3, &lv);

    lv.pszText = (LPSTR)TEXT("内存大小");
    lv.cx = 80;
    lv.iSubItem = 4;
    ListView_InsertColumn(hwndList, 4, &lv);

    lv.pszText = (LPSTR)TEXT("节区属性");
    lv.cx = 80;
    lv.iSubItem = 5;
    ListView_InsertColumn(hwndList, 5, &lv);

    //填充list控件
    for (int row = 0; row < GetSectionNumbers(g_pFileBuffer); row++)
    {
        PIMAGE_SECTION_HEADER pSectionHeader = GetSectionHeader(g_pFileBuffer, row);
        TCHAR tBuffer[16];

        LV_ITEM vitem = { 0 };
        vitem.mask = LVIF_TEXT | LVCFMT_LEFT;
        vitem.iItem = row;

        //区块名
        char cBuffer[16] = { 0 };
        wsprintfA(cBuffer, ("%8s"), pSectionHeader->Name); 

        if (sizeof(TCHAR) == 2)
        {
            ZeroMemory(tBuffer, sizeof(TCHAR) * 16);
            MultiByteToWideChar(CP_ACP, 0, cBuffer, -1, (LPWSTR)tBuffer, IMAGE_SIZEOF_SHORT_NAME);
            vitem.pszText = tBuffer;
        }
        else
        {
            vitem.pszText = cBuffer;
        }
        vitem.iSubItem = 0;
        ListView_InsertItem(hwndList, &vitem);

        //文件偏移
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->PointerToRawData);    
        vitem.pszText = tBuffer;
        vitem.iSubItem = 1;
        ListView_SetItem(hwndList, &vitem);


        //文件大小
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->SizeOfRawData);
        vitem.pszText = tBuffer;
        vitem.iSubItem = 2;
        ListView_SetItem(hwndList, &vitem);

        //内存偏移
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->VirtualAddress);
        vitem.pszText = tBuffer;
        vitem.iSubItem = 3;
        ListView_SetItem(hwndList, &vitem);

        // 内存大小
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->Misc.VirtualSize);
        vitem.pszText = tBuffer;
        vitem.iSubItem = 4;
        ListView_SetItem(hwndList, &vitem);

        // 节区属性
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->Characteristics);
        vitem.pszText = tBuffer;
        vitem.iSubItem = 5;
        ListView_SetItem(hwndList, &vitem);

    }
}

BOOL CALLBACK DialogSectionsProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = TRUE;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hwndDialogSections = hwndDlg;
        InitSectionsDialog();
        break;
    case WM_CLOSE:
        EndDialog(hwndDlg, 0);
        break;
    case WM_COMMAND:
    {
        switch (wParam)
        {

        default:
            break;
        }
    }
    default:
        result = FALSE;
    }
    return result;
}
