#pragma once
#include <A3DSDKIncludes.h>

#include "libconverter.h"
using namespace Communicator;
#include<string>
#include <vector>
#include <map>

class ExProcess
{
public:
	ExProcess();
	~ExProcess();

private:
	std::map<std::string, A3DAsmModelFile*> m_mModelFile;
    A3DRWParamsLoadData m_sLoadData;
	Converter m_libConverter;
	Importer m_libImporter; // Import Initialization
	bool m_bAddEntityIds;
	bool m_bSewModel;
	double m_dSewingTol;

	void adaptBrep(A3DAsmModelFile *pModelFile);

public:
	bool Init();
	void Terminate();

	void SetOptions(const bool entityIds, const bool sewModel, const double sewingTol);
	void DeleteModelFile(const char* session_id);
	std::vector<float> LoadFile(const char* session_id, const char* file_name, const char* sc_name);
	std::vector<float> AddAttributes(A3DAsmModelFile* pModelFile);
	std::vector<float> ClassifyEntities(const char* session_id, const std::string entityType, const bool simplify, const double tol, const int bodyId = -1, const int entityId = -1);
	std::vector<float> FeatureRecognition(const char* session_id, const bool simplify, const double tol);
};

