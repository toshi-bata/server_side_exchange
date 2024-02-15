/***********************************************************************************************************************
*
* Copyright (c) 2010 - 2023 by Tech Soft 3D, Inc.
* The information contained herein is confidential and proprietary to Tech Soft 3D, Inc., and considered a trade secret
* as defined under civil and criminal statutes. Tech Soft 3D shall pursue its civil and criminal remedies in the event
* of unauthorized use or misappropriation of its trade secrets. Use of this information by anyone other than authorized 
* employees of Tech Soft 3D, Inc. is granted only under a written non-disclosure agreement, expressly prescribing the 
* scope and manner of such use.
*
***********************************************************************************************************************/

#include "VisitorCascadedAttribute.h"
#include "CascadedAttributeConnector.h"
#include "VisitorTree.h"

A3DVisitorColorMaterials::A3DVisitorColorMaterials(A3DVisitorContainer* psContainer)
:	A3DVisitor("CascadedAttribute", psContainer),
	m_pCurrentRi(NULL)
{
}

A3DVisitorColorMaterials::~A3DVisitorColorMaterials()
{
	std::vector<A3DMiscCascadedAttributes*>::iterator itCur = m_apsCascadedAttribute.begin();
	std::vector<A3DMiscCascadedAttributes*>::iterator itEnd = m_apsCascadedAttribute.end();
	for(; itCur < itEnd; ++itCur)
	{
		A3DMiscCascadedAttributesDelete(*itCur);
	}
}

A3DStatus A3DVisitorColorMaterials::pushCascadedAttribute(const A3DConnector& sEntity)
{
	unsigned int uSize = (unsigned int)m_apsCascadedAttribute.size();
	A3DInt32 iRet = A3D_SUCCESS;
	if(!uSize)
	{
		A3DMiscCascadedAttributes* pAttr;
		iRet = A3DMiscCascadedAttributesCreate(&pAttr);
		m_apsCascadedAttribute.push_back(pAttr);
		uSize++;
	}
	
	A3DMiscCascadedAttributes* pAttr;
	iRet = A3DMiscCascadedAttributesCreate(&pAttr);
	iRet = A3DMiscCascadedAttributesPush(pAttr, sEntity.GetA3DEntity(), m_apsCascadedAttribute[uSize-1]);
	m_apsCascadedAttribute.push_back(pAttr);

	return A3D_SUCCESS;
}


A3DStatus A3DVisitorColorMaterials::popCascadedAttribute(/*const A3DConnector& sEntity*/)
{
	if(m_apsCascadedAttribute.size() > 0)
	{
    A3DMiscCascadedAttributesDelete(m_apsCascadedAttribute[m_apsCascadedAttribute.size() - 1]);
		m_apsCascadedAttribute.pop_back();
	}

	return A3D_SUCCESS;
}

// Visit Product Occurrence
A3DStatus A3DVisitorColorMaterials::visitEnter(const A3DProductOccurrenceConnector& sEntity)
{
	pushCascadedAttribute( (const A3DConnector&) sEntity);

	/*	A3DEEntityType eType;
		A3DEntityGetType( sEntity.m_sProductOccurrenceData.m_ppEntityReferences[uI], &eType);
		if(eType == kA3DTypeMiscEntityReference)
		{
			A3DMiscReferenceOnTopologyData  sTopoRefData;
			A3D_INITIALIZE_DATA(A3DMiscReferenceOnTopologyData,sTopoRefData);
			A3DMiscReferenceOnTopologyGet(sEntity.m_sProductOccurrenceData.m_ppEntityReferences[uI],
										  &sTopoRefData);
			// Get Entity Target: sTopoRefData.m_pBrepData, Edge, Face, Vertex
			//Add also systematically the 3DTessData or Face Tess Data 
			//add in the Visitor container Map
		
				
				// kA3DTypeTopoMultipleVertex
				// kA3DTypeTopoUniqueVertex
				// kA3DTypeTopoWireEdge
				// kA3DTypeTopoEdge
				// kA3DTypeTopoLoop
				// kA3DTypeTopoFace
				// kA3DTypeTopoShell
				// kA3DTypeTopoConnex

			A3DMiscReferenceOnTopologyGet(NUL,
										  &sTopoRefData));

		
			A3DTopoBrepData* pBrepData = sTopoRefData.m_pBrepData;
			A3DTopoBrepDataData sTopoBrepDataData;
			A3D_INITIALIZE_DATA(	A3DTopoBrepDataData,sTopoBrepDataData);
			A3DTopoBrepDataGet(pBrepData,&sTopoBrepDataData);
			unsigned int unbconnex = sTopoBrepDataData.m_uiConnexSize;
		}
	}*/

	return A3D_SUCCESS;
}

A3DStatus A3DVisitorColorMaterials::visitLeave(const A3DProductOccurrenceConnector& /*sEntity*/)
{
	popCascadedAttribute();
	return A3D_SUCCESS;
}

// Visit PartDefintion 
A3DStatus A3DVisitorColorMaterials::visitEnter(const A3DPartConnector& sEntity)
{
	pushCascadedAttribute((const A3DConnector& ) sEntity);
	return A3D_SUCCESS;
}

A3DStatus A3DVisitorColorMaterials::visitLeave(const A3DPartConnector& /*sEntity*/)
{
	popCascadedAttribute();
	return A3D_SUCCESS;
}

// Visit Representation Item
A3DStatus A3DVisitorColorMaterials::visitEnter(const A3DRiConnector& sRi)
{
	A3DStatus iRet = A3D_SUCCESS;
	m_pCurrentRi = (A3DRiRepresentationItem*)sRi.GetA3DEntity();
	CHECK_RET(pushCascadedAttribute((const A3DConnector&) sRi));
	CHECK_RET(SetGlobalAttributeOnLeaf(m_apsCascadedAttribute[ m_apsCascadedAttribute.size() - 1]));
	return iRet;
}

A3DStatus A3DVisitorColorMaterials::visitLeave(const A3DRiConnector& /*sRi*/)
{
	A3DStatus iRet = A3D_SUCCESS;
	m_pCurrentRi = NULL;
	CHECK_RET(popCascadedAttribute());
	return A3D_SUCCESS;
}

// Visit Face Tess Data
A3DStatus A3DVisitorColorMaterials::visitEnter(const A3DFaceTessDataConnector& sFaceTess)
{
	A3DStatus iRet = A3D_SUCCESS;
	A3DTessFaceData* pFaceTessData = (A3DTessFaceData*)sFaceTess.GetA3DEntity();

	if(m_pCurrentRi && !pFaceTessData->m_bIsRGBA && pFaceTessData->m_uiStyleIndexesSize == 1)
	{
		//CHECK_RET(pushCascadedAttribute((const A3DConnector&) sRi));
		//home made push for tess face
		A3DMiscCascadedAttributes* pAttr;
		iRet = A3DMiscCascadedAttributesCreate(&pAttr);
		iRet = A3DMiscCascadedAttributesPushTessFace(pAttr,
																	m_pCurrentRi,
																	(A3DTess3D*)sFaceTess.GetTessDataConnector()->GetA3DEntity(),
																	(A3DTessFaceData*)sFaceTess.GetA3DEntity(),
																	sFaceTess.GetFaceIndex(),
																	m_apsCascadedAttribute[m_apsCascadedAttribute.size()-1]);
		m_apsCascadedAttribute.push_back(pAttr);

		CHECK_RET(SetGlobalAttributeOnLeaf(m_apsCascadedAttribute[ m_apsCascadedAttribute.size() - 1]));
	}
	return iRet;
}

A3DStatus A3DVisitorColorMaterials::visitLeave(const A3DFaceTessDataConnector& sFaceTess)
{
	A3DStatus iRet = A3D_SUCCESS;
	A3DTessFaceData* pFaceTessData = (A3DTessFaceData*)sFaceTess.GetA3DEntity();
	if(!pFaceTessData->m_bIsRGBA && pFaceTessData->m_uiStyleIndexesSize == 1)
	{
		CHECK_RET(popCascadedAttribute());
	}
	return A3D_SUCCESS;
}

A3DMiscCascadedAttributes* A3DVisitorColorMaterials::GetLastCascadedAttributes() const
{
	size_t iSize = m_apsCascadedAttribute.size();
	if(iSize == 0)
		return NULL;
	return m_apsCascadedAttribute[iSize-1];
}

A3DStatus A3DVisitorColorMaterials::GetColorMaterialConnector( ColorMaterialsConnector& rsColorConnector,
																				 bool bUseInstances /*= false*/ )
{
	A3DStatus iErr = A3D_SUCCESS;
	size_t iSize = m_apsCascadedAttribute.size();
	if(iSize == 0)
		return A3D_ERROR;
	A3DMiscCascadedAttributes* pCurrent = m_apsCascadedAttribute[iSize-1];

	if(!bUseInstances)
	{
		A3DMiscEntityReference* pMER = NULL;
		A3DStepEntityRefManager const * pSERM = m_psContainer->GetActiveStepEntityRefManager(NULL);
		if (pSERM)
		{
			pMER = pSERM->m_pStepEntityRef;
		}

		A3DViewLinkedItemManager const* pVLIM = m_psContainer->GetActiveViewLinkedItemManager(NULL);
		if (pVLIM)
		{
			pMER = pVLIM->m_pMarkupLinkedItem;
		}

		if (pMER)
		{
			A3DMiscEntityReferenceData sSERData;
			A3D_INITIALIZE_DATA(A3DMiscEntityReferenceData, sSERData);
			iErr = A3DMiscEntityReferenceGet(pMER, &sSERData);
			if(iErr == A3D_SUCCESS)
			{
				A3DEEntityType eType;
				A3DEntityGetType( pMER, &eType);
				if(eType == kA3DTypeMiscEntityReference || eType==kA3DTypeMiscMarkupLinkedItem)
				{
					A3DMiscReferenceOnTopologyData  sTopoRefData;
					A3D_INITIALIZE_DATA(A3DMiscReferenceOnTopologyData,sTopoRefData);
					iErr = A3DMiscReferenceOnTopologyGet(pMER,	&sTopoRefData);
					if(iErr == A3D_SUCCESS) // topo
					{
						switch(sTopoRefData.m_eTopoItemType)
						{
						case kA3DTypeTopoMultipleVertex:
						case kA3DTypeTopoUniqueVertex:
						case kA3DTypeTopoWireEdge:
						case kA3DTypeTopoEdge:
						case kA3DTypeTopoLoop:
							break;
						case kA3DTypeTopoFace:
							{
								A3DTopoBrepData* pBrepData = sTopoRefData.m_pBrepData;
								A3DTopoBrepDataData sTopoBrepDataData;
								A3D_INITIALIZE_DATA(	A3DTopoBrepDataData,sTopoBrepDataData);
								A3DTopoBrepDataGet(pBrepData,&sTopoBrepDataData);

								A3DTopoBrepDataGet(NULL,&sTopoBrepDataData);

								break;
							}
						case kA3DTypeTopoShell:
							{
								break;
							}
						case kA3DTypeTopoConnex:
							{
								break;
							}
						default:
							{
								break;
							}
						}
					}
					else // no topo -> global PO/Part/RI/RIBrepModel
					{
						A3DEEntityType eLType;
						A3DEntityGetType( sSERData.m_pEntity, &eLType);

						//A3DGraphStyleData
						A3DMiscCascadedAttributes* pAttr = NULL;
						A3DMiscCascadedAttributesCreate(&pAttr);
						iErr = A3DMiscCascadedAttributesPush(pAttr, pMER, pCurrent);
						A3DMiscCascadedAttributesData sCAData;
						A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, sCAData);
						iErr = A3DMiscCascadedAttributesGet(pAttr, &sCAData);
						rsColorConnector = ColorMaterialsConnector(pAttr);
						A3DMiscCascadedAttributesDelete (pAttr);
					}

					A3DMiscReferenceOnTopologyGet(NULL,&sTopoRefData);
				}
				A3DMiscEntityReferenceGet( NULL, &sSERData);
			}
		}
		else
			rsColorConnector = ColorMaterialsConnector(pCurrent);
	}

	return A3D_SUCCESS;
}
