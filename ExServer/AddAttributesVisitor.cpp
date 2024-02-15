#include "AddAttributesVisitor.h"
#include <vector>
#include <string>

//######################################################################################################################
A3DStatus addEntityAttributes(A3DEntity* pEntity, A3DUns32 uiAttributeSize, A3DMiscAttribute** ppAttributes)
{
	A3DStatus iRet = A3D_SUCCESS;

	if (pEntity != NULL)
	{
		A3DRootBaseData sBaseData;
		A3D_INITIALIZE_DATA(A3DRootBaseData, sBaseData);
		CHECK_RET(A3DRootBaseGet(pEntity, &sBaseData));

		A3DMiscAttribute** ppAttr = new A3DMiscAttribute*[sBaseData.m_uiSize + uiAttributeSize];
		// copy old attributes
		for (A3DUns32 i = 0; i < sBaseData.m_uiSize; ++i) ppAttr[i] = sBaseData.m_ppAttributes[i];
		// add the new ones
		for (A3DUns32 i = 0; i < uiAttributeSize; ++i) ppAttr[i + sBaseData.m_uiSize] = ppAttributes[i];
		// replace the array
		sBaseData.m_uiSize = sBaseData.m_uiSize + uiAttributeSize;
		sBaseData.m_ppAttributes = ppAttr;

		CHECK_RET(A3DRootBaseSet(pEntity, &sBaseData));
		delete[] ppAttr;
	}

	return iRet;
}

A3DStatus addMyAttribute(A3DEntity* pEntity, const std::vector<std::string> attrNames, const std::vector<std::string> attrVals)
{
	A3DStatus iRet = A3D_SUCCESS;

	size_t iAttrCnt = attrNames.size();
	A3DMiscSingleAttributeData *psSingleAttributeData = new A3DMiscSingleAttributeData[iAttrCnt];

	for (size_t i = 0; i < iAttrCnt; i++)
	{
		A3D_INITIALIZE_DATA(A3DMiscSingleAttributeData, psSingleAttributeData[i]);
		psSingleAttributeData[i].m_eType = A3DEModellerAttributeType::kA3DModellerAttributeTypeString;
		psSingleAttributeData[i].m_pcTitle = (A3DUTF8Char*)attrNames[i].c_str();
		psSingleAttributeData[i].m_pcData = (A3DUTF8Char*)attrVals[i].c_str();
	}

	A3DMiscAttribute* pAttribute;
	A3DMiscAttributeData sAttributeData;
	A3D_INITIALIZE_DATA(A3DMiscAttributeData, sAttributeData);
	sAttributeData.m_pcTitle = (A3DUTF8Char*)"myAttributes";
	sAttributeData.m_uiSize = iAttrCnt;
	sAttributeData.m_asSingleAttributesData = psSingleAttributeData;
	CHECK_RET(A3DMiscAttributeCreate(&sAttributeData, &pAttribute));

	CHECK_RET(addEntityAttributes(pEntity, 1, &pAttribute));

	return iRet;
}

AddAttributesVisitor::AddAttributesVisitor(A3DVisitorContainer* psContainer, const bool addEntityIds)
	: A3DTreeVisitor(psContainer), m_bAddEntityIds(addEntityIds)
{
	m_iPartCnt = 0;
	m_iBodyCnt = 0;
	m_iFaceCnt = 0;
	m_iLoopCnt = 0;
	m_iEdgeCnt = 0;
}


AddAttributesVisitor::~AddAttributesVisitor()
{
}

A3DStatus AddAttributesVisitor::visitEnter(const A3DPartConnector& sConnector)
{
	A3DStatus iRet = A3DTreeVisitor::visitEnter(sConnector);

	m_iPartCnt++;

	return iRet;
}

A3DStatus AddAttributesVisitor::visitEnter(const A3DBrepDataConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	A3DTopoBrepData* topoBrepData = (A3DTopoBrepData*)sConnector.GetA3DEntity();

	// To compute the area for a single face, you’ll need a TopoFace object and its context
	A3D_INITIALIZE_DATA(A3DTopoBodyData, m_topoBodyData);
	A3DTopoBodyGet(topoBrepData, &m_topoBodyData);

	std::vector<bool>().swap(m_abShellIsClosed);
	m_iFaceId = 0;
	m_iEdgeId = 0;

	return iRet;
}

A3DStatus AddAttributesVisitor::visitEnter(const A3DShellConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;
	A3DTopoShellData sData = sConnector.m_sShellData;

	// Get whether shell is closed
	m_abShellIsClosed.push_back(sData.m_bClosed);

	return iRet;
}

A3DStatus AddAttributesVisitor::visitEnter(const A3DFaceConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	m_iFaceCnt++;

	return iRet;
}

A3DStatus AddAttributesVisitor::visitEnter(const A3DLoopConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;
	A3DTopoLoopData sData = sConnector.m_sLoopData;

	m_iLoopCnt++;

	return iRet;
}

A3DStatus AddAttributesVisitor::visitEnter(const A3DEdgeConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	m_iEdgeCnt++;

	return iRet;
}

A3DStatus AddAttributesVisitor::visitLeave(const A3DEdgeConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	A3DTopoEdge* pTopoEdge = (A3DTopoEdge*)sConnector.GetA3DEntity();

	A3DTopoEdgeData sData = sConnector.m_sEdgeData;

	A3DCrvBase* curve = sData.m_p3dCurve;

	std::vector<std::string> attrNames;
	std::vector<std::string> attrVals;

	if (curve)
	{
		// Edge langth
		double dLength = 0;
		CHECK_RET(A3DCurveLength(curve, nullptr, &dLength));
		char attData[256];
		sprintf(attData, "%f", dLength);

		attrNames.push_back("Edge Length");
		attrVals.push_back(attData);
	}

	if (m_bAddEntityIds)
	{
		char attData[256];
		sprintf(attData, "%d", m_iEdgeId++);

		attrNames.push_back("Edge ID");
		attrVals.push_back(attData);
	}

	// Add attribute
	if (attrNames.size())
	{
		CHECK_RET(addMyAttribute((A3DEntity*)pTopoEdge, attrNames, attrVals));
	}

	return iRet;
}

A3DStatus AddAttributesVisitor::visitLeave(const A3DFaceConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	A3DTopoFace* pTopoFace = (A3DTopoFace*)sConnector.GetA3DEntity();

	// Compute face area
	double dArea = 0;
	CHECK_RET(A3DComputeFaceArea(pTopoFace, m_topoBodyData.m_pContext, &dArea));

	// Add attribute
	std::vector<std::string> attrNames;
	std::vector<std::string> attrVals;

	{
		char attData[256];
		sprintf(attData, "%f", dArea);
		attrNames.push_back("Face Area");
		attrVals.push_back(attData);
	}

	if (m_bAddEntityIds)
	{
		char attData[256];
		sprintf(attData, "%d", m_iFaceId++);

		attrNames.push_back("Face ID");
		attrVals.push_back(attData);
	}

	CHECK_RET(addMyAttribute((A3DEntity*)pTopoFace, attrNames, attrVals));

	return iRet;
}

A3DStatus AddAttributesVisitor::visitLeave(const A3DBrepDataConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	A3DTopoBodyGet(NULL, &m_topoBodyData);

	return iRet;
}

A3DStatus AddAttributesVisitor::visitLeave(const A3DRiConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;
	
	A3DRiRepresentationItem* pRi = (A3DRiRepresentationItem*)sConnector.GetA3DEntity();

	A3DEEntityType eType = kA3DTypeUnknown;
	iRet = A3DEntityGetType(pRi, &eType);

	if (kA3DTypeRiBrepModel == eType)
	{
		// Body ID
		char attData[256];
		sprintf(attData, "%d", m_iBodyCnt++);

		// Add attribute
		std::vector<std::string> attrNames;
		std::vector<std::string> attrVals;

		attrNames.push_back("Body ID");
		attrVals.push_back(attData);

		// Whether dody is closed
		if (0 < m_abShellIsClosed.size())
		{
			char closedData[256] = { "\0" };
			for (size_t i = 0; i < m_abShellIsClosed.size(); i++)
			{
				if (0 < i)
					strcat(closedData, ", ");

				if (m_abShellIsClosed[i])
					strcat(closedData, "Solid");
				else
					strcat(closedData, "Surface");

			}
			attrNames.push_back("Is closed");
			attrVals.push_back(closedData);
		}

		CHECK_RET(addMyAttribute((A3DEntity*)pRi, attrNames, attrVals));
	}

	iRet = A3DTreeVisitor::visitLeave(sConnector);
	return iRet;
}

A3DStatus AddAttributesVisitor::visitLeave(const A3DPartConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	A3DAsmPartDefinitionData sData = sConnector.m_sPartData;

	// Set attribute to the father PO
	A3DAsmProductOccurrence* pParentPo = (A3DAsmProductOccurrence*)sConnector.GetProductOccurrenceFather();

	// Body count
	char attData[256];
	sprintf(attData, "%d", sData.m_uiRepItemsSize);

	// Add attribute
	std::vector<std::string> attrNames;
	std::vector<std::string> attrVals;

	attrNames.push_back("Body count");
	attrVals.push_back(attData);

	CHECK_RET(addMyAttribute((A3DEntity*)pParentPo, attrNames, attrVals));

	iRet = A3DTreeVisitor::visitLeave(sConnector);

	return iRet;
}