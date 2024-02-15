#include "ClassifyEntityVisitor.h"



ClassifyEntityVisitor::ClassifyEntityVisitor(A3DVisitorContainer* psContainer, const std::string entityType, const bool simplify, const double tol, const int bodyId, const int entityId)
	: A3DTreeVisitor(psContainer), m_sEntityType(entityType), m_bSimplify(simplify), m_dTol(tol), m_iTargetBodtId(bodyId), m_iTargetEntityId(entityId)
{
}


ClassifyEntityVisitor::~ClassifyEntityVisitor()
{
	if (0 < m_vEntityType.size())
		std::vector<float>().swap(m_vEntityType);
}

A3DStatus ClassifyEntityVisitor::visitEnter(const A3DRiConnector& sConnector)
{
	A3DStatus iRet = A3DTreeVisitor::visitEnter(sConnector);

	const A3DEntity* pEntity = sConnector.GetA3DEntity();
	A3DRiRepresentationItem* pRi = (A3DRiRepresentationItem*)pEntity;

	A3DEEntityType eType = kA3DTypeUnknown;
	CHECK_RET(A3DEntityGetType(pRi, &eType));

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
								char *cData = sAttrData.m_asSingleAttributesData[j].m_pcData;
								iBodyId = atoi(cData);
								break;
							}

						}
					}
					if (0 <= iBodyId)
						break;
				}
			}
		}
		CHECK_RET(A3DRootBaseGet(NULL, &sBaseData));

		m_iFaceId = 0;
		m_iEdgeId = 0;

		m_bIsTargetBody = false;
		if (iBodyId == m_iTargetBodtId)
			m_bIsTargetBody = true;

		if (-1 == m_iTargetBodtId)
		{
			m_vEntityType.push_back(iBodyId);
			m_iEntitySizePnt = m_vEntityType.size();
			m_vEntityType.push_back(0.0);
		}
	}
	return iRet;
}

A3DStatus ClassifyEntityVisitor::visitEnter(const A3DFaceConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	if ("FACE" != m_sEntityType)
		return iRet;

	if (-1 < m_iTargetBodtId && !m_bIsTargetBody)
		return iRet;

	if (-1 < m_iTargetEntityId && m_iTargetEntityId != m_iFaceId)
		return iRet;

	const A3DEntity *pEntity = sConnector.GetA3DEntity();
	A3DTopoFace *pFace = (A3DTopoFace*)pEntity;

	A3DTopoFaceData sData = sConnector.m_sFaceData;

	A3DSurfBase *surface = sData.m_pSurface;

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

	A3DSurfBase *surfaceSimp = NULL;
	if (m_bSimplify)
	{
		A3DEAnalyticType analiticType;
		CHECK_RET(A3DSimplifySurfaceWithAnalytics(surface, m_dTol, NUM_VALID_SURFACES, ACCEPTED_SURFACES, &surfaceSimp, &analiticType));
	}

	A3DEEntityType entityType;
	if (NULL == surfaceSimp)
		A3DEntityGetType(surface, &entityType);
	else
		A3DEntityGetType(surfaceSimp, &entityType);

	if (-1 == m_iTargetBodtId)
	{
		if (kA3DTypeSurfPlane == entityType)
			m_vEntityType.push_back(0);
		else if (kA3DTypeSurfCylinder == entityType)
			m_vEntityType.push_back(1);
		else if (kA3DTypeSurfCone == entityType)
			m_vEntityType.push_back(2);
		else if (kA3DTypeSurfTorus == entityType)
			m_vEntityType.push_back(3);
		else if (kA3DTypeSurfSphere == entityType)
			m_vEntityType.push_back(4);
		else
			m_vEntityType.push_back(5);

		m_vEntityType[m_iEntitySizePnt]++;
	}
	else
	{
		if (kA3DTypeSurfPlane == entityType)
			m_vEntityType.push_back(0);
		else if (kA3DTypeSurfCylinder == entityType)
		{
			m_vEntityType.push_back(1);

			A3DSurfCylinderData sData;
			A3D_INITIALIZE_DATA(A3DSurfCylinderData, sData);
			if (NULL == surfaceSimp)
				A3DSurfCylinderGet(surface, &sData);
			else
				A3DSurfCylinderGet(surfaceSimp, &sData);
			double dRadius = sData.m_dRadius;
			m_vEntityType.push_back(dRadius);
		}
		else if (kA3DTypeSurfCone == entityType)
			m_vEntityType.push_back(2);
		else if (kA3DTypeSurfTorus == entityType)
			m_vEntityType.push_back(3);
		else if (kA3DTypeSurfSphere == entityType)
		{
			m_vEntityType.push_back(4);
			A3DSurfSphereData sData;
			A3D_INITIALIZE_DATA(A3DSurfSphereData, sData);
			if (NULL == surfaceSimp)
				A3DSurfSphereGet(surface, &sData);
			else
				A3DSurfSphereGet(surfaceSimp, &sData);
			double dRadius = sData.m_dRadius;
			m_vEntityType.push_back(dRadius);
		}
		else
			m_vEntityType.push_back(5);
	}

	return iRet;
}

A3DStatus ClassifyEntityVisitor::visitEnter(const A3DEdgeConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	if ("EDGE" != m_sEntityType)
		return iRet;

	if (-1 < m_iTargetBodtId && !m_bIsTargetBody)
		return iRet;

	if (-1 < m_iTargetEntityId && m_iTargetEntityId != m_iEdgeId)
		return iRet;

	const A3DEntity* pEntity = sConnector.GetA3DEntity();
	A3DTopoEdge* pEdge = (A3DTopoEdge*)pEntity;

	A3DTopoEdgeData sData = sConnector.m_sEdgeData;

	A3DCrvBase* curve = sData.m_p3dCurve;

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

	A3DEEntityType entityType;
	if (NULL == curveSimp)
		A3DEntityGetType(curve, &entityType);
	else
		A3DEntityGetType(curveSimp, &entityType);

	if (-1 == m_iTargetBodtId)
	{
		if (kA3DTypeCrvLine == entityType)
			m_vEntityType.push_back(0);
		else if (kA3DTypeCrvCircle == entityType)
			m_vEntityType.push_back(1);
		else if (kA3DTypeCrvEllipse == entityType)
			m_vEntityType.push_back(2);
		else
			m_vEntityType.push_back(3);

		m_vEntityType[m_iEntitySizePnt]++;
	}
	else
	{
		if (kA3DTypeCrvLine == entityType)
			m_vEntityType.push_back(0);
		else if (kA3DTypeCrvCircle == entityType)
		{
			m_vEntityType.push_back(1);
		
			A3DCrvCircleData sData;
			A3D_INITIALIZE_DATA(A3DCrvCircleData, sData);
			if (NULL == curveSimp)
				A3DCrvCircleGet(curve, &sData);
			else
				A3DCrvCircleGet(curveSimp, &sData);
			double dRadius = sData.m_dRadius;
			m_vEntityType.push_back(dRadius);
		}
		else if (kA3DTypeCrvEllipse == entityType)
			m_vEntityType.push_back(2);
		else
			m_vEntityType.push_back(3);
	}
	return iRet;
}

A3DStatus ClassifyEntityVisitor::visitLeave(const A3DEdgeConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	m_iEdgeId++;

	return iRet;
}

A3DStatus ClassifyEntityVisitor::visitLeave(const A3DFaceConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	m_iFaceId++;

	return iRet;
}
