#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
#include <microhttpd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <time.h>
#include <vector>
#include "ProcessManager.h"
#include "utilities.h"
#include <string>

#define MAX_PARAM_SIZE 256
#define POSTBUFFERSIZE 1024
#define GET 0
#define POST 1
#define START_PORT 8889
#define PORT_LENGTH 30

static char s_pcWorkingDir[256];
struct PsProcessInfo {
	char session[256];
	time_t time;
	int count;
};
std::map<int, PsProcessInfo> _activeProcessMap;
std::map<std::string, std::string> _params;

struct connection_info_struct
{
	int connectiontype;
	char *sessionId;
	struct MHD_PostProcessor *postprocessor;
};

void deleteWorkingFiles(const char *session)
{
	{
		char filePath[FILENAME_MAX];
		sprintf(filePath, "%s%s.snp_txt", s_pcWorkingDir, session);
		remove(filePath);
	}
	{
		char filePath[FILENAME_MAX];
		sprintf(filePath, "%s%s.X_T", s_pcWorkingDir, session);
		remove(filePath);
	}
	{
		char filePath[FILENAME_MAX];
		sprintf(filePath, "%s%s.sfr", s_pcWorkingDir, session);
		remove(filePath);
	}
	{
		char filePath[FILENAME_MAX];
		sprintf(filePath, "%s%s.tet", s_pcWorkingDir, session);
		remove(filePath);
	}
	{
		char filePath[FILENAME_MAX];
		sprintf(filePath, "%s%s.bdf", s_pcWorkingDir, session);
		remove(filePath);
	}
	{
		char filePath[FILENAME_MAX];
		remove(filePath);
	}
}

void deleteTimeOutProcess(const char *activeSession)
{
	time_t timeout = 60 * 30;	// 30 min
	std::vector<int> timeOutPorts;

	CProcessManager procMgr = CProcessManager();

	for (auto itr = _activeProcessMap.begin(); itr != _activeProcessMap.end(); ++itr)
	{
		PsProcessInfo procInfo = itr->second;
		// Check aidring time without active session
		if (0 != strcmp(activeSession, procInfo.session))
		{
			time_t elapsedTime = time(0) - procInfo.time;
			if (elapsedTime > timeout)
				timeOutPorts.push_back(itr->first);
		}
	}

	for (int i = 0; i < timeOutPorts.size(); i++)
	{
		auto itr = _activeProcessMap.find(timeOutPorts[i]);
		if (itr != _activeProcessMap.end())
		{
			// Terminate timeout process
			procMgr.TerminatePortProcess(timeOutPorts[i]);

			// Delete working files
			PsProcessInfo procInfo = itr->second;
			deleteWorkingFiles(procInfo.session);
			printf("  Process tarminated (Port: %d, Session: %s\n", timeOutPorts[i], procInfo.session);

			_activeProcessMap.erase(itr);
		}
	}
}

int sendResponseSuccess(struct MHD_Connection *connection)
{
	struct MHD_Response *response;
	int ret = MHD_NO;

	response = MHD_create_response_from_buffer(0, (void*)NULL, MHD_RESPMEM_MUST_COPY);
	MHD_add_response_header(response, MHD_HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN, "*");
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);

	return ret;
}

int sendResponseIntArr(struct MHD_Connection *connection, std::vector<int> &intArray)
{
	struct MHD_Response *response;
	int ret = MHD_NO;

	if (0 < intArray.size())
	{
		response = MHD_create_response_from_buffer(intArray.size() * sizeof(float), (void*)&intArray[0], MHD_RESPMEM_MUST_COPY);
		std::vector<int>().swap(intArray);
	}
	else
	{
		return MHD_NO;
	}

	MHD_add_response_header(response, MHD_HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN, "*");

	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);

	return ret;
}

static int
iterate_post(void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
	const char *filename, const char *content_type,
	const char *transfer_encoding, const char *data,
	uint64_t off, size_t size)
{
	struct connection_info_struct *con_info = (connection_info_struct*)coninfo_cls;

	if ((size > 0) && (size <= MAX_PARAM_SIZE))
	{
		if (0 == strcmp(key, "sessionId"))
		{
			char *session;
			session = (char*)malloc(MAX_PARAM_SIZE);
			if (!session) return MHD_NO;

			sprintf(session, "%s", data);
			con_info->sessionId = session;
		}
		else
			_params.insert(std::make_pair(std::string(key), std::string(data)));

	}

	return MHD_YES;
}

int findUnsedPort()
{
	// Find unused port
	int iPort = START_PORT;
	for (int i = 0; i < PORT_LENGTH; i++)
	{
		bool inUse = false;
		for (auto itr = _activeProcessMap.begin(); itr != _activeProcessMap.end(); ++itr)
		{
			if (itr->first == iPort)
			{
				inUse = true;
				break;
			}
		}
		if (inUse)
			iPort++;
		else
		{
			// Check whether the port isn't used by other process
			CProcessManager procMgr = CProcessManager();
			if (procMgr.IsPortInuse(iPort))
				iPort++;
			else
				break;
		}
	}

	if (iPort < START_PORT + PORT_LENGTH)
		return iPort;
	else
		return 0;
}

int findIdlePort()
{
	// Find idle port (no count & oldest)
	int iPort = 0;
	time_t oldestTime = time(0);

	for (auto itr = _activeProcessMap.begin(); itr != _activeProcessMap.end(); ++itr)
	{
		PsProcessInfo procInfo = itr->second;
		if (0 == procInfo.count)
		{
			if (oldestTime > procInfo.time)
			{
				iPort = itr->first;
				oldestTime = procInfo.time;
			}
		}
	}

	return iPort;
}

int answer_to_connection(void *cls, struct MHD_Connection *connection,
	const char *url,
	const char *method, const char *version,
	const char *upload_data,
	size_t *upload_data_size, void **con_cls)
{
	printf("New %s request for %s using version %s\n", method, url, version);

	int ret = MHD_NO;

	if (NULL == *con_cls)
	{
		struct connection_info_struct *con_info;

		con_info = (connection_info_struct*)malloc(sizeof(struct connection_info_struct));
		if (NULL == con_info) return MHD_NO;
		con_info->sessionId = NULL;

		// If the new request is a POST, the postprocessor must be created now.
		// In addition, the type of the request is stored for convenience.
		if (0 == strcmp(method, "POST"))
		{
			con_info->postprocessor
				= MHD_create_post_processor(connection, POSTBUFFERSIZE,
				(MHD_PostDataIterator)iterate_post, (void*)con_info);

			if (NULL == con_info->postprocessor)
			{
				free(con_info);
				return MHD_NO;
			}
			con_info->connectiontype = POST;
		}
		else con_info->connectiontype = GET;
		// The address of our structure will both serve as the indicator for successive iterations and to remember the particular details about the connection.

		*con_cls = (void*)con_info;
		return MHD_YES;
	}

	// The rest of the function will not be executed on the first iteration. A GET request is easily satisfied by sending the question form.
	if (0 == strcmp(method, "GET"))
	{
		return ret;
	}

	// In case of POST, we invoke the post processor for as long as data keeps incoming, setting *upload_data_size to zero in order to indicate that we have processed―or at least have considered―all of it.
	else if (0 == strcmp(method, "POST"))
	{
		struct connection_info_struct *con_info = (connection_info_struct*)*con_cls;

		if (*upload_data_size != 0)
		{
			MHD_post_process(con_info->postprocessor, upload_data,
				*upload_data_size);
			*upload_data_size = 0;

			return MHD_YES;
		}
		else if (NULL != con_info->sessionId)
		{
			deleteTimeOutProcess(con_info->sessionId);

			if (0 == strcmp(url, "/RequestPsServer"))
			{
				// Find unused port
				int iPort = findUnsedPort();

				// If not find, find idle port (no count & oldest)
				if (0 == iPort)
					iPort = findIdlePort();

				if (0 != iPort)
				{
					// Start PsServer with a port
					char cmd[256];
					sprintf(cmd, "start PsServer.exe %d", iPort);
					system(cmd);

					// Create process map
					PsProcessInfo procInfo;
					strcpy(procInfo.session, con_info->sessionId);
					procInfo.time = time(0);
					procInfo.count = 0;
					_activeProcessMap[iPort] = procInfo;

					printf("  Process started (Port: %d, Session: %s\n", iPort, con_info->sessionId);

					// Return the port number to the client 
					std::vector <int> intArray;
					intArray.push_back(iPort);
					return sendResponseIntArr(connection, intArray);
				}
				else
				{
					// Return 0 (port is full) to the client 
					std::vector <int> intArray;
					intArray.push_back(0);

					printf("  Process is full.\n");

					return sendResponseIntArr(connection, intArray);
				}
			}
			if (0 == strcmp(url, "/CheckProcessAlive"))
			{
				std::string sPort = _params["port"];
				if (sPort.empty())
					return MHD_NO;

				int iPort = atoi(sPort.c_str());
				if (iPort)
				{
					// Check active process alive
					for (auto itr = _activeProcessMap.begin(); itr != _activeProcessMap.end(); ++itr)
					{
						if (itr->first == iPort)
						{
							// Check session ID is same
							PsProcessInfo *procInfo = &itr->second;
							if (0 == strcmp(con_info->sessionId, procInfo->session))
							{
								// Update time
								procInfo->time = time(0);
								// Increment count
								procInfo->count++;

								// Retrun success
								return sendResponseSuccess(connection);
							}
						}
					}
				}
				return MHD_NO;
			}

		}
	}
	return ret;
}

void request_completed(void *cls, struct MHD_Connection *connection,
	void **con_cls,
	enum MHD_RequestTerminationCode toe)
{
	struct connection_info_struct *con_info = (connection_info_struct*)*con_cls;

	if (NULL == con_info) return;
	if (con_info->connectiontype == POST)
	{
		MHD_destroy_post_processor(con_info->postprocessor);
		if (con_info->sessionId) free(con_info->sessionId);
		if (_params.size())
		{
			_params.clear();
			std::map<std::string, std::string>(_params).swap(_params);
		}
	}

	free(con_info);
	*con_cls = NULL;
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		printf("%s PORT\n",
			argv[0]);
		return 1;
	}

	int iPort = atoi(argv[1]);
	printf("Bind to %d port\n", iPort);

	// Get working dir
	GetEnvironmentVariablePath("EX_SERVER_WORKING_DIR", s_pcWorkingDir, true);
	if (0 == strlen(s_pcWorkingDir))
		return 1;
	printf("EX_SERVER_WORKING_DIR=%s\n", s_pcWorkingDir);

	// Tarminate all ExServer processes
	{
		CProcessManager procMgr = CProcessManager();
		procMgr.TerminateAllProcess();
	}

	// Delete all working files
#ifndef _WIN32
	delete_files(s_pcWorkingDir);
#else
	wchar_t wscDir[_MAX_FNAME];
	size_t iRet;
	mbstowcs_s(&iRet, wscDir, _MAX_FNAME, s_pcWorkingDir, _MAX_FNAME);
	delete_files(wscDir);
#endif

	// Start demon
	struct MHD_Daemon *daemon;

	daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, iPort, NULL, NULL,
		(MHD_AccessHandlerCallback)&answer_to_connection, NULL,
		MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL,
		MHD_OPTION_END);
	if (NULL == daemon)
	{
		fprintf(stderr,
			"Failed to start daemon.\n");
		return 1;
	}

	printf("MHD_start_daemon\n");

	bool bFlg = true;
	while (bFlg)
	{
		if (getchar())
			bFlg = false;

	}

	MHD_stop_daemon(daemon);

	return 0;
}