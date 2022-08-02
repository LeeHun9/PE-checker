#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wtypes.h>
#include <tchar.h>
#define _CRT_SECURE_NO_WARNINGS

BOOL CheckPE64(const PVOID pFileBuffer);

PIMAGE_DOS_HEADER GetDosHeader(const PVOID  p_FileBuffer);


PIMAGE_FILE_HEADER GetFileHeader(const PVOID p_FileBuffer);


PIMAGE_OPTIONAL_HEADER GetOptionalHeader(const PVOID p_FileBuffer);


DWORD GetSectionNumbers(const PVOID p_FileBuffer);


PIMAGE_SECTION_HEADER GetSectionHeader(const PVOID p_FileBuffer, DWORD index);


PIMAGE_DATA_DIRECTORY GetDataDirectory(const PVOID p_FileBuffer, DWORD index);


PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor(const PVOID p_FileBuffer, DWORD index);


PIMAGE_BOUND_IMPORT_DESCRIPTOR GetBoundImportDescriptor(const PVOID p_FileBuffer);


DWORD GetImportDescriptorNumbers(const PVOID p_FileBuffer);


PIMAGE_EXPORT_DIRECTORY GetExportDirectory(const PVOID p_FileBuffer);


PIMAGE_BASE_RELOCATION GetRelocation(const PVOID p_FileBuffer);


PIMAGE_RESOURCE_DIRECTORY GetResourceDirectory(const PVOID p_FileBuffer);


DWORD Rva2Foa(const PVOID p_FileBuffer, DWORD rva);


DWORD AlignedSize(DWORD unalignedSize, DWORD aligenment);

BOOL ReadPeFile(LPTSTR lp_FilePath, OUT PVOID* p_FileBuffer, OUT DWORD* bufferSize);

BOOL MemoryToFile(LPVOID pMemBuffer, DWORD dwSize, LPCSTR lpszFile);

DWORD AddCodeSection(LPVOID pFileBuffer, LPVOID* pNewFileBuffer, DWORD dwFileBufferSize, DWORD dwNewSectionSize);