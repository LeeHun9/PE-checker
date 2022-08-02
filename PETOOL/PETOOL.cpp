#include "PETOOL.h"
#define     _CRT_SECURE_NO_WARNINGS

PIMAGE_DOS_HEADER GetDosHeader(const PVOID  p_FileBuffer)
{
	return (PIMAGE_DOS_HEADER)p_FileBuffer;
}


PIMAGE_FILE_HEADER GetFileHeader(const PVOID p_FileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = GetDosHeader(p_FileBuffer);
	return (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
}


PIMAGE_OPTIONAL_HEADER GetOptionalHeader(const PVOID p_FileBuffer)
{
	PIMAGE_FILE_HEADER pFileHeader = GetFileHeader(p_FileBuffer);
	return (PIMAGE_OPTIONAL_HEADER)((DWORD)pFileHeader + sizeof(IMAGE_FILE_HEADER));
}


DWORD GetSectionNumbers(const PVOID p_FileBuffer)
{
	PIMAGE_FILE_HEADER pFileHeader = GetFileHeader(p_FileBuffer);
	return (DWORD)pFileHeader->NumberOfSections;
}


PIMAGE_SECTION_HEADER GetSectionHeader(const PVOID p_FileBuffer, DWORD index)
{
	PIMAGE_OPTIONAL_HEADER pOPtionalHeader = GetOptionalHeader(p_FileBuffer);
	PIMAGE_FILE_HEADER pFileHeader = GetFileHeader(p_FileBuffer);
	return (PIMAGE_SECTION_HEADER)((DWORD)pOPtionalHeader + pFileHeader->SizeOfOptionalHeader \
		+ index * sizeof(IMAGE_SECTION_HEADER));
}


PIMAGE_DATA_DIRECTORY GetDataDirectory(const PVOID p_FileBuffer, DWORD index)
{
	if (CheckPE64(p_FileBuffer))
	{
		PIMAGE_OPTIONAL_HEADER64 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER64)GetOptionalHeader(p_FileBuffer);
		PIMAGE_DATA_DIRECTORY pDataDirectory = pOptionalHeader->DataDirectory;
		return pDataDirectory + index;
	}
	else
	{
		PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = GetOptionalHeader(p_FileBuffer);
		PIMAGE_DATA_DIRECTORY pDataDirectory = pOptionalHeader->DataDirectory;
		return pDataDirectory + index;
	}
}


PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor(const PVOID p_FileBuffer, DWORD index)
{
	PIMAGE_DATA_DIRECTORY p_DateDirectoryEntryImport = GetDataDirectory(p_FileBuffer, IMAGE_DIRECTORY_ENTRY_IMPORT);

	if (p_DateDirectoryEntryImport->VirtualAddress == 0)
	{
		return NULL;
	}

	DWORD importDescriptorRva = p_DateDirectoryEntryImport->VirtualAddress;
	DWORD importDescriptorFoa = Rva2Foa(p_FileBuffer, importDescriptorRva);

	return (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)p_FileBuffer + importDescriptorFoa) + index;
}


PIMAGE_BOUND_IMPORT_DESCRIPTOR GetBoundImportDescriptor(const PVOID p_FileBuffer)
{
	PIMAGE_DATA_DIRECTORY p_DateDirectoryBoundImport = GetDataDirectory(p_FileBuffer, IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);

	if (p_DateDirectoryBoundImport->VirtualAddress == 0)
	{
		return NULL;
	}

	DWORD boundImportRva = p_DateDirectoryBoundImport->VirtualAddress;
	DWORD boundImportFoa = Rva2Foa(p_FileBuffer, boundImportRva);

	return (PIMAGE_BOUND_IMPORT_DESCRIPTOR)((DWORD)p_FileBuffer + boundImportFoa);
}


DWORD GetImportDescriptorNumbers(const PVOID p_FileBuffer)
{
	DWORD countOfImportDescriptor = 0;


	while (TRUE)
	{
		PIMAGE_IMPORT_DESCRIPTOR p_ImportDescriptor = GetImportDescriptor(p_FileBuffer, countOfImportDescriptor);
		if (!p_ImportDescriptor->OriginalFirstThunk

			&& !p_ImportDescriptor->ForwarderChain
			&& !p_ImportDescriptor->Name
			&& !p_ImportDescriptor->FirstThunk)
		{
			return countOfImportDescriptor;
		}
		else
		{
			countOfImportDescriptor++;
		}
	}
}


PIMAGE_EXPORT_DIRECTORY GetExportDirectory(const PVOID p_FileBuffer)
{
	PIMAGE_DATA_DIRECTORY p_ExportDataDirectory = GetDataDirectory(p_FileBuffer, IMAGE_DIRECTORY_ENTRY_EXPORT);

	if (p_ExportDataDirectory->VirtualAddress == 0)
	{
		return NULL;
	}

	DWORD exportDirectoryRva = p_ExportDataDirectory->VirtualAddress;
	DWORD exportDirectoryFoa = Rva2Foa(p_FileBuffer, exportDirectoryRva);

	return (PIMAGE_EXPORT_DIRECTORY)((DWORD)p_FileBuffer + exportDirectoryFoa);
}


PIMAGE_BASE_RELOCATION GetRelocation(const PVOID p_FileBuffer)
{
	PIMAGE_DATA_DIRECTORY p_BaserelocDataDirectory = GetDataDirectory(p_FileBuffer, IMAGE_DIRECTORY_ENTRY_BASERELOC);
	if (p_BaserelocDataDirectory->VirtualAddress == 0)
	{
		return NULL;
	}

	DWORD dw_RelocationRva = p_BaserelocDataDirectory->VirtualAddress;
	DWORD dw_RelocationFoa = Rva2Foa(p_FileBuffer, dw_RelocationRva);

	return (PIMAGE_BASE_RELOCATION)((DWORD)p_FileBuffer + dw_RelocationFoa);
}


PIMAGE_RESOURCE_DIRECTORY GetResourceDirectory(const PVOID p_FileBuffer)
{
	PIMAGE_DATA_DIRECTORY p_ResourceDataDirectory = GetDataDirectory(p_FileBuffer, IMAGE_DIRECTORY_ENTRY_RESOURCE);
	if (p_ResourceDataDirectory->VirtualAddress == 0)
	{
		return NULL;
	}

	DWORD dw_ResourceRva = p_ResourceDataDirectory->VirtualAddress;
	DWORD dw_ResourceFoa = Rva2Foa(p_FileBuffer, dw_ResourceRva);

	return (PIMAGE_RESOURCE_DIRECTORY)((DWORD)p_FileBuffer + dw_ResourceFoa);
}


DWORD Rva2Foa(const PVOID p_FileBuffer, DWORD rva)
{
	DWORD numberOfSections = GetSectionNumbers(p_FileBuffer);
	PIMAGE_SECTION_HEADER firstSection = GetSectionHeader(p_FileBuffer, 0);
	if (rva < firstSection->VirtualAddress)
	{
		return rva;
	}
	PIMAGE_SECTION_HEADER targetSection = NULL;
	for (DWORD i = numberOfSections - 1; i >= 0; i--)
	{
		targetSection = GetSectionHeader(p_FileBuffer, i);

		if (rva >= targetSection->VirtualAddress)
		{
			break;
		}
	}
	DWORD offset = targetSection->VirtualAddress - targetSection->PointerToRawData;  
	DWORD foa = rva - offset;
	return foa;
}


DWORD AlignedSize(DWORD unalignedSize, DWORD aligenment)
{
	if ((unalignedSize % aligenment) == 0)
	{
		return unalignedSize;
	}
	else
	{
		return ((unalignedSize / aligenment) + 1) * aligenment;
	}
}


BOOL CheckPE64(const PVOID pFileBuffer)
{
	PIMAGE_FILE_HEADER pFileHeader = GetFileHeader(pFileBuffer);
	return (pFileHeader->SizeOfOptionalHeader == 0x00F0);
}


BOOL ReadPeFile(LPTSTR lp_FilePath, OUT PVOID* p_FileBuffer, OUT DWORD* bufferSize)
{
	PVOID p_newBuffer = NULL;
	DWORD fileSize = 0;
	FILE* pFile = NULL;

	if (lp_FilePath == NULL)
	{
		return FALSE;
	}

	fopen_s(&pFile,lp_FilePath, TEXT("rb"));
	if (!pFile)
	{
		return FALSE;
	}

	fseek(pFile, 0, SEEK_END);
	fileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	p_newBuffer = malloc(fileSize);
	if (!p_newBuffer)
	{
		fclose(pFile);
		return FALSE;
	}

	size_t n = fread(p_newBuffer, fileSize, 1, pFile);
	if (!n)
	{
		free(p_newBuffer);
		fclose(pFile);
		return FALSE;
	}
	fclose(pFile);

	*p_FileBuffer = p_newBuffer;
	*bufferSize = fileSize;

	return TRUE;
}

// 将 FileBuffer 拉伸成 ImageBuffer 并写入到新的缓冲区
// 返回 ImageBuffer 的大小；失败返回0
DWORD CopyFileBufferToImageBuffer(LPVOID pFileBuffer, LPVOID* pImageBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSectionHeader = \
		(PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	*pImageBuffer = malloc(pOptionHeader->SizeOfImage);
	if (*pImageBuffer == NULL)
	{
		//printf("分配内存失败\n");
		return 0;
	}
	memset(*pImageBuffer, 0, pOptionHeader->SizeOfImage);
	// 复制DOS头+PE头+可选PE头+节表+文件对齐
	memcpy(*pImageBuffer, pFileBuffer, pOptionHeader->SizeOfHeaders);
	// 遍历节表，复制所有节	
	for (int i = 0; i < pPEHeader->NumberOfSections; i++)
	{
		memcpy((LPVOID)((DWORD)(*pImageBuffer) + pSectionHeader[i].VirtualAddress), \
			(LPVOID)((DWORD)pFileBuffer + pSectionHeader[i].PointerToRawData), \
			pSectionHeader[i].SizeOfRawData);
	}
	return pOptionHeader->SizeOfImage;
}

// 将 ImageBuffer 变成文件对齐的 FileBuffer 写入新的缓冲区
// 返回复制的大小，失败返回0
DWORD CopyImageBufferToFileBuffer(LPVOID pImageBuffer, LPVOID* pNewBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSectionHeader = \
		(PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	// 最后一个节表
	PIMAGE_SECTION_HEADER pLastSectionHeader = pSectionHeader + pPEHeader->NumberOfSections - 1;
	// 计算要复制的字节
	// 这一步有BUG，当最后一个节后面还有数据时（多见于控制台程序），丢失数据
	DWORD dwFileBufferSize = pLastSectionHeader->PointerToRawData + pLastSectionHeader->SizeOfRawData;
	*pNewBuffer = malloc(dwFileBufferSize);
	if (*pNewBuffer == NULL)
	{
		//printf("分配内存失败\n");
		return 0;
	}
	memset(*pNewBuffer, 0, dwFileBufferSize);
	// 复制DOS头+PE头+可选PE头+节表+文件对齐
	memcpy(*pNewBuffer, pImageBuffer, pOptionHeader->SizeOfHeaders);
	// 遍历节表，复制文件对齐后的节	
	for (int i = 0; i < pPEHeader->NumberOfSections; i++)
	{
		memcpy((LPVOID)((DWORD)(*pNewBuffer) + pSectionHeader[i].PointerToRawData), \
			(LPVOID)((DWORD)pImageBuffer + pSectionHeader[i].VirtualAddress), \
			pSectionHeader[i].SizeOfRawData);
	}
	return dwFileBufferSize;
}

// 内存数据写入文件
BOOL MemoryToFile(LPVOID pMemBuffer, DWORD dwSize, LPCSTR lpszFile)
{
	FILE* fp = NULL;
	fp = fopen(lpszFile, "wb+");
	if (fp == NULL)
	{
		//printf("打开文件失败\n");
		return FALSE;
	}
	DWORD dwWritten = fwrite(pMemBuffer, 1, dwSize, fp);
	if (dwWritten != dwSize)
	{
		//printf("写入了 %d 字节，不等于 %d\n", dwWritten, dwSize);
		fclose(fp);
		return FALSE;
	}
	fclose(fp);
	return TRUE;
}


// 移动NT头和节表到DOS STUB，该函数在新增节时节表空间不足的情况下调用；返回地址减小值
DWORD MoveNTHeaderAndSectionHeadersToDosStub(LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSectionHeader = \
		(PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	LPVOID pDst = (LPVOID)((DWORD)pDosHeader + sizeof(IMAGE_DOS_HEADER)); // NT头插入点
	DWORD dwRet = (DWORD)pNTHeader - (DWORD)pDst; // 返回地址减小的值
	DWORD dwSize = 4 + sizeof(IMAGE_FILE_HEADER) + pPEHeader->SizeOfOptionalHeader + \
		sizeof(IMAGE_SECTION_HEADER) * pPEHeader->NumberOfSections; // 移动的字节数
	LPVOID pSrc = malloc(dwSize);
	if (pSrc == NULL)
	{
		//printf("分配内存失败\n");
		return 0;
	}
	memcpy(pSrc, (LPVOID)pNTHeader, dwSize); // 保存要复制的数据
	memset((LPVOID)pNTHeader, 0, dwSize); // 清空原数据
	memcpy(pDst, pSrc, dwSize); // 移动数据
	free(pSrc);
	pDosHeader->e_lfanew = sizeof(IMAGE_DOS_HEADER); // 更新 e_lfanew

	return dwRet;
}

// 新增一个大小为 newSectionSize 的代码节
// dwFileBufferSize 是原来的文件大小
// 返回新缓冲区的大小，失败返回0
DWORD AddCodeSection(LPVOID pFileBuffer, LPVOID* pNewFileBuffer, DWORD dwFileBufferSize, DWORD dwNewSectionSize)
{
	// 复制一份 pFileBuffer，不要修改原来的数据
	LPVOID pFileBuffer2 = malloc(dwFileBufferSize);
	memcpy(pFileBuffer2, pFileBuffer, dwFileBufferSize);
	pFileBuffer = pFileBuffer2;

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSectionHeader = \
		(PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	PWORD pNumberOfSections = &(pPEHeader->NumberOfSections); // 节的数量
	PIMAGE_SECTION_HEADER pLastSectionHeader = pSectionHeader + *pNumberOfSections - 1; // 最后一个节表
	PIMAGE_SECTION_HEADER pNewSectionHeader = pSectionHeader + *pNumberOfSections; // 新节表插入点
	DWORD newFileBufferSize = 0; // 新文件的大小

	// 判断最后一个节表后面是否有空闲的80字节
	if (80 > (DWORD)pFileBuffer + pOptionHeader->SizeOfHeaders - (DWORD)pNewSectionHeader)
	{
		printf("没有足够的80字节插入新节表\n");
		free(pFileBuffer2);
		return 0;
	}

	// 判断空闲的80字节是否全为0，如果不是，则把整个NT头往上挪覆盖dos stub以空出空间插入节表
	for (int i = 0; i < 80; i++)
	{
		if (((PBYTE)pNewSectionHeader)[i] != 0)
		{
			DWORD dwRet = MoveNTHeaderAndSectionHeadersToDosStub(pFileBuffer);
			printf("节表空间不足，NT头和节表向低地址移动了 %d 字节\n", dwRet);
			if (dwRet < 80)
			{
				printf("移动后仍没有足够的80字节空间插入新节表\n");
				free(pFileBuffer2);
				return 0;
			}
			// 更新指针
			pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
			pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
			pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + sizeof(IMAGE_FILE_HEADER));
			pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
			pNumberOfSections = &(pPEHeader->NumberOfSections); // 节的数量
			pLastSectionHeader = pSectionHeader + *pNumberOfSections - 1; // 最后一个节表
			pNewSectionHeader = pSectionHeader + *pNumberOfSections; // 新节表插入点
			break;
		}
	}

	// 定义一个 IMAGE_SECTION_HEADER 结构，计算里面的属性
	IMAGE_SECTION_HEADER newSectionHeader;
	memcpy(newSectionHeader.Name, ".newsec", 8);
	newSectionHeader.Misc.VirtualSize = AlignedSize(dwNewSectionSize, pOptionHeader->SectionAlignment);
	newSectionHeader.VirtualAddress = pLastSectionHeader->VirtualAddress + \
		AlignedSize(pLastSectionHeader->Misc.VirtualSize, pOptionHeader->SectionAlignment);
	newSectionHeader.SizeOfRawData = AlignedSize(dwNewSectionSize, pOptionHeader->FileAlignment);
	newSectionHeader.PointerToRawData = pLastSectionHeader->PointerToRawData + pLastSectionHeader->SizeOfRawData;
	newSectionHeader.PointerToRelocations = 0;
	newSectionHeader.PointerToLinenumbers = 0;
	newSectionHeader.NumberOfRelocations = 0;
	newSectionHeader.NumberOfLinenumbers = 0;
	newSectionHeader.Characteristics = 0x60000020;

	// pNewFileBuffer 分配内存，把 pFileBuffer 复制过去，后面的修改都在 pNewFileBuffer 进行
	*pNewFileBuffer = malloc(dwFileBufferSize + newSectionHeader.SizeOfRawData);
	memcpy(*pNewFileBuffer, pFileBuffer, dwFileBufferSize);
	memset((LPVOID)((DWORD)*pNewFileBuffer + dwFileBufferSize), 0, newSectionHeader.SizeOfRawData); // 新增节数据清0

	// 更新指针，指向新内存	
	pDosHeader = (PIMAGE_DOS_HEADER)*pNewFileBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + sizeof(IMAGE_FILE_HEADER));
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	pNumberOfSections = &(pPEHeader->NumberOfSections);
	pLastSectionHeader = pSectionHeader + *pNumberOfSections - 1;
	pNewSectionHeader = pSectionHeader + *pNumberOfSections;

	// 节的数量+1，SizeOfImage是内存中拉伸后的大小
	*pNumberOfSections += 1;
	pOptionHeader->SizeOfImage += AlignedSize(newSectionHeader.Misc.VirtualSize, pOptionHeader->SectionAlignment);

	// 拷贝 newSectionHeader
	memcpy(pNewSectionHeader, &newSectionHeader, sizeof(newSectionHeader));

	//printf("插入成功\n");
	free(pFileBuffer2);
	return dwFileBufferSize + newSectionHeader.SizeOfRawData;
}