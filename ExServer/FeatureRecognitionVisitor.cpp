#include "FeatureRecognitionVisitor.h"
#include <iterator>
#include <algorithm>

FeatureRecognitionVisitor::FeatureRecognitionVisitor(A3DVisitorContainer* psContainer, const bool simplify, const double tol)
	: A3DTreeVisitor(psContainer),m_bSimplify(simplify), m_dTol(tol)
{
}

FeatureRecognitionVisitor::~FeatureRecognitionVisitor()
{
	m_mCirEdgeCylFace.clear();
}

A3DStatus FeatureRecognitionVisitor::visitEnter(const A3DRiConnector& sConnector)
{
	A3DStatus iRet = A3DTreeVisitor::visitEnter(sConnector);

	const A3DEntity* pEntity = sConnector.GetA3DEntity();
	A3DRiRepresentationItem* pRi = (A3DRiRepresentationItem*)pEntity;

	A3DEEntityType eType = kA3DTypeUnknown;
	CHECK_RET(A3DEntityGetType(pRi, &eType));

	m_iCurrentBodyId = -1;

	if (kA3DTypeRiBrepModel == eType)
	{
		// Find Body ID attribute
		A3DRootBaseData sBaseData;
		A3D_INITIALIZE_DATA(A3DRootBaseData, sBaseData);
		CHECK_RET(A3DRootBaseGet(pRi, &sBaseData));

		int iBodyId = -1;
		for (A3DUns32 ui = 0; ui < sBaseData.m_uiSize; ++ui)
		{
			A3DMiscAttributeData sAttrData;
			A3D_INITIALIZE_DATA(A3DMiscAttributeData, sAttrData);
			CHECK_RET(A3DMiscAttributeGet(sBaseData.m_ppAttributes[ui], &sAttrData));

			if (!sAttrData.m_bTitleIsInt)
			{
				if (0 == strcmp("myAttributes", sAttrData.m_pcTitle))
				{
					for (A3DUns32 j = 0; j < sBaseData.m_uiSize; ++j)
					{
						if (kA3DModellerAttributeTypeString == sAttrData.m_asSingleAttributesData[j].m_eType)
						{
							if (0 == strcmp("Body ID", sAttrData.m_asSingleAttributesData[j].m_pcTitle))
							{
								char* cData = sAttrData.m_asSingleAttributesData[j].m_pcData;
								iBodyId = atoi(cData);
								break;
							}

						}
					}
					if (0 <= iBodyId)
					{
						CHECK_RET(A3DMiscAttributeGet(NULL, &sAttrData));
						break;
					}
				}
			}
			CHECK_RET(A3DMiscAttributeGet(NULL, &sAttrData));
		}
		CHECK_RET(A3DRootBaseGet(NULL, &sBaseData));

		m_iFaceId = 0;

		BodyRoLoops bodyRoLoops;
		bodyRoLoops.bodyId = iBodyId;
		m_vBodyRoLoops.push_back(bodyRoLoops);
		m_iCurrentBodyId = int(m_vBodyRoLoops.size() - 1);
	}

	return iRet;
}

A3DStatus FeatureRecognitionVisitor::visitEnter(const A3DFaceConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	if (-1 == m_iCurrentBodyId)
		return iRet;

	const A3DEntity* pEntity = sConnector.GetA3DEntity();
	A3DTopoFace* pFace = (A3DTopoFace*)pEntity;

	A3DTopoFaceData sData = sConnector.m_sFaceData;

	A3DSurfBase* surface = sData.m_pSurface;

	const A3DUns32 NUM_VALID_SURFACES = 6;
	A3DEEntityType ACCEPTED_SURFACES[NUM_VALID_SURFACES] = {
		//kA3DTypeSurfBlend01,
		//kA3DTypeSurfBlend02,
		//kA3DTypeSurfBlend03,
		kA3DTypeSurfNurbs,
		kA3DTypeSurfCone,
		kA3DTypeSurfCylinder,
		//kA3DTypeSurfCylindrical,
		//kA3DTypeSurfOffset,
		//kA3DTypeSurfPipe,
		kA3DTypeSurfPlane,
		//kA3DTypeSurfRuled,
		kA3DTypeSurfSphere,
		//kA3DTypeSurfRevolution,
		//kA3DTypeSurfExtrusion,
		//kA3DTypeSurfFromCurves,
		kA3DTypeSurfTorus,
		//kA3DTypeSurfTransform,
		//kA3DTypeSurfBlend04,
	};

	// Get face type
	A3DSurfBase* surfaceSimp = NULL;
	if (m_bSimplify)
	{
		A3DEAnalyticType analiticType;
		CHECK_RET(A3DSimplifySurfaceWithAnalytics(surface, m_dTol, NUM_VALID_SURFACES, ACCEPTED_SURFACES, &surfaceSimp, &analiticType));
	}

	if (NULL == surfaceSimp)
		A3DEntityGetType(surface, &m_eCrrentFaceType);
	else
		A3DEntityGetType(surfaceSimp, &m_eCrrentFaceType);

	return iRet;
}

A3DStatus FeatureRecognitionVisitor::visitEnter(const A3DLoopConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;
	
	if (-1 == m_iCurrentBodyId)
		return iRet;

	m_dLoopRadius = 0;

	return iRet;
}

A3DStatus FeatureRecognitionVisitor::visitEnter(const A3DCoEdgeConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	if (-1 == m_iCurrentBodyId)
		return iRet;

	if (kA3DTypeSurfPlane != m_eCrrentFaceType && kA3DTypeSurfCylinder != m_eCrrentFaceType)
		return iRet;

	A3DTopoCoEdgeData sData = sConnector.m_sCoEdgeData;
	A3DTopoEdge* pEdge = sData.m_pEdge;

	A3DTopoEdgeData sEdgeData;
	A3D_INITIALIZE_DATA(A3DTopoEdgeData, sEdgeData);
	iRet = A3DTopoEdgeGet(pEdge, &sEdgeData);

	A3DCrvBase* curve = sEdgeData.m_p3dCurve;

	const A3DUns32 NUM_VALID_CURVES = 13;
	A3DEEntityType ACCEPTED_CURVES[NUM_VALID_CURVES] = {
		//kA3DTypeCrvBase,
		//kA3DTypeCrvBlend02Boundary,
		kA3DTypeCrvNurbs,
		kA3DTypeCrvCircle,
		//kA3DTypeCrvComposite,
		//kA3DTypeCrvOnSurf,
		kA3DTypeCrvEllipse,
		//kA3DTypeCrvEquation,
		//kA3DTypeCrvHelix,
		//kA3DTypeCrvHyperbola,
		//kA3DTypeCrvIntersection,
		kA3DTypeCrvLine,
		//kA3DTypeCrvOffset,
		//kA3DTypeCrvParabola,
		//kA3DTypeCrvPolyLine,
		//kA3DTypeCrvTransform,
	};

	A3DCrvBase* curveSimp = NULL;
	if (m_bSimplify)
	{
		A3DEAnalyticType analiticType;
		CHECK_RET(A3DSimplifyCurveWithAnalytics(curve, m_dTol, NUM_VALID_CURVES, ACCEPTED_CURVES, &curveSimp, &analiticType));
	}

	A3DEEntityType eCoEdgeType;

	if (NULL == curveSimp)
		A3DEntityGetType(curve, &eCoEdgeType);
	else
		A3DEntityGetType(curveSimp, &eCoEdgeType);

	iRet = A3DTopoEdgeGet(NULL, &sEdgeData);

	if (kA3DTypeCrvCircle == eCoEdgeType)
	{
		// Update loop radious of loop in plane face
		if (kA3DTypeSurfPlane == m_eCrrentFaceType && -1 != m_dLoopRadius)
		{
			A3DCrvCircleData sCirData;
			A3D_INITIALIZE_DATA(A3DCrvCircleData, sCirData);
			if (NULL == curveSimp)
				A3DCrvCircleGet(curve, &sCirData);
			else
				A3DCrvCircleGet(curveSimp, &sCirData);
			double dRadius = sCirData.m_dRadius;

			A3DCrvCircleGet(NULL, &sCirData);

			if (0 == m_dLoopRadius)
				m_dLoopRadius = dRadius;
			else if (m_dTol < abs(m_dLoopRadius - dRadius))
				m_dLoopRadius = -1;
		}

		// Create Circle edge - owner cyliner face map
		if (kA3DTypeSurfCylinder == m_eCrrentFaceType)
		{
			m_mCirEdgeCylFace.insert(std::make_pair(pEdge, m_iFaceId));
		}
	}
	else
		m_dLoopRadius = -1;

	return iRet;
}

A3DStatus FeatureRecognitionVisitor::visitLeave(const A3DLoopConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;
	
	if (-1 == m_iCurrentBodyId)
		return iRet;

	const A3DEntity* pEntity = sConnector.GetA3DEntity();
	A3DTopoLoop* pLoop = (A3DTopoLoop*)pEntity;

	if (kA3DTypeSurfPlane == m_eCrrentFaceType && 0 < m_dLoopRadius)
	{
		m_vBodyRoLoops[m_iCurrentBodyId].vRoundLoops.push_back(pLoop);
		m_vBodyRoLoops[m_iCurrentBodyId].vRadius.push_back(m_dLoopRadius);
	}

	return iRet;
}

A3DStatus FeatureRecognitionVisitor::visitLeave(const A3DFaceConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	if (-1 == m_iCurrentBodyId)
		return iRet;

	m_iFaceId++;

	return iRet;
}

std::vector<float> FeatureRecognitionVisitor::getRoundHoleFaces()
{
	std::vector<float> roundHoleFaces;

	for (int i = 0; i < (int)m_vBodyRoLoops.size(); i++)
	{
		BodyRoLoops bodyRoLoops = m_vBodyRoLoops[i];

		roundHoleFaces.push_back(bodyRoLoops.bodyId);
		int iDataCntPtr = (int)roundHoleFaces.size();
		roundHoleFaces.push_back(0);

		std::vector<int> vRecognizedFaceIds;

		for (int j = 0; j < (int)bodyRoLoops.vRoundLoops.size(); j++)
		{
			A3DTopoLoop* pLoop = bodyRoLoops.vRoundLoops[j];
			float fRadius = bodyRoLoops.vRadius[j];

			// Get cylinder face ID of round hole
			std::vector<int> vCylFaceIds;

			A3DTopoLoopData sLoopData;
			A3D_INITIALIZE_DATA(A3DTopoLoopData, sLoopData);
			A3DTopoLoopGet(pLoop, &sLoopData);

			for (A3DUns32 k = 0; k < sLoopData.m_uiCoEdgeSize; k++)
			{
				A3DTopoCoEdgeData sCoEdgeData;
				A3D_INITIALIZE_DATA(A3DTopoCoEdgeData, sCoEdgeData);
				A3DTopoCoEdgeGet(sLoopData.m_ppCoEdges[k], &sCoEdgeData);
				if (1 == m_mCirEdgeCylFace.count(sCoEdgeData.m_pEdge))
				{
					int iFaceId = m_mCirEdgeCylFace[sCoEdgeData.m_pEdge];
					vCylFaceIds.push_back(iFaceId);
				}
				A3DTopoCoEdgeGet(NULL, &sCoEdgeData);

			}
			A3DTopoLoopGet(NULL, &sLoopData);

			if (0 < vCylFaceIds.size())
			{
				// Check whether face IDs are already detected
				bool bFlg = false;
				for (int k = 0; k < (int)vCylFaceIds.size(); k++)
				{
					auto itr = std::find(vRecognizedFaceIds.begin(), vRecognizedFaceIds.end(), vCylFaceIds[k]);
					if (itr != vRecognizedFaceIds.end())
					{
						bFlg = true;
						break;
					}
				}

				// Register round face IDs with radious value
				if (!bFlg)
				{
					roundHoleFaces.push_back((float)vCylFaceIds.size());
					roundHoleFaces.push_back(fRadius);

					std::copy(vCylFaceIds.begin(), vCylFaceIds.end(), std::back_inserter(roundHoleFaces));
					roundHoleFaces[iDataCntPtr] += float(vCylFaceIds.size() + 2);

					std::copy(vCylFaceIds.begin(), vCylFaceIds.end(), std::back_inserter(vRecognizedFaceIds));
				}
			}
		}
	}

	return roundHoleFaces;
}
