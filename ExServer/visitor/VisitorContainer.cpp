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

#include "VisitorContainer.h"
#include "Visitors.h"

#ifdef CONNECT_BREP
#include "VisitorBrep.h"
#endif
#ifdef CONNECT_TRANSFO
#include "VisitorTransfo.h"
#endif
#ifdef CONNECT_COLORS
#include "VisitorCascadedAttribute.h"
#endif
#ifdef CONNECT_ASSEMBLY_TREE
#include "VisitorTree.h"
#endif
#ifdef CONNECT_MESH
#include "VisitorTessellation.h"
#endif
#ifdef CONNECT_PMI
//#include <VisitorPMI.h>
#endif
#ifdef CONNECT_PMI
#include "ViewTraverse.h"
#endif

#include <map>
#include <algorithm>
#include <iostream>

using namespace std;

#define	 VISITENTER \
{\
	ActivateEntityReference(sConnector.GetA3DEntity()); \
	for(unsigned uI = 0; uI < m_apVisitor.size(); uI++)\
		m_apVisitor[uI]->visitEnter(sConnector);\
	return A3D_SUCCESS;\
}

#define	 VISITLEAVE \
{\
	DeactivateEntityReference(sConnector.GetA3DEntity()); \
	for(unsigned uI = 0; uI < m_apVisitor.size(); uI++)\
		m_apVisitor[uI]->visitLeave(sConnector);\
	return A3D_SUCCESS;\
}

//Tree
#ifdef CONNECT_ASSEMBLY_TREE

// static A3DStatus stCreateAndPushCascadedAttributes(	const A3DRootBaseWithGraphics* pBase,
// 																	const A3DMiscCascadedAttributes* pFatherAttr,
// 																	A3DMiscCascadedAttributes** ppAttr,
// 																	A3DMiscCascadedAttributesData* psAttrData)
// {
// 	A3DStatus iRet = A3D_SUCCESS;
//
// 	CHECK_RET(A3DMiscCascadedAttributesCreate(ppAttr));
// 	CHECK_RET(A3DMiscCascadedAttributesPush(*ppAttr, pBase, pFatherAttr));
//
// 	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, (*psAttrData));
// 	CHECK_RET(A3DMiscCascadedAttributesGet(*ppAttr, psAttrData));
//
// 	return iRet;
// }

A3DStatus A3DVisitorContainer::visitEnter(const A3DModelFileConnector& sConnector)
VISITENTER

A3DStatus A3DVisitorContainer::visitEnter(const A3DProductOccurrenceConnector& sConnector)
{
	m_uiCurrentLevel++;

	// Entity Reference
	unsigned int uI, uNbRefEntity = sConnector.m_sProductOccurrenceData.m_uiEntityReferenceSize;
	for(uI = 0; uI < uNbRefEntity; uI++)
	{
		A3DMiscEntityReferenceData sMiscRefData;
		A3D_INITIALIZE_DATA(A3DMiscEntityReferenceData, sMiscRefData);
		A3DMiscEntityReference* pEntityRef = sConnector.m_sProductOccurrenceData.m_ppEntityReferences[uI];
		A3DMiscEntityReferenceGet (pEntityRef, &sMiscRefData);
		A3DStepEntityRefManager sEntityManager;
		sEntityManager.m_pStepEntityRef = pEntityRef;
		sEntityManager.m_uiPOLevel = m_uiCurrentLevel;

		A3DEEntityType sType;
		A3DEntityGetType (sMiscRefData.m_pEntity, &sType);
		if(sType ==  kA3DTypeMiscReferenceOnTopology)
		{
			A3DMiscReferenceOnTopologyData  sTopoRefData;
			A3D_INITIALIZE_DATA(A3DMiscReferenceOnTopologyData,sTopoRefData);
			A3DMiscReferenceOnTopologyGet (pEntityRef, &sTopoRefData);
			sEntityManager.m_pA3DEntityRef = sTopoRefData.m_pBrepData;
			sEntityManager.m_bRefOnTopoItem = true;
			A3DMiscReferenceOnTopologyGet (NULL, &sTopoRefData);
		}
		else if(sType ==  kA3DTypeMiscReferenceOnCsysItem)
		{
			A3DMiscReferenceOnCsysItemData sA3DMiscReferenceOnCSYSITemData;
			A3D_INITIALIZE_DATA(A3DMiscReferenceOnCsysItemData, sA3DMiscReferenceOnCSYSITemData);
			A3DMiscReferenceOnCsysItemGet (pEntityRef, &sA3DMiscReferenceOnCSYSITemData);
			sEntityManager.m_pA3DEntityRef = sA3DMiscReferenceOnCSYSITemData.m_pCoordinateSystem;
			A3DMiscReferenceOnCsysItemGet (NULL, &sA3DMiscReferenceOnCSYSITemData);
		}
		else
			sEntityManager.m_pA3DEntityRef = sMiscRefData.m_pEntity;

		A3DMiscEntityReferenceGet (NULL, &sMiscRefData);

		m_asStepEntityRefManager.push_back(sEntityManager);
	}

	A3DStatus iRet = A3D_SUCCESS;

	// Active View ...
	if (m_psActivatedView)
	{
		A3DUns32 uNbView = sConnector.m_sProductOccurrenceData.m_uiViewsSize;
		for (uI = 0; uI < uNbView; uI++)
		{
			if (sConnector.m_sProductOccurrenceData.m_ppViews[uI] != m_psActivatedView)
				continue;

			A3DMkpViewConnector sMkpViewConnector(sConnector.m_sProductOccurrenceData.m_ppViews[uI]);
			sMkpViewConnector.TraverseView(this);
		}
	}

	ActivateEntityReference(sConnector.GetA3DEntity());
	for( uI = 0; uI < m_apVisitor.size(); uI++)
	{
		m_apVisitor[uI]->visitEnter(sConnector);
	}

	return iRet;
}

A3DStatus A3DVisitorContainer::visitEnter(const A3DPartConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DRiConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DRiBrepModelConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DRiSetConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DPolyRiBrepModelConnector& sConnector)
VISITENTER


A3DStatus A3DVisitorContainer::visitLeave(const A3DModelFileConnector& sConnector)
VISITLEAVE

A3DStatus A3DVisitorContainer::visitLeave(const A3DProductOccurrenceConnector& sConnector)
{
	for(unsigned uI = 0; uI < m_apVisitor.size(); uI++)
	{
		m_apVisitor[uI]->visitLeave(sConnector);
	}

	unsigned int uI, uNbRefEntity = sConnector.m_sProductOccurrenceData.m_uiEntityReferenceSize;
	for (uI = 0; uI < uNbRefEntity; uI++)
	{
		m_asStepEntityRefManager.pop_back();
	}

	if (m_psActivatedView != NULL)
	{
		A3DUns32 uNbView = sConnector.m_sProductOccurrenceData.m_uiViewsSize;
		for (uI = 0; uI < uNbView; uI++)
		{
			if (sConnector.m_sProductOccurrenceData.m_ppViews[uI]!=m_psActivatedView)
				continue;

			A3DMkpViewConnector sViewConnector(m_psActivatedView);
			unsigned int uJ;
			uNbRefEntity = sViewConnector.GetViewData().m_uiLinkedItemsSize;
			for (uJ = 0; uJ < uNbRefEntity; uJ++)
			{
				m_asViewLinkedItemManager.pop_back();
			}
		}
	}

	DeactivateEntityReference(sConnector.GetA3DEntity());

	m_uiCurrentLevel--;

	return A3D_SUCCESS;
}

A3DStatus A3DVisitorContainer::visitLeave(const A3DPartConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DRiConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DRiBrepModelConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DRiSetConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DPolyRiBrepModelConnector& sConnector)
VISITLEAVE
#endif

//Markup
#ifdef CONNECT_PMI
A3DStatus A3DVisitorContainer::visitEnter(const A3DMkpAnnotationEntityConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DMkpAnnotationSetConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DMkpAnnotationItemConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DMarkupConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DMarkupDimensionConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DMarkupGDTConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DMarkupDatumConnector& sConnector)
VISITENTER

A3DStatus A3DVisitorContainer::visitLeave(const A3DMkpAnnotationEntityConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DMkpAnnotationSetConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DMkpAnnotationItemConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DMarkupDimensionConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DMarkupConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DMarkupGDTConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DMarkupDatumConnector& sConnector)
VISITLEAVE
#endif

//Brep
#ifdef CONNECT_BREP
A3DStatus A3DVisitorContainer::visitEnter(const A3DBrepDataConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DConnexConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DShellConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DFaceConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DLoopConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DCoEdgeConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DEdgeConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DUniqueVertexConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitEnter(const A3DMultipleVertexConnector& sConnector)
VISITENTER

A3DStatus A3DVisitorContainer::visitLeave(const A3DBrepDataConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DConnexConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DShellConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DFaceConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DLoopConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DCoEdgeConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DEdgeConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DUniqueVertexConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitLeave(const A3DMultipleVertexConnector& sConnector)
VISITLEAVE
#endif

//Mesh
#ifdef CONNECT_MESH
A3DStatus A3DVisitorContainer::visitEnter(const A3DTessDataConnector& sConnector)
VISITENTER

A3DStatus A3DVisitorContainer::visitLeave(const A3DTessDataConnector& sConnector)
VISITLEAVE

A3DStatus A3DVisitorContainer::visitEnter(const A3DFaceTessDataConnector& sConnector)
VISITENTER

A3DStatus A3DVisitorContainer::visitLeave(const A3DFaceTessDataConnector& sConnector)
VISITLEAVE

//VISIT(const	A3DTessDataConnector&)
#endif

A3DVisitorContainer::A3DVisitorContainer(unsigned int uFlagElementToconnect)
:	A3DVisitor("Container"),
	m_pTreeVisitor(NULL),
	m_pCurrentPOFather(NULL),
	m_psActivatedView(NULL),
	m_bTraverseActivatedViewOnly(true),
	m_uiCurrentLevel(0),
	m_uFlagElementToConnect(uFlagElementToconnect),
	m_bTraverseInstance(false)
{
#ifdef CONNECT_TRANSFO
	if(uFlagElementToconnect&CONNECT_TRANSFO)
	{
		A3DVisitorTransfo* psVisistorTransfo = new A3DVisitorTransfo(this);
		this->push(	psVisistorTransfo);
	}
#endif
#ifdef CONNECT_COLORS
	if(uFlagElementToconnect&CONNECT_COLORS)
	{
		A3DVisitorColorMaterials* psVisitorcascadedAttribute = new A3DVisitorColorMaterials(this);
		this->push(	psVisitorcascadedAttribute);
	}
#endif
#ifdef	CONNECT_MESH
	if(uFlagElementToconnect&CONNECT_MESH)
	{
		A3DVisitorTessellation* psVisitorTessellation = new A3DVisitorTessellation(this);
		this->push(	psVisitorTessellation);
	}
#endif
#ifdef CONNECT_BREP
   	if(uFlagElementToconnect&CONNECT_BREP)
	{
		A3DVisitorBrep* psVisitorBrep = new A3DVisitorBrep(this);
		this->push(	psVisitorBrep);
	}
#endif
#ifdef CONNECT_ASSEMBLY_TREE
	if(uFlagElementToconnect&CONNECT_ASSEMBLY_TREE)
	{
		A3DTreeVisitor* psVisitorTree= new A3DTreeVisitor(this);
		this->push(	psVisitorTree);
	}
#endif
#ifdef	CONNECT_PMI
	if(uFlagElementToconnect&CONNECT_PMI)
	{
		//A3DVisitorPMI* psVisitorPMI = new A3DVisitorPMI(this);
		//this->push(	psVisitorPMI );
	}
#endif
#ifdef	CONNECT_VIEWS
 	if(uFlagElementToconnect&CONNECT_VIEWS)
	{
	// Not yet Implemented
	//	A3DVisitorview* psVisitorViews = new A3DVisitorview(this);
	//	this->push(	psVisitorViews);
	}
#endif
}

#ifdef CONNECT_VIEWS

A3DStatus A3DVisitorContainer::visitEnter(const A3DMkpViewConnector& sConnector)
{
	if ((m_psActivatedView == NULL) || (m_psActivatedView != sConnector.GetA3DEntity()))
	{
		if (!m_bTraverseActivatedViewOnly)
		{
			VISITENTER;
		}

		return A3D_SUCCESS;
	}

	// VISITENTER
	ActivateEntityReference(sConnector.GetA3DEntity());
	for (unsigned uI = 0; uI < m_apVisitor.size(); uI++)
	{
		m_apVisitor[uI]->visitEnter(sConnector);
	}

	unsigned int uI, uNbRefEntity = sConnector.GetViewData().m_uiLinkedItemsSize;
	for (uI = 0; uI < uNbRefEntity; uI++)
	{
		A3DMiscMarkupLinkedItemData sMkpLinkedItemData;
		A3D_INITIALIZE_DATA(A3DMiscMarkupLinkedItemData, sMkpLinkedItemData);
		A3DMiscMarkupLinkedItem* pMkpLinkedItemEntityRef = sConnector.GetViewData().m_ppLinkedItems[uI];
		A3DMiscMarkupLinkedItemGet (pMkpLinkedItemEntityRef, &sMkpLinkedItemData);
		A3DViewLinkedItemManager sViewLinkedItemManager;

		A3DMiscEntityReferenceData sMiscRefData;
		A3D_INITIALIZE_DATA(A3DMiscEntityReferenceData, sMiscRefData);
		A3DMiscEntityReferenceGet (pMkpLinkedItemEntityRef, &sMiscRefData);

		sViewLinkedItemManager.m_pA3DEntityRef = sMiscRefData.m_pEntity;
		sViewLinkedItemManager.m_POOwner = sMkpLinkedItemData.m_pTargetProductOccurrence;
		sViewLinkedItemManager.m_pMarkupLinkedItem = pMkpLinkedItemEntityRef;
		sViewLinkedItemManager.m_uiPOLevel = m_uiCurrentLevel;

		A3DEEntityType sType;
		A3DEntityGetType (sMiscRefData.m_pEntity, &sType);
		if (sType == kA3DTypeMiscReferenceOnTopology)
		{
			A3DMiscReferenceOnTopologyData  sTopoRefData;
			A3D_INITIALIZE_DATA(A3DMiscReferenceOnTopologyData, sTopoRefData);
			A3DMiscReferenceOnTopologyGet (sMiscRefData.m_pEntity, &sTopoRefData);
			sViewLinkedItemManager.m_pA3DEntityRef = sTopoRefData.m_pBrepData;
			sViewLinkedItemManager.m_bRefOnTopoItem = true;
			A3DMiscReferenceOnTopologyGet (NULL, &sTopoRefData);
		}
		else if (sType == kA3DTypeMiscReferenceOnCsysItem)
		{
			A3DMiscReferenceOnCsysItemData sA3DMiscReferenceOnCSYSITemData;
			A3D_INITIALIZE_DATA(A3DMiscReferenceOnCsysItemData, sA3DMiscReferenceOnCSYSITemData);
			A3DMiscReferenceOnCsysItemGet (sMiscRefData.m_pEntity, &sA3DMiscReferenceOnCSYSITemData);
			sViewLinkedItemManager.m_pA3DEntityRef = sA3DMiscReferenceOnCSYSITemData.m_pCoordinateSystem;
			A3DMiscReferenceOnCsysItemGet (NULL, &sA3DMiscReferenceOnCSYSITemData);
		}
		else
			sViewLinkedItemManager.m_pA3DEntityRef = sMiscRefData.m_pEntity;

		A3DMiscEntityReferenceGet (NULL, &sMiscRefData);

		m_asViewLinkedItemManager.push_back(sViewLinkedItemManager);
	}

	return A3D_SUCCESS;
}

A3DStatus A3DVisitorContainer::visitLeave(const A3DMkpViewConnector& sConnector)
{
	if ((m_psActivatedView == NULL) || (m_psActivatedView != sConnector.GetA3DEntity()))
	{
		if (!m_bTraverseActivatedViewOnly)
			VISITLEAVE;

		return A3D_SUCCESS;
	}

	VISITLEAVE;
}

#endif

void A3DVisitorContainer::push( A3DVisitor* psVisitor )
{
//	vector<A3DVisitor*>::iterator iter =
//		search(m_apVisitor.begin(), m_apVisitor.end(), &psVisitor, &psVisitor+1);
// 	if(iter == m_apVisitor.end())
// 	{
// 		m_apVisitorExcludedForDelete.push_back(psVisitor);
// 	}
	m_apVisitor.push_back(psVisitor);
	if(!m_pTreeVisitor)
		m_pTreeVisitor = (A3DVisitor*)dynamic_cast<A3DTreeVisitor*>(psVisitor);//dynamic_cast return null if failed.
}


A3DVisitorContainer::~A3DVisitorContainer()
{
	size_t uI, uNbVisitor = m_apVisitor.size();
//	vector<A3DVisitor*>::iterator iter;
	for(uI = 0; uI < uNbVisitor; uI++)
// 	{
// 		iter = search(	m_apVisitorExcludedForDelete.begin(),
// 									m_apVisitorExcludedForDelete.end(),
// 									&m_apVisitor[uI],
// 									&m_apVisitor[uI]+1);
// 		if(iter == m_apVisitorExcludedForDelete.end())
			delete m_apVisitor[uI];
// 	}
}


void* A3DVisitorContainer::FindInMap(const A3DEntity* pA3DEntity)
{
	map<const A3DEntity*, void*>::iterator my_mapIter;
	my_mapIter = m_apA3DEntityYourEntityMap.find(pA3DEntity);
	if(my_mapIter == m_apA3DEntityYourEntityMap.end())
		return NULL;
	return my_mapIter->second;
}

void A3DVisitorContainer::SetInMap(const A3DEntity* pA3DEntity, void* pYourEntity)
{
	m_apA3DEntityYourEntityMap.insert(std::pair<const A3DEntity*, void*>(pA3DEntity, pYourEntity));
}

void A3DVisitorContainer::RemoveFromMap(const A3DEntity* pA3DEntity)
{
	m_apA3DEntityYourEntityMap.erase(pA3DEntity);
}

A3DVisitor* A3DVisitorContainer::GetVisitorByName( std::string strName )
{
	for(auto visitor : m_apVisitor)
	{
		if(visitor->GetName() == strName)
		{
			return visitor;
		}
	}
	return nullptr;
}

A3DVisitor* A3DVisitorContainer::GetTreeVisitor() const
{
	return m_pTreeVisitor;
}

void A3DVisitorContainer::ActivateEntityReference( A3DEntity const* pEntity )
{
	// StepEntityReference
	size_t uI, uNbPushStepEntityRef = m_asStepEntityRefManager.size();
	for(uI = 0; uI < uNbPushStepEntityRef; uI++)
	{
		if(m_asStepEntityRefManager[uI].m_pA3DEntityRef == pEntity)
		{
			m_asStepEntityRefManager[uI].m_uiPushLevel = m_uiCurrentLevel;
		}
	}

	// ViewLinkedItem
	size_t uNbPushViewLMkpLinkedItem = m_asViewLinkedItemManager.size();
	for (uI = 0; uI < uNbPushViewLMkpLinkedItem; uI++)
	{
		A3DViewLinkedItemManager *pCurManager = &m_asViewLinkedItemManager[uI];
		if (pCurManager->m_pA3DEntityRef == pEntity &&
			(	pCurManager->m_POOwner == NULL ||
			pCurManager->m_POOwner == pEntity ||
			pCurManager->m_POOwner == m_pCurrentPOFather
			)
			)
		{
			pCurManager->m_uiPushLevel = m_uiCurrentLevel;
		}
	}
}

void A3DVisitorContainer::DeactivateEntityReference( A3DEntity const* pEntity )
{
	// StepEntityReference
	size_t uI, uNbPushStepEntityRef = m_asStepEntityRefManager.size();
	for(uI = 0; uI < uNbPushStepEntityRef; uI++)
	{
		if(m_asStepEntityRefManager[uI].m_pA3DEntityRef == pEntity)
		{
			m_asStepEntityRefManager[uI].m_uiPushLevel = ~0U;
		}
	}

	// ViewLinkedItem
	size_t uNbPushViewLMkpLinkedItem = m_asViewLinkedItemManager.size();
	for (uI = 0; uI < uNbPushViewLMkpLinkedItem; uI++)
	{
		A3DViewLinkedItemManager *pCurManager = &m_asViewLinkedItemManager[uI];
		if (pCurManager->m_pA3DEntityRef == pEntity &&
			(	pCurManager->m_POOwner == NULL ||
			pCurManager->m_POOwner == pEntity ||
			pCurManager->m_POOwner == m_pCurrentPOFather
			)
			)
		{
			pCurManager->m_uiPushLevel = ~0U;
		}
	}
}

A3DStepEntityRefManager const* A3DVisitorContainer::GetActiveStepEntityRefManager(A3DEntity const * pEntity) const
{
	A3DStepEntityRefManager const * psActivated = NULL;
	size_t uI, uNbPushStepEntityRef = m_asStepEntityRefManager.size();
	for(uI = 0; uI < uNbPushStepEntityRef; uI++)
	{
		A3DStepEntityRefManager const * psCurManager = &m_asStepEntityRefManager[uI];
		if (psCurManager->m_uiPushLevel!=~0U && (pEntity==NULL || psCurManager->m_pA3DEntityRef==pEntity))
		{
			if (!psActivated)
				psActivated = psCurManager;
			else if (psCurManager->IsFirst(*psActivated))
				psActivated = psCurManager;
		}
	}
	return psActivated;
}

A3DViewLinkedItemManager const* A3DVisitorContainer::GetActiveViewLinkedItemManager(A3DEntity const * pEntity) const
{
	A3DViewLinkedItemManager const * psActivated = NULL;

	size_t uI, uNbPushViewMarkupLinkedItem = m_asViewLinkedItemManager.size();
	for (uI = 0; uI < uNbPushViewMarkupLinkedItem; uI++)
	{
		A3DViewLinkedItemManager const * psCurManager = &m_asViewLinkedItemManager[uI];
		if (psCurManager->m_uiPushLevel!=~0U  && (pEntity==NULL || psCurManager->m_pA3DEntityRef==pEntity))
		{
			if (!psActivated)
				psActivated = psCurManager;
			else if (psCurManager->IsFirst(*psActivated))
				psActivated = psCurManager;
		}
	}

	return psActivated;
}

#ifdef CONNECT_FEATURE
A3DStatus A3DVisitorContainer::visitEnter(const A3DFRMFeatureConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitLeave(const A3DFRMFeatureConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitEnter(const A3DFRMParameterConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitLeave(const A3DFRMParameterConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitEnter(const A3DFRMFeatureTreeConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitLeave(const A3DFRMFeatureTreeConnector& sConnector)
VISITLEAVE
A3DStatus A3DVisitorContainer::visitEnter(const A3DFRMFeatureLinkedItemConnector& sConnector)
VISITENTER
A3DStatus A3DVisitorContainer::visitLeave(const A3DFRMFeatureLinkedItemConnector& sConnector)
VISITLEAVE

#endif
