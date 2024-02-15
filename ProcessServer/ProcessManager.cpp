#include "ProcessManager.h"
#ifndef _WIN32
#else
#include <iphlpapi.h>
#include <tlhelp32.h>
#endif
#include <stdio.h>

#include <assert.h>

CProcessManager::CProcessManager()
{
}

CProcessManager::~CProcessManager()
{
}

#ifndef _WIN32
#else
void CProcessManager::InitializeProcessTable() {

	HANDLE                  hProcessSnap = NULL;
	PROCESSENTRY32     pe32;
	BOOL               bRet = FALSE;
	DWORD               i = 0;
	g_cProcess = 0;

	ZeroMemory(&pe32, sizeof(pe32));

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	assert(INVALID_HANDLE_VALUE != hProcessSnap);

	pe32.dwSize = sizeof(PROCESSENTRY32);

	bRet = Process32First(hProcessSnap, &pe32);

	if (bRet) {

		do {
			wchar_t processName[MAX_PATH];
			swprintf_s(processName, L"%s", pe32.szExeFile);
			size_t iRet;
			wcstombs_s(&iRet, g_ProcessTable[i].szProcessName, processName, MAX_PATH);

			g_ProcessTable[i].dwProcessID = pe32.th32ProcessID;

			g_ProcessTable[i].hProcess = hProcessSnap;

			i++;
		} while (Process32Next(hProcessSnap, &pe32)

			&& i < PROCESS_MAX_NUM);

		g_cProcess = i;

	}

	CloseHandle(hProcessSnap);


}

const char* CProcessManager::GetProcessName(DWORD dwProcessID) {

	for (DWORD i = 0; i < g_cProcess; i++) {

		if (g_ProcessTable[i].dwProcessID == dwProcessID) {

			return g_ProcessTable[i].szProcessName;
		}

	}

	return "";

}


const char* GetTcpState(DWORD dwState) {

	switch (dwState) {
	case MIB_TCP_STATE_CLOSED:
		return "CLOSED";
	case MIB_TCP_STATE_LISTEN:
		return "LISTEN";
	case MIB_TCP_STATE_SYN_SENT:
		return "SYN_SENT";
	case MIB_TCP_STATE_SYN_RCVD:
		return "SYN_RCVD";
	case MIB_TCP_STATE_ESTAB:
		return "ESTAB";
	case MIB_TCP_STATE_FIN_WAIT1:
		return "FIN_WAIT1";
	case MIB_TCP_STATE_FIN_WAIT2:
		return "FIN_WAIT2";
	case MIB_TCP_STATE_CLOSE_WAIT:
		return "CLOSE_WAIT";
	case MIB_TCP_STATE_CLOSING:
		return "CLOSING";
	case MIB_TCP_STATE_LAST_ACK:
		return "LAST_ACK";
	case MIB_TCP_STATE_TIME_WAIT:
		return "TIME_WAIT";
	case MIB_TCP_STATE_DELETE_TCB:
		return "DELETE_TCB";
	}

	return "(unknown)";

}
#endif

void CProcessManager::TerminateAllProcess()
{
#ifndef _WIN32
#else
	PMIB_TCPTABLE_OWNER_PID pTcpTable = NULL;
	DWORD dwSize = 0;
	BOOL bOrder = TRUE;
	ULONG ulAf = AF_INET;
	MIB_TCPROW_OWNER_PID row;
	DWORD dwRet;

	// Initialize the process table array
	InitializeProcessTable();

	// Get the buffer size
	dwRet = GetExtendedTcpTable(NULL, &dwSize, bOrder, ulAf, TCP_TABLE_OWNER_PID_ALL, 0);
	assert(dwRet == ERROR_INSUFFICIENT_BUFFER);

	// Allocate the buffer
	pTcpTable = (PMIB_TCPTABLE_OWNER_PID)malloc(dwSize);
	assert(pTcpTable);

	// Get TCP table
	dwRet = GetExtendedTcpTable(pTcpTable, &dwSize, bOrder, ulAf, TCP_TABLE_OWNER_PID_ALL, 0);
	assert(dwRet == NO_ERROR);


	for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
		in_addr addr_l, addr_r;

		row = pTcpTable->table[i];

		addr_l.S_un.S_addr = row.dwLocalAddr;
		addr_r.S_un.S_addr = row.dwRemoteAddr;

		//printf("%-11s %15s:%-8d %15s:%-8d %-6u %s\n",
		//	GetTcpState(row.dwState),
		//	inet_ntoa(addr_l),
		//	ntohs(row.dwLocalPort),

		//	inet_ntoa(addr_r),
		//	ntohs(row.dwRemotePort),
		//	row.dwOwningPid,

		//	GetProcessName(row.dwOwningPid)
		//);
		
		const char *processName = GetProcessName(row.dwOwningPid);
		if (0 == strcmp("PsServer.exe", processName))
		{
			DWORD dwProcId = row.dwOwningPid;
			HANDLE hProc = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);

			TerminateProcess(hProc, 0);
		}

	}

	free(pTcpTable);
#endif
}


void CProcessManager::TerminatePortProcess(int port)
{
#ifndef _WIN32
#else
	PMIB_TCPTABLE_OWNER_PID pTcpTable = NULL;
	DWORD dwSize = 0;
	BOOL bOrder = TRUE;
	ULONG ulAf = AF_INET;
	MIB_TCPROW_OWNER_PID row;
	DWORD dwRet;

	// Initialize the process table array
	InitializeProcessTable();

	// Get the buffer size
	dwRet = GetExtendedTcpTable(NULL, &dwSize, bOrder, ulAf, TCP_TABLE_OWNER_PID_ALL, 0);
	assert(dwRet == ERROR_INSUFFICIENT_BUFFER);

	// Allocate the buffer
	pTcpTable = (PMIB_TCPTABLE_OWNER_PID)malloc(dwSize);
	assert(pTcpTable);

	// Get TCP table
	dwRet = GetExtendedTcpTable(pTcpTable, &dwSize, bOrder, ulAf, TCP_TABLE_OWNER_PID_ALL, 0);
	assert(dwRet == NO_ERROR);


	for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
		in_addr addr_l, addr_r;

		row = pTcpTable->table[i];

		addr_l.S_un.S_addr = row.dwLocalAddr;
		addr_r.S_un.S_addr = row.dwRemoteAddr;

		const char *processName = GetProcessName(row.dwOwningPid);
		int localPort = ntohs(row.dwLocalPort);

		if (0 == strcmp("PsServer.exe", processName) && port == localPort)
		{
			DWORD dwProcId = row.dwOwningPid;
			HANDLE hProc = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);

			TerminateProcess(hProc, 0);
		}

	}

	free(pTcpTable);
#endif
}

bool CProcessManager::IsPortInuse(const int port)
{
	bool inUse = false;

#ifndef _WIN32
#else
	PMIB_TCPTABLE_OWNER_PID pTcpTable = NULL;
	DWORD dwSize = 0;
	BOOL bOrder = TRUE;
	ULONG ulAf = AF_INET;
	MIB_TCPROW_OWNER_PID row;
	DWORD dwRet;

	// Get the buffer size
	dwRet = GetExtendedTcpTable(NULL, &dwSize, bOrder, ulAf, TCP_TABLE_OWNER_PID_ALL, 0);
	assert(dwRet == ERROR_INSUFFICIENT_BUFFER);

	// Allocate the buffer
	pTcpTable = (PMIB_TCPTABLE_OWNER_PID)malloc(dwSize);
	assert(pTcpTable);

	// Get TCP table
	dwRet = GetExtendedTcpTable(pTcpTable, &dwSize, bOrder, ulAf, TCP_TABLE_OWNER_PID_ALL, 0);
	assert(dwRet == NO_ERROR);

	for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
		in_addr addr_l, addr_r;

		row = pTcpTable->table[i];

		addr_l.S_un.S_addr = row.dwLocalAddr;
		addr_r.S_un.S_addr = row.dwRemoteAddr;

		char *localAddr = inet_ntoa(addr_l);
		DWORD dwProcId = row.dwOwningPid;

		int localPort = ntohs(row.dwLocalPort);
		if (port == localPort && 0 != dwProcId)
		{ 
			inUse = true;
			break;;
		}
	}

	free(pTcpTable);
#endif
	return inUse;
}