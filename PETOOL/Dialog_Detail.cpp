#include "Dialog_Detail.h"

HWND g_hwndDialogDetail;
DWORD g_dwShowID;

TCHAR* resource_types[] = {
    (PTCHAR)TEXT("0"),            // 00h
    (PTCHAR)TEXT("CURSOR"),       // 01h
    (PTCHAR)TEXT("BITMAP"),       // 02h
    (PTCHAR)TEXT("ICON"),         // 03h
    (PTCHAR)TEXT("MENU"),         // 04h
    (PTCHAR)TEXT("DIALOG"),       // 05h
    (PTCHAR)TEXT("STRING"),       // 06h
    (PTCHAR)TEXT("FONTDIR"),      // 07h
    (PTCHAR)TEXT("FONT"),         // 08h
    (PTCHAR)TEXT("ACCELERATOR"),  // 09h
    (PTCHAR)TEXT("RCDATA"),       // 0Ah
    (PTCHAR)TEXT("MESSAGETABLE"), // 0Bh
    (PTCHAR)TEXT("GROUP_CURSOR"), // 0Ch
    (PTCHAR)TEXT(""),             // 0Dh
    (PTCHAR)TEXT("GROUP_ICON"),   // 0Eh
    (PTCHAR)TEXT(""),             // 0Fh
    (PTCHAR)TEXT("VERSION INFOMATION")       // 10h
};

VOID  PrintfResrouceDirectoryRe(DWORD dw_Base, PIMAGE_RESOURCE_DIRECTORY p_ResourceDirectory, DWORD dw_Depth, PTSTR str)
{
    DWORD dw_NumberEntries = p_ResourceDirectory->NumberOfIdEntries + p_ResourceDirectory->NumberOfNamedEntries;
    for (DWORD i = 0; i < dw_NumberEntries; i++)
    {
        if (dw_Depth == 0)
        {
            wsprintf(str += _tcslen(str), TEXT("\r\n|-"));
        }
        if (dw_Depth == 1)
        {
            wsprintf(str += _tcslen(str), TEXT("\r\n|----"));
        }

        PIMAGE_RESOURCE_DIRECTORY_ENTRY p_ResourceDirEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)p_ResourceDirectory + sizeof(IMAGE_RESOURCE_DIRECTORY) + i * sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY));
        if ((p_ResourceDirEntry->Name & 0x80000000) == 0x80000000)
        {
            DWORD dw_Offset = p_ResourceDirEntry->Name & 0x7FFFFFFF;
            PIMAGE_RESOURCE_DIR_STRING_U p_DirString = (PIMAGE_RESOURCE_DIR_STRING_U)(dw_Base + dw_Offset);
            DWORD length = p_DirString->Length;
            const wchar_t* str = p_DirString->NameString;
            wsprintf((LPSTR)(str += _tcslen((LPSTR)str)), TEXT("%-10.*s"), length, str);
        }
        else
        {
            if (dw_Depth > 1)
            {
                wsprintf(str += _tcslen(str), TEXT("ID:%x  "), p_ResourceDirEntry->Name);
            }
            else
            {
                if (dw_Depth == 0 && p_ResourceDirEntry->Name < sizeof(resource_types) / sizeof(CHAR*))
                {
                    wsprintf(str += _tcslen(str), TEXT("%-10s"), resource_types[p_ResourceDirEntry->Name]);
                }
                else
                {
                    wsprintf(str += _tcslen(str), TEXT("%-10x"), p_ResourceDirEntry->Name);
                }
            }
        }

        if ((p_ResourceDirEntry->OffsetToData & 0x80000000) == 0x80000000)
        {
            DWORD offset = p_ResourceDirEntry->OffsetToData & 0x7FFFFFFF;
            PIMAGE_RESOURCE_DIRECTORY p_NextResourceDir = (PIMAGE_RESOURCE_DIRECTORY)(dw_Base + offset);

            PrintfResrouceDirectoryRe(dw_Base, p_NextResourceDir, dw_Depth + 1, str);
        }
        else
        {
            DWORD dw_Offset = p_ResourceDirEntry->OffsetToData;
            PIMAGE_RESOURCE_DATA_ENTRY p_DataEntry = (PIMAGE_RESOURCE_DATA_ENTRY)(dw_Base + dw_Offset);
            wsprintf(str += _tcslen(str), TEXT("RVA:%x  Size:%x"), p_DataEntry->OffsetToData, p_DataEntry->Size);
        }
    }
}


void InitEXPDETAIL()
{
    PIMAGE_EXPORT_DIRECTORY p_ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)GetExportDirectory(g_pFileBuffer);
    if (p_ExportDirectory == NULL)
    {
        return;
    }

    LPVOID buffer = malloc(1024 * 1024);
    memset(buffer, 0, 1024 * 1024);
    PTSTR str = (PTSTR)buffer;

    wsprintf(str += _tcslen(str), TEXT("============================= EXPORT_DIRECTORY ============================= \r\n"));

    //写入信息
    wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("Characteristics"), p_ExportDirectory->Characteristics);
    wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("TimeDateStamp"), p_ExportDirectory->TimeDateStamp);
    wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("MajorVersion"), p_ExportDirectory->MajorVersion);
    wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("MinorVersion"), p_ExportDirectory->MinorVersion);
    wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("Name"), p_ExportDirectory->Name);
    wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("Base"), p_ExportDirectory->Base);
    wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("NumberOfFunctions"), p_ExportDirectory->NumberOfFunctions);
    wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("NumberOfNames"), p_ExportDirectory->NumberOfNames);
    wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("AddressOfFunctions"), p_ExportDirectory->AddressOfFunctions);
    wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("AddressOfNames"), p_ExportDirectory->AddressOfNames);
    wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("AddressOfNameOrdinals"), p_ExportDirectory->AddressOfNameOrdinals);

    DWORD dw_NumberOfFunction = p_ExportDirectory->NumberOfFunctions;
    DWORD dw_NumberOfNames = p_ExportDirectory->NumberOfNames;
    DWORD dw_Base = p_ExportDirectory->Base;
    PWORD p_OrdinalTable = (PWORD)((DWORD)g_pFileBuffer + Rva2Foa(g_pFileBuffer, p_ExportDirectory->AddressOfNameOrdinals));
    PDWORD p_NameTable = (PDWORD)((DWORD)g_pFileBuffer + Rva2Foa(g_pFileBuffer, p_ExportDirectory->AddressOfNames));
    PDWORD p_FunctionTable = (PDWORD)((DWORD)g_pFileBuffer + Rva2Foa(g_pFileBuffer, p_ExportDirectory->AddressOfFunctions));

    wsprintf(str += _tcslen(str), TEXT("%-8s%-10s%-15s%-10s\r\n"), TEXT(""), TEXT("Ordinal"), TEXT("Address(RVA)"), TEXT("Name"));
    for (DWORD i = 0; i < dw_NumberOfFunction; i++)
    {
        PCHAR p_Name = (PCHAR)TEXT("-");
        DWORD dw_Ordinal = i + dw_Base;
        DWORD dw_Address = *(p_FunctionTable + i);

        for (DWORD j = 0; j < dw_NumberOfNames; j++)
        {
            if (*(p_OrdinalTable + j) == i)
            {
                DWORD dw_NameRva = *(p_NameTable + j);
                DWORD dw_NameFoa = Rva2Foa(g_pFileBuffer, dw_NameRva);

                p_Name = (PCHAR)((DWORD)g_pFileBuffer + dw_NameFoa);
                break;
            }
        }

        if (sizeof(TCHAR) == sizeof(wchar_t))
        {
            wchar_t wBuffer[32] = { 0 };
            MultiByteToWideChar(CP_ACP, 0, p_Name, -1, wBuffer, 32);
            p_Name = (PCHAR)wBuffer;
        }

        wsprintf(str += _tcslen(str), TEXT("%-8s"), TEXT(""));
        wsprintf(str += _tcslen(str), TEXT("%-10x%-15x%-10s"), dw_Ordinal, dw_Address, p_Name);

        wsprintf(str += _tcslen(str), TEXT("\r\n"));
    }

    wsprintf(str += _tcslen(str), TEXT("\r\n"));

    HWND hwndEdit = GetDlgItem(g_hwndDialogDetail, IDC_EDIT_Detail);
    SetWindowText(hwndEdit, (LPCSTR)buffer);

    free(buffer);
}

void InitIMPDETAIL()
{
    wchar_t wBuffer[128] = { 0 };
    PVOID buffer = malloc(1024 * 1024 * 1);
    memset(buffer, 0, 1024 * 1024 * 1);
    PTSTR str = (PTSTR)buffer;

    DWORD  numberOfDescriptors = GetImportDescriptorNumbers(g_pFileBuffer);

    for (DWORD importDescriptorIndex = 0; importDescriptorIndex < numberOfDescriptors; importDescriptorIndex++)
    {
        PIMAGE_IMPORT_DESCRIPTOR p_ImportDescriptor = GetImportDescriptor(g_pFileBuffer, importDescriptorIndex);

        DWORD nameRva = p_ImportDescriptor->Name;
        DWORD nameFoa = Rva2Foa(g_pFileBuffer, nameRva);
        PCHAR p_DllName = (PCHAR)((DWORD)g_pFileBuffer + nameFoa);

        wsprintf(str += _tcslen(str), TEXT("============================= IMPORT_DESCRIPTOR[%d] ============================= \r\n"), importDescriptorIndex);
        wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("OriginalFirstThunk"), p_ImportDescriptor->OriginalFirstThunk);
        wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("TimeDateStamp"), p_ImportDescriptor->TimeDateStamp);
        wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("ForwarderChain"), p_ImportDescriptor->ForwarderChain);
#ifdef _UNICODE
        MultiByteToWideChar(CP_ACP, 0, p_DllName, -1, wBuffer, 128);
        wsprintf(str += _tcslen(str), TEXT("%-30s%-16x%-s\r\n"), TEXT("Name"), p_ImportDescriptor->Name, wBuffer);
#else
        wsprintf(str += _tcslen(str), TEXT("%-30s%-16x%-s\r\n"), TEXT("Name"), p_ImportDescriptor->Name, p_DllName);
#endif // _UNICODE
        wsprintf(str += _tcslen(str), TEXT("%-30s%-30x\r\n"), TEXT("FirstThunk"), p_ImportDescriptor->FirstThunk);

        DWORD originalFirstThunk = p_ImportDescriptor->OriginalFirstThunk;
        DWORD intFoa = Rva2Foa(g_pFileBuffer, originalFirstThunk);
        PIMAGE_THUNK_DATA p_Int = (PIMAGE_THUNK_DATA)((DWORD)g_pFileBuffer + intFoa);

        wsprintf(str += _tcslen(str), TEXT("%-30s%"), TEXT("INT(Hint/Name)"));
        for (DWORD i = 0; p_Int->u1.AddressOfData != 0; p_Int++, i++)  
        {
            if (i > 0 && i % 3 == 0)
            {
                wsprintf(str += _tcslen(str), TEXT("\r\n"));
                wsprintf(str += _tcslen(str), TEXT("%30s"), TEXT(" "));
            }

            if (p_Int->u1.AddressOfData >> 31 == 0)
            {
                DWORD importHintNameRva = p_Int->u1.AddressOfData;
                DWORD importHintNameFoa = Rva2Foa(g_pFileBuffer, importHintNameRva);
                PIMAGE_IMPORT_BY_NAME p_ImportHintName = (PIMAGE_IMPORT_BY_NAME)((DWORD)g_pFileBuffer + importHintNameFoa);

#ifdef _UNICODE
                MultiByteToWideChar(CP_ACP, 0, p_ImportHintName->Name, -1, wBuffer, 128);
                wsprintf(str += _tcslen(str), TEXT("%x/%-16s\t"), p_ImportHintName->Hint, wBuffer);
#else
                wsprintf(str += _tcslen(str), TEXT("%x/%-16s\t"), p_ImportHintName->Hint, p_ImportHintName->Name);
#endif

            }
            else
            {
                DWORD dw_Ordinal = p_Int->u1.Ordinal & 0x7FFFFFFF;
                wsprintf(str += _tcslen(str), TEXT("(Orinal)%-16x\t"), dw_Ordinal);
            }

        }
        wsprintf(str += _tcslen(str), TEXT("\r\n"));
        wsprintf(str += _tcslen(str), TEXT("\r\n"));
        DWORD firstThunk = p_ImportDescriptor->FirstThunk;
        DWORD iatFoa = Rva2Foa(g_pFileBuffer, firstThunk);
        PIMAGE_THUNK_DATA p_Iat = (PIMAGE_THUNK_DATA)((DWORD)g_pFileBuffer + iatFoa);


        wsprintf(str += _tcslen(str), TEXT("%-30s%"), TEXT("IAT"));
        for (DWORD i = 0; p_Iat->u1.AddressOfData != 0; p_Iat++, i++)
        {
            if (i > 0 && i % 5 == 0)
            {
                wsprintf(str += _tcslen(str), TEXT("\r\n"));
                wsprintf(str += _tcslen(str), TEXT("%30s"), TEXT(" "));
            }
            wsprintf(str += _tcslen(str), TEXT("%-12x"), p_Iat->u1.AddressOfData);
        }
        wsprintf(str += _tcslen(str), TEXT("\r\n"));
        wsprintf(str += _tcslen(str), TEXT("\r\n"));
    }

    wsprintf(str += _tcslen(str), TEXT("\r\n"));


    HWND hwndEdit = GetDlgItem(g_hwndDialogDetail, IDC_EDIT_Detail);
    SetWindowText(hwndEdit, (LPSTR)buffer);
    free(buffer);

}

void InitRESDETAIL()
{
    wchar_t wBuffer[128] = { 0 };
    PVOID buffer = malloc(1024 * 1024 * 1);
    memset(buffer, 0, 1024 * 1024 * 1);
    PTSTR str = (PTSTR)buffer;

    wsprintf(str += _tcslen(str), TEXT("============================= RESOURCE_DIRECTORY ============================= \r\n"));

    PIMAGE_RESOURCE_DIRECTORY p_ResourceDirectory = GetResourceDirectory(g_pFileBuffer);
    if (p_ResourceDirectory == NULL)
    {
        wsprintf(str += _tcslen(str), TEXT("没有资源表!\r\n"));
        return;
    }

    DWORD dw_ResourceBase = (DWORD)p_ResourceDirectory;

    PrintfResrouceDirectoryRe(dw_ResourceBase, p_ResourceDirectory, 0, str);
    wsprintf(str += _tcslen(str), TEXT("\r\n"));
    wsprintf(str += _tcslen(str), TEXT("\r\n"));

    HWND hwndEdit = GetDlgItem(g_hwndDialogDetail, IDC_EDIT_Detail);
    SetWindowText(hwndEdit, (LPSTR)buffer);
    free(buffer);
}

void InitRELOCDETAIL()
{
    wchar_t wBuffer[128] = { 0 };
    PVOID buffer = malloc(1024 * 1024 * 1);
    memset(buffer, 0, 1024 * 1024 * 1);
    PTSTR str = (PTSTR)buffer;

    PIMAGE_BASE_RELOCATION p_Relocation = GetRelocation(g_pFileBuffer);
    if (p_Relocation == NULL)
    {
        wsprintf(str += _tcslen(str), TEXT("没有重定位表\r\n"));
        return;
    }

    wsprintf(str += _tcslen(str), TEXT("============================= RELOCATION ============================= \r\n"));
    wsprintf(str += _tcslen(str), TEXT("%-8s%-15s%-15s\r\n"), TEXT("NO"), TEXT("VirtualAddress"), TEXT("SizeOfBlock"));

    for (DWORD i = 1;; i++)
    {
        if (*(PDWORD)p_Relocation == 0)
        {
            break;
        }

        wsprintf(str += _tcslen(str), TEXT("%-8d%-15x%-15x\r\n"), i, p_Relocation->VirtualAddress, p_Relocation->SizeOfBlock);

        DWORD dw_Items = (p_Relocation->SizeOfBlock - 8) / sizeof(WORD);
        for (DWORD i = 0; i < dw_Items; i++)
        {
            PWORD pw_TypeOffset = (PWORD)((DWORD)p_Relocation + 8 + i * sizeof(WORD));
            WORD w_Type = *pw_TypeOffset >> 12;
            WORD w_Offset = *pw_TypeOffset & 0x0FFF;
            wsprintf(str += _tcslen(str), TEXT("%8s"), TEXT(""));
            wsprintf(str += _tcslen(str), TEXT("(%x)%x"), w_Type, w_Offset);
            if (i != 0 && i % 6 == 0)
            {
                wsprintf(str += _tcslen(str), TEXT("\r\n"));
            }
        }

        wsprintf(str += _tcslen(str), TEXT("\r\n"));

        p_Relocation = (PIMAGE_BASE_RELOCATION)((DWORD)p_Relocation + p_Relocation->SizeOfBlock);
    };



    HWND hwndEdit = GetDlgItem(g_hwndDialogDetail, IDC_EDIT_Detail);
    SetWindowText(hwndEdit, (LPSTR)buffer);
    free(buffer);
}

void InitBINDIPDETAIL()
{
    wchar_t wBuffer[128] = { 0 };
    PVOID buffer = malloc(1024 * 1024 * 1);
    memset(buffer, 0, 1024 * 1024 * 1);
    PTSTR str = (PTSTR)buffer;

    PIMAGE_BOUND_IMPORT_DESCRIPTOR p_BoundImport = GetBoundImportDescriptor(g_pFileBuffer);
    if (p_BoundImport == NULL)
    {
        wsprintf(str += _tcslen(str), TEXT("没有绑定导入表!\r\n"));
        return;
    }

    DWORD bountImportBase = (DWORD)p_BoundImport;

    IMAGE_BOUND_IMPORT_DESCRIPTOR nullBoundImport = { 0 };

    wsprintf(str += _tcslen(str), TEXT("============================= BOUND_IMPORT ============================= \r\n"));
    wsprintf(str += _tcslen(str), TEXT("%-15s%-10s%-30s\r\n"), TEXT("TimeDateStamp"), TEXT("Number"), TEXT("OffsetModuleName"));


    while (memcmp(p_BoundImport, &nullBoundImport, sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR)) != 0)
    {
        PCHAR name = (PCHAR)(+bountImportBase + p_BoundImport->OffsetModuleName);

        wsprintf(str += _tcslen(str), TEXT("%-15x"), p_BoundImport->TimeDateStamp);
        wsprintf(str += _tcslen(str), TEXT("%-10x"), p_BoundImport->NumberOfModuleForwarderRefs);

#ifdef _UNICODE
        MultiByteToWideChar(CP_ACP, 0, name, -1, wBuffer, 128);
        wsprintf(str += _tcslen(str), TEXT("%x->%s"), p_BoundImport->OffsetModuleName, wBuffer);
#else
        wsprintf(str += _tcslen(str), TEXT("%x->%s"), p_BoundImport->OffsetModuleName, name);
#endif // _UNICODE


        PIMAGE_BOUND_FORWARDER_REF p_BoundRef = (PIMAGE_BOUND_FORWARDER_REF)(p_BoundImport + 1);
        for (DWORD i = 0; i < p_BoundImport->NumberOfModuleForwarderRefs; i++)
        {
            name = (PCHAR)(+bountImportBase + p_BoundRef->OffsetModuleName);
            wsprintf(str += _tcslen(str), TEXT("\r\n"));
            wsprintf(str += _tcslen(str), TEXT("%8s"), TEXT(""));
#ifdef _UNICODE
            MultiByteToWideChar(CP_ACP, 0, name, -1, wBuffer, 128);
            wsprintf(str += _tcslen(str), TEXT("TimeDateStamp:%x\tName:%x->%s\tReserved:%x"), p_BoundRef->TimeDateStamp, p_BoundRef->OffsetModuleName, wBuffer, p_BoundRef->Reserved);
#else
            wsprintf(str += _tcslen(str), TEXT("TimeDateStamp:%x\tName:%x->%s\tReserved:%x"), p_BoundRef->TimeDateStamp, p_BoundRef->OffsetModuleName, name, p_BoundRef->Reserved);
#endif // _UNICODE
            p_BoundRef++;
        }

        p_BoundImport += p_BoundImport->NumberOfModuleForwarderRefs;
        p_BoundImport++;
        wsprintf(str += _tcslen(str), TEXT("\r\n"));
    }

    HWND hwndEdit = GetDlgItem(g_hwndDialogDetail, IDC_EDIT_Detail);
    SetWindowText(hwndEdit, (LPSTR)buffer);
    free(buffer);
}

void InitIATDETAIL()
{
    wchar_t wBuffer[128] = { 0 };
    PVOID buffer = malloc(1024 * 1024 * 1);
    memset(buffer, 0, 1024 * 1024 * 1);
    PTSTR str = (PTSTR)buffer;

    PIMAGE_DATA_DIRECTORY pDataDirectory = GetDataDirectory(g_pFileBuffer, IMAGE_DIRECTORY_ENTRY_IAT);
    if (pDataDirectory->VirtualAddress == 0 || pDataDirectory->Size == 0)
    {
        wsprintf(str += _tcslen(str), TEXT("没有导入表\r\n"));
        return;
    }

    DWORD iatRva = pDataDirectory->VirtualAddress;
    DWORD iatFoa = Rva2Foa(g_pFileBuffer, iatRva);
    PDWORD pIat = (PDWORD)((DWORD)g_pFileBuffer + iatFoa);

    DWORD length = (pDataDirectory->Size) / sizeof(DWORD);
    for (size_t index = 0; index < length; index++)
    {
        wsprintf(str += _tcslen(str), TEXT("[%04d] %08x\r\n"), index, pIat[index]);
    }

    HWND hwndEdit = GetDlgItem(g_hwndDialogDetail, IDC_EDIT_Detail);
    SetWindowText(hwndEdit, (LPSTR)buffer);
    free(buffer);
}

BOOL CALLBACK DialogDetailProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = TRUE;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hwndDialogDetail = hwndDlg;
        SendMessage(hwndDlg, g_dwShowID, NULL, NULL);   //向当前窗口发送一条消息，Msg 为 Dialog_Dirg 传入的 dwShowID
        break;
    case WM_CLOSE:
        EndDialog(hwndDlg, 0);
        break;
    //导出表
    case IDC_BUTTON_EXPDETAIL:
        InitEXPDETAIL();
        break;
    
    //导入表
    case IDC_BUTTON_IMPDETAIL:
        InitIMPDETAIL();
        break;
    
    //资源表
    case IDC_BUTTON_RESDETAIL:
        InitRESDETAIL();
        break;

    //重定位表
    case IDC_BUTTON_RELOCDETAIL:
        InitRELOCDETAIL();
        break;

    //绑定导入表
    case IDC_BUTTON_BINDIPDETAIL:
        InitBINDIPDETAIL();
        break;

    //IAT 表
    case IDC_BUTTON_IATDETAIL:
        InitIATDETAIL();
        break;

    default:
        result = FALSE;
    }
    return result;
}
