#pragma once
#ifndef _WIN32
#else
#include <windows.h>
#endif

#define PROCESS_MAX_NUM (1024)

class CProcessManager
{
public:
	CProcessManager();
	~CProcessManager();

private:
#ifndef _WIN32
#else
	typedef struct _PROCESS_ENTRY {
		DWORD dwProcessID;
		char szProcessName[MAX_PATH];
		HANDLE hProcess;
	} PROCESS_ENTRY, *PPROCESS_ENTRY;
	DWORD g_cProcess;
	PROCESS_ENTRY g_ProcessTable[PROCESS_MAX_NUM];

	void InitializeProcessTable();
	const char* GetProcessName(DWORD dwProcessID);
#endif

public:
	void TerminateAllProcess();
	void TerminatePortProcess(const int port);
	bool IsPortInuse(const int port);
};

