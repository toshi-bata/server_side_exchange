#define INITIALIZE_A3D_API
#include "ExProcess.h"

#include "visitor/VisitorContainer.h"
#include "visitor/VisitorCascadedAttribute.h"
#include "visitor/VisitorTree.h"
#include "visitor/VisitorTransfo.h"
#include "visitor/CascadedAttributeConnector.h"
#include "visitor/TransfoConnector.h"
#include "AddAttributesVisitor.h"
#include "ClassifyEntityVisitor.h"
#include "FeatureRecognitionVisitor.h"
#include <iterator>

ExProcess::ExProcess()
{
}

ExProcess::~ExProcess()
{
    Terminate();
}

bool ExProcess::Init()
{
    A3DStatus iRet;
#ifndef _MSC_VER
    if (!A3DSDKLoadLibrary(""))
        return false;
#else

    wchar_t bin_dir[2048];
#ifdef _DEBUG
    std::wstring buffer;
    buffer.resize(2048);
    if (GetEnvironmentVariable(L"HEXCHANGE_INSTALL_DIR", &buffer[0], static_cast<DWORD>(buffer.size())))
    {
        swprintf(bin_dir, _T("%s/bin/win64_v142"), buffer.data());
    }
#else
    swprintf(bin_dir, _T(""));
#endif
    wprintf(_T("Exchange bin dir=\"%s\"\n"), bin_dir);
    if (!A3DSDKLoadLibrary(bin_dir))
        return false;

#endif
    iRet = A3DLicPutUnifiedLicense(HOOPS_LICENSE);
    if (iRet != A3D_SUCCESS)
        return false;

    A3DInt32 iMajorVersion = 0, iMinorVersion = 0;
    iRet = A3DDllGetVersion(&iMajorVersion, &iMinorVersion);
    if (iRet != A3D_SUCCESS)
        return false;

    iRet = A3DDllInitialize(A3D_DLL_MAJORVERSION, A3D_DLL_MINORVERSION);
    if (iRet != A3D_SUCCESS)
        return iRet;

    // Init libconverter
    m_libConverter.Init(HOOPS_LICENSE);

    if (!m_libImporter.Init(&m_libConverter))
        return false;

    return true;
}

void ExProcess::Terminate()
{
    if (A3D_SUCCESS == A3DDllTerminate())
    {
        A3DSDKUnloadLibrary();
        printf("HOOPS Exchanged Unloaded\n");
    }

}

void ExProcess::SetOptions(const bool entityIds, const bool sewModel, const double sewingTol)
{
    m_bAddEntityIds = entityIds;
    m_bSewModel = sewModel;
    m_dSewingTol = sewingTol;

    // Init import options
    A3D_INITIALIZE_DATA(A3DRWParamsLoadData, m_sLoadData);
    m_sLoadData.m_sGeneral.m_bReadSolids = true;
    m_sLoadData.m_sGeneral.m_bReadSurfaces = true;
    m_sLoadData.m_sGeneral.m_bReadWireframes = true;
    m_sLoadData.m_sGeneral.m_bReadPmis = true;
    m_sLoadData.m_sGeneral.m_bReadAttributes = true;
    m_sLoadData.m_sGeneral.m_bReadHiddenObjects = false;
    m_sLoadData.m_sGeneral.m_bReadConstructionAndReferences = false;
    m_sLoadData.m_sGeneral.m_bReadActiveFilter = false;
    m_sLoadData.m_sGeneral.m_eReadingMode2D3D = kA3DRead_3D;
    m_sLoadData.m_sGeneral.m_eReadGeomTessMode = kA3DReadGeomAndTess;
    m_sLoadData.m_sGeneral.m_eDefaultUnit = kA3DUnitUnknown;
    m_sLoadData.m_sTessellation.m_eTessellationLevelOfDetail = kA3DTessLODMedium;
    m_sLoadData.m_sAssembly.m_bUseRootDirectory = true;
    m_sLoadData.m_sMultiEntries.m_bLoadDefault = true;
    m_sLoadData.m_sPmi.m_bAlwaysSubstituteFont = false;
    m_sLoadData.m_sPmi.m_pcSubstitutionFont = (char*)"Myriad CAD";

    m_sLoadData.m_sSpecifics.m_sRevit.m_eMultiThreadedMode = kA3DRevitMultiThreadedMode_Disabled;
}

void ExProcess::DeleteModelFile(const char* session_id)
{
    // Delete loaded modelFile
    if (1 == m_mModelFile.count(session_id))
    {
        A3DAsmModelFile* pModelFile = m_mModelFile[session_id];
        m_mModelFile.erase(session_id);

        A3DStatus iRet = A3DAsmModelFileDelete(pModelFile);
        if (iRet == A3D_SUCCESS)
            printf("ModelFile was removed.\n");
    }
}

std::vector<float> ExProcess::LoadFile(const char* session_id, const char* file_name, const char* sc_name)
{
    std::vector<float> floatArray;
    
    A3DStatus iRet;

    A3DAsmModelFile* pModelFile;

    iRet = A3DAsmModelFileLoadFromFile(file_name, &m_sLoadData, &pModelFile);

    if (iRet != A3D_SUCCESS)
    {
        printf("File loading failed: %d\n", iRet);
        return floatArray;
    }
    printf("Model was loaded\n");
    m_mModelFile.insert(std::make_pair(session_id, pModelFile));

    // Sewing
    if (m_bSewModel)
    {
        A3DSewOptionsData sewOption;
        A3D_INITIALIZE_DATA(A3DSewOptionsData, sewOption);
        iRet = A3DAsmModelFileSew(&pModelFile, m_dSewingTol, &sewOption);
    }

	// Convert UV => 3D
	adaptBrep(pModelFile);

    // Add attributes
    std::vector<float> entityCnt = AddAttributes(pModelFile);

    // HC libconverter
    if (!m_libImporter.Load(pModelFile))
        return floatArray;

    Exporter exporter; // Export Initialization
    if (!exporter.Init(&m_libImporter))
        return floatArray;

    SC_Export_Options exportOptions; // Export Stream Cache Model
    exportOptions.sc_create_scz = true;
    exportOptions.export_attributes = true;

    if (!exporter.WriteSC(sc_name, nullptr, exportOptions))
        return floatArray;
    printf("SC model was exported\n");
 
    // Get unit
    A3DAsmModelFileData sData;
    A3D_INITIALIZE_DATA(A3DAsmModelFileData, sData);
    iRet = A3DAsmModelFileGet(pModelFile, &sData);
    floatArray.push_back(sData.m_dUnit);

    // Retrieve physical properties
    A3DPhysicalPropertiesData sPhysPropsData;
    A3D_INITIALIZE_DATA(A3DPhysicalPropertiesData, sPhysPropsData);
    iRet = A3DComputeModelFilePhysicalProperties(pModelFile, &sPhysPropsData);

    floatArray.push_back(sPhysPropsData.m_dSurface);
    floatArray.push_back(sPhysPropsData.m_dVolume);
    floatArray.push_back(sPhysPropsData.m_sGravityCenter.m_dX);
    floatArray.push_back(sPhysPropsData.m_sGravityCenter.m_dY);
    floatArray.push_back(sPhysPropsData.m_sGravityCenter.m_dZ);

    // Compute bounding box
    A3DBoundingBoxData sBoundingBox;
    A3D_INITIALIZE_DATA(A3DBoundingBoxData, sBoundingBox);
    iRet = A3DMiscComputeBoundingBox(pModelFile, 0, &sBoundingBox);
    double dX = sBoundingBox.m_sMax.m_dX - sBoundingBox.m_sMin.m_dX;
    double dY = sBoundingBox.m_sMax.m_dY - sBoundingBox.m_sMin.m_dY;
    double dZ = sBoundingBox.m_sMax.m_dZ - sBoundingBox.m_sMin.m_dZ;

    floatArray.push_back(dX);
    floatArray.push_back(dY);
    floatArray.push_back(dZ);

    // Merge float array
    floatArray.reserve(floatArray.size() + entityCnt.size());
    std::copy(entityCnt.begin(), entityCnt.end(), std::back_inserter(floatArray));

    iRet = A3DAsmModelFileGet(NULL, &sData);

    return floatArray;
}

std::vector<float> ExProcess::AddAttributes(A3DAsmModelFile* pModelFile)
{
    A3DStatus status;

    std::vector<float> vEntityCount{ 0, 0, 0, 0, 0 };

    if (nullptr == pModelFile)
        return vEntityCount;

    A3DVisitorContainer sA3DVisitorContainer(CONNECT_BREP);
    sA3DVisitorContainer.SetTraverseInstance(true);

    AddAttributesVisitor* pMyTreeVisitor = new AddAttributesVisitor(&sA3DVisitorContainer, m_bAddEntityIds);
    sA3DVisitorContainer.push(pMyTreeVisitor);

    A3DModelFileConnector sModelFileConnector(pModelFile);
    status = sModelFileConnector.Traverse(&sA3DVisitorContainer);

    vEntityCount[0] = pMyTreeVisitor->m_iPartCnt;
    vEntityCount[1] = pMyTreeVisitor->m_iBodyCnt;
    vEntityCount[2] = pMyTreeVisitor->m_iFaceCnt;
    vEntityCount[3] = pMyTreeVisitor->m_iLoopCnt;
    vEntityCount[4] = pMyTreeVisitor->m_iEdgeCnt;

    return vEntityCount;
}

std::vector<float> ExProcess::ClassifyEntities(const char* session_id, const std::string entityType, const bool simplify, const double tol, const int bodyId, const int entityId)
{
	A3DStatus status;

	std::vector<float> vEntityType;

    A3DAsmModelFile* pModelFile = m_mModelFile[session_id];

	if (nullptr == pModelFile)
	    return vEntityType;

	A3DVisitorContainer sA3DVisitorContainer(CONNECT_BREP);
	sA3DVisitorContainer.SetTraverseInstance(true);

	ClassifyEntityVisitor *pMyTreeVisitor = new ClassifyEntityVisitor(&sA3DVisitorContainer, entityType, simplify, tol, bodyId, entityId);
	sA3DVisitorContainer.push(pMyTreeVisitor);

	A3DModelFileConnector sModelFileConnector(pModelFile);
	status = sModelFileConnector.Traverse(&sA3DVisitorContainer);

	vEntityType = pMyTreeVisitor->m_vEntityType;

	return vEntityType;

}

std::vector<float> ExProcess::FeatureRecognition(const char* session_id, const bool simplify, const double tol)
{
    A3DStatus status;

    std::vector<float> vRecognizedFeature;

    A3DAsmModelFile* pModelFile = m_mModelFile[session_id];

    if (nullptr == pModelFile)
        return vRecognizedFeature;

    A3DVisitorContainer sA3DVisitorContainer(CONNECT_BREP);
    sA3DVisitorContainer.SetTraverseInstance(true);

    FeatureRecognitionVisitor* pMyTreeVisitor = new FeatureRecognitionVisitor(&sA3DVisitorContainer, simplify, tol);
    sA3DVisitorContainer.push(pMyTreeVisitor);

    A3DModelFileConnector sModelFileConnector(pModelFile);
    status = sModelFileConnector.Traverse(&sA3DVisitorContainer);

    vRecognizedFeature = pMyTreeVisitor->getRoundHoleFaces();
    
    return vRecognizedFeature;
}


void ExProcess::adaptBrep(A3DAsmModelFile *pModelFile)
{
	A3DStatus status;

	const A3DUns32 NUM_VALID_SURFACES = 11;
	A3DUns32 ACCEPTED_SURFACES[NUM_VALID_SURFACES] = {
		kA3DTypeSurfBlend01,
		//kA3DTypeSurfBlend02,
		//kA3DTypeSurfBlend03,
		kA3DTypeSurfNurbs,
		kA3DTypeSurfCone,
		kA3DTypeSurfCylinder,
		kA3DTypeSurfCylindrical,
		//kA3DTypeSurfOffset,
		//kA3DTypeSurfPipe,
		kA3DTypeSurfPlane,
		kA3DTypeSurfRuled,
		kA3DTypeSurfSphere,
		//kA3DTypeSurfRevolution,
		kA3DTypeSurfExtrusion,
		//kA3DTypeSurfFromCurves,
		kA3DTypeSurfTorus,
		kA3DTypeSurfTransform,
		//kA3DTypeSurfBlend04,
	};

	const A3DUns32 NUM_VALID_CURVES = 13;
	A3DUns32 ACCEPTED_CURVES[NUM_VALID_CURVES] = {
		//kA3DTypeCrvBase,
		//kA3DTypeCrvBlend02Boundary,
		kA3DTypeCrvNurbs,
		kA3DTypeCrvCircle,
		kA3DTypeCrvComposite,
		kA3DTypeCrvOnSurf,
		kA3DTypeCrvEllipse,
		kA3DTypeCrvEquation,
		kA3DTypeCrvHelix,
		kA3DTypeCrvHyperbola,
		//kA3DTypeCrvIntersection,
		kA3DTypeCrvLine,
		kA3DTypeCrvOffset,
		kA3DTypeCrvParabola,
		kA3DTypeCrvPolyLine,
		kA3DTypeCrvTransform,
	};

	A3DCopyAndAdaptBrepModelData sCopyAndAdaptBrepModelData;
	A3D_INITIALIZE_DATA(A3DCopyAndAdaptBrepModelData, sCopyAndAdaptBrepModelData);

	sCopyAndAdaptBrepModelData.m_bUseSameParam = false;
	sCopyAndAdaptBrepModelData.m_dTol = 1.0E-4;
	sCopyAndAdaptBrepModelData.m_bDeleteCrossingUV = false;
	sCopyAndAdaptBrepModelData.m_bAllowUVCrossingSeams = false;
	sCopyAndAdaptBrepModelData.m_bSplitFaces = false;
	sCopyAndAdaptBrepModelData.m_bSplitClosedFaces = false;
	sCopyAndAdaptBrepModelData.m_bForceComputeUV = false;
	sCopyAndAdaptBrepModelData.m_bForceCompute3D = true;
	sCopyAndAdaptBrepModelData.m_bContinueOnError = true;
	sCopyAndAdaptBrepModelData.m_uiAcceptableSurfacesSize = NUM_VALID_SURFACES;
	sCopyAndAdaptBrepModelData.m_puiAcceptableSurfaces = &ACCEPTED_SURFACES[0];
	sCopyAndAdaptBrepModelData.m_uiAcceptableCurvesSize = NUM_VALID_CURVES;
	sCopyAndAdaptBrepModelData.m_puiAcceptableCurves = &ACCEPTED_CURVES[0];

	A3DUns32 uiNbErrors = 0;
	A3DCopyAndAdaptBrepModelErrorData *pErrors;

	status = A3DAdaptAndReplaceAllBrepInModelFileAdvanced(pModelFile, &sCopyAndAdaptBrepModelData, &uiNbErrors, &pErrors);

}
