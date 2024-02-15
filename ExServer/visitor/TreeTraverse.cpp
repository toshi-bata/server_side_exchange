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
/**
\file TreeConnector.cpp	

This file contains fuctionalities to traverse a Prc Model Tree 

***********************************************************************************************************************/

#include <stdlib.h>

#include "TreeTraverse.h"
#include "VisitorContainer.h"
#include "MarkupTraverse.h"
#include "ViewTraverse.h"


/************************************************************************************

Traverse Model File

*************************************************************************************/

// Traverse all the model tree
A3DStatus A3DModelFileConnector::Traverse(A3DVisitorContainer* psVisitor, bool bVisitPrototype)
{
	unsigned int uI;
	psVisitor->visitEnter(*this);
	
	for (uI = 0; uI < m_sModelFileData.m_uiPOccurrencesSize; uI++)
	{
		psVisitor->SetCurrentPoFather(NULL);
		A3DProductOccurrenceConnector sTreeConnector(m_sModelFileData.m_ppPOccurrences[uI]);
		if(sTreeConnector.TraversePO(m_sModelFileData.m_ppPOccurrences[uI], psVisitor, bVisitPrototype)!= A3D_SUCCESS)
			return A3D_ERROR;
	}
	psVisitor->visitLeave(*this);
	return A3D_SUCCESS;
}


/************************************************************************************

Traversing the assembly Structure

*************************************************************************************/

A3DProductOccurrenceConnector::A3DProductOccurrenceConnector(const A3DAsmProductOccurrence* pProductOccurrence)
	: A3DConnector(pProductOccurrence),
	m_pProductOccurrenceDataSLW(NULL),
	m_pProductOccurrenceDataCat(NULL),
	m_pProductOccurrenceDataCV5(NULL),
	m_pProductOccurrenceDataUg(NULL),
	m_pProductOccurrenceDataProe(NULL),
	m_pProductOccurrenceDataInv(NULL),
	m_pProductOccurrenceDataJT(NULL)
{
	m_pFather = NULL;
	m_bIsInstanciated = false;
	A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, m_sProductOccurrenceData);
	A3DAsmProductOccurrenceGet (pProductOccurrence, &m_sProductOccurrenceData);
	
	switch (m_sProductOccurrenceData.m_eModellerType)
	{
		case kA3DModellerSlw:
		{
			m_pProductOccurrenceDataSLW = (A3DAsmProductOccurrenceDataSLW*)malloc(sizeof(A3DAsmProductOccurrenceDataSLW));
			A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceDataSLW, *m_pProductOccurrenceDataSLW);
			A3DAsmProductOccurrenceGetSLW (pProductOccurrence, m_pProductOccurrenceDataSLW);
			break;
		}

		case kA3DModellerCatia:
		{
			m_pProductOccurrenceDataCat = (A3DAsmProductOccurrenceDataCat*)malloc(sizeof(A3DAsmProductOccurrenceDataCat));
			A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceDataCat, *m_pProductOccurrenceDataCat);
			A3DAsmProductOccurrenceGetCat (pProductOccurrence, m_pProductOccurrenceDataCat);
			break;
		}

		case kA3DModellerCatiaV5:
		{
			m_pProductOccurrenceDataCV5 = (A3DAsmProductOccurrenceDataCV5*)malloc(sizeof(A3DAsmProductOccurrenceDataCV5));
			A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceDataCV5, *m_pProductOccurrenceDataCV5);
			A3DAsmProductOccurrenceGetCV5 (pProductOccurrence, m_pProductOccurrenceDataCV5);
			break;
		}

		case kA3DModellerUnigraphics:
		{
			m_pProductOccurrenceDataUg = (A3DAsmProductOccurrenceDataUg*)malloc(sizeof(A3DAsmProductOccurrenceDataUg));
			A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceDataUg, *m_pProductOccurrenceDataUg);
			A3DAsmProductOccurrenceGetUg (pProductOccurrence, m_pProductOccurrenceDataUg);
			break;
		}

		case kA3DModellerProE:
		{
			m_pProductOccurrenceDataProe = (A3DAsmProductOccurrenceDataProe*)malloc(sizeof(A3DAsmProductOccurrenceDataProe));
			A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceDataProe, *m_pProductOccurrenceDataProe);
			A3DAsmProductOccurrenceGetProe (pProductOccurrence, m_pProductOccurrenceDataProe);
			break;
		}

		case kA3DModellerInventor:
		{
			m_pProductOccurrenceDataInv = (A3DAsmProductOccurrenceDataInv*)malloc(sizeof(A3DAsmProductOccurrenceDataInv));
			A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceDataInv, *m_pProductOccurrenceDataInv);
			A3DAsmProductOccurrenceGetInv (pProductOccurrence, m_pProductOccurrenceDataInv);
			break;
		}

		case kA3DModellerJt:
		{
			m_pProductOccurrenceDataJT = (A3DAsmProductOccurrenceDataJT*)malloc(sizeof(A3DAsmProductOccurrenceDataJT));
			A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceDataJT, *m_pProductOccurrenceDataJT);
			A3DAsmProductOccurrenceGetJT (pProductOccurrence, m_pProductOccurrenceDataJT);
			break;
		}
		default:
			break;
	}

	m_bIsPrototype = false;
	m_bIsExternal = false;
}

A3DProductOccurrenceConnector::~A3DProductOccurrenceConnector()
{
	A3DAsmProductOccurrenceGet (NULL, &m_sProductOccurrenceData);
	if (m_pProductOccurrenceDataSLW)
	{
		A3DAsmProductOccurrenceGetSLW (NULL, m_pProductOccurrenceDataSLW);
		free(m_pProductOccurrenceDataSLW);
	}

	if (m_pProductOccurrenceDataCat)
	{
		A3DAsmProductOccurrenceGetCat (NULL, m_pProductOccurrenceDataCat);
		free(m_pProductOccurrenceDataCat);
	}

	if (m_pProductOccurrenceDataCV5)
	{
		A3DAsmProductOccurrenceGetCV5 (NULL, m_pProductOccurrenceDataCV5);
		free(m_pProductOccurrenceDataCV5);
	}

	if (m_pProductOccurrenceDataUg)
	{
		A3DAsmProductOccurrenceGetUg (NULL, m_pProductOccurrenceDataUg);
		free(m_pProductOccurrenceDataUg);
	}

	if (m_pProductOccurrenceDataProe)
	{
		A3DAsmProductOccurrenceGetProe (NULL, m_pProductOccurrenceDataProe);
		free(m_pProductOccurrenceDataProe);
	}

	if (m_pProductOccurrenceDataInv)
	{
		A3DAsmProductOccurrenceGetInv (NULL, m_pProductOccurrenceDataInv);
		free(m_pProductOccurrenceDataInv);
	}

	if (m_pProductOccurrenceDataJT)
	{
		A3DAsmProductOccurrenceGetJT (NULL, m_pProductOccurrenceDataJT);
		free(m_pProductOccurrenceDataJT);
	}
}

// Traversing the assembly Structure
A3DStatus A3DProductOccurrenceConnector::TraversePO(const A3DAsmProductOccurrence* pOccurrence,
													A3DVisitorContainer* psVisitor,
													bool bVisitPrototype)
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));

	//Traverse AnnotationEntity
	A3DUns32 uI;
#ifdef CONNECT_PMI
	if (psVisitor->GetFlagElementToConnect() & CONNECT_PMI)
	{
		A3DUns32 uNbAnnotationEntity = m_sProductOccurrenceData.m_uiAnnotationsSize;
		for (uI = 0; uI < uNbAnnotationEntity; uI++)
		{
			A3DMkpAnnotationEntityConnector sAnnotationEntityConnector(m_sProductOccurrenceData.m_ppAnnotations[uI]);
			sAnnotationEntityConnector.TraverseAnnotationEntity(psVisitor);
		}
		if (!uNbAnnotationEntity && m_sProductOccurrenceData.m_pPrototype != NULL)
		{
			A3DAsmProductOccurrence* pProductPrototype = m_sProductOccurrenceData.m_pPrototype;
			while (pProductPrototype)
			{
				A3DAsmProductOccurrenceData sProductPrototypeData;
				A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sProductPrototypeData);
				CHECK_RET(A3DAsmProductOccurrenceGet (pProductPrototype, &sProductPrototypeData));
				uNbAnnotationEntity = sProductPrototypeData.m_uiAnnotationsSize;
				for (uI = 0; uI < uNbAnnotationEntity; uI++)
				{
					A3DMkpAnnotationEntityConnector sAnnotationEntityConnector(sProductPrototypeData.m_ppAnnotations[uI]);
					sAnnotationEntityConnector.TraverseAnnotationEntity(psVisitor);
				}

				pProductPrototype = sProductPrototypeData.m_pPrototype;
				CHECK_RET(A3DAsmProductOccurrenceGet (NULL, &sProductPrototypeData));
			}
		}
	}
#endif

#ifdef CONNECT_VIEWS
	if (psVisitor->GetFlagElementToConnect() & CONNECT_VIEWS)
	{
		A3DUns32 uNbView = m_sProductOccurrenceData.m_uiViewsSize;
		if (!uNbView && m_sProductOccurrenceData.m_pPrototype != NULL)
		{
			A3DAsmProductOccurrence* pProductPrototype = m_sProductOccurrenceData.m_pPrototype;
			while (pProductPrototype)
			{
				A3DAsmProductOccurrenceData sProductPrototypeData;
				A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sProductPrototypeData);
				CHECK_RET(A3DAsmProductOccurrenceGet (pProductPrototype, &sProductPrototypeData));
				uNbView = sProductPrototypeData.m_uiViewsSize;
				for (uI = 0; uI < uNbView; uI++)
				{
					A3DMkpViewConnector sMkpViewConnector(sProductPrototypeData.m_ppViews[uI]);
					sMkpViewConnector.TraverseView(psVisitor);
				}
				pProductPrototype = sProductPrototypeData.m_pPrototype;
				CHECK_RET(A3DAsmProductOccurrenceGet (NULL, &sProductPrototypeData));
			}
		}
		else if (uNbView)
		{
			for (uI = 0; uI < uNbView; uI++)
			{
				A3DMkpViewConnector sMkpViewConnector(m_sProductOccurrenceData.m_ppViews[uI]);
				sMkpViewConnector.TraverseView(psVisitor);
			}
		}
	}

#endif

#ifdef CONNECT_FEATURE
	if (psVisitor->GetFlagElementToConnect() & CONNECT_FEATURE)
	{
		A3DUns32 uNbFeatureBasedEntity = m_sProductOccurrenceData.m_uiFeatureBasedEntitiesSize;
		if (!uNbFeatureBasedEntity && m_sProductOccurrenceData.m_pPrototype != NULL)
		{
			A3DAsmProductOccurrence* pProductPrototype = m_sProductOccurrenceData.m_pPrototype;
			while (pProductPrototype)
			{
				A3DAsmProductOccurrenceData sProductPrototypeData;
				A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sProductPrototypeData);
				CHECK_RET(A3DAsmProductOccurrenceGet(pProductPrototype, &sProductPrototypeData));
				uNbFeatureBasedEntity = sProductPrototypeData.m_uiFeatureBasedEntitiesSize;
				for (uI = 0; uI < uNbFeatureBasedEntity; uI++)
				{
					A3DFRMFeatureTreeConnector sConnector(sProductPrototypeData.m_ppFeatureBasedEntities[uI]);
					sConnector.TraverseFeatureTree(psVisitor);
				}
				pProductPrototype = sProductPrototypeData.m_pPrototype;
				CHECK_RET(A3DAsmProductOccurrenceGet(NULL, &sProductPrototypeData));
			}
		}
		else if (uNbFeatureBasedEntity)
		{
			for (uI = 0; uI < uNbFeatureBasedEntity; uI++)
			{
				A3DFRMFeatureTreeConnector sConnector(m_sProductOccurrenceData.m_ppFeatureBasedEntities[uI]);
				sConnector.TraverseFeatureTree(psVisitor);
			}
		}
	}
#endif
	A3DAsmPartDefinition* pPart = NULL;
	if(!bVisitPrototype)
	{
		CHECK_RET(GetPart(pPart));
	}
	else
	{
		pPart=  m_sProductOccurrenceData.m_pPart;
	}
	if(pPart)
	{
		void * pEntityInMap = psVisitor->FindInMap(pPart);
		psVisitor->SetInMap( pPart, pPart);
		A3DPartConnector sPartConnector(pPart);
		psVisitor->SetCurrentPoFather(pOccurrence);
		sPartConnector.SetProductOccurrenceFather(pOccurrence);

		// if we haven't found the part in the map or if we traverse the instance
		if(pEntityInMap==NULL || psVisitor->TraverseInstances())
		{
			CHECK_RET(sPartConnector.TraversePart(psVisitor));
		}
		psVisitor->SetCurrentPoFather(NULL);
	}
	std::vector<A3DAsmProductOccurrence*> apSons;
	if(!bVisitPrototype)
	{
		CollectSons(apSons);
	}
	else
	{
		if(m_sProductOccurrenceData.m_pPrototype)
		{
			if(psVisitor->FindInMap(m_sProductOccurrenceData.m_pPrototype) == NULL)
			{
				psVisitor->SetInMap(m_sProductOccurrenceData.m_pPrototype, m_sProductOccurrenceData.m_pPrototype);	
				A3DProductOccurrenceConnector sPrototypeConnector(m_sProductOccurrenceData.m_pPrototype);
				sPrototypeConnector.SetPrototypeType(true);
				sPrototypeConnector.TraversePO(m_sProductOccurrenceData.m_pPrototype, psVisitor, bVisitPrototype);
			}
		}
		if(m_sProductOccurrenceData.m_pExternalData)
		{
			if(psVisitor->FindInMap(m_sProductOccurrenceData.m_pExternalData) == NULL)
			{
				psVisitor->SetInMap(m_sProductOccurrenceData.m_pExternalData, m_sProductOccurrenceData.m_pExternalData);	
				A3DProductOccurrenceConnector sExternalConnector(m_sProductOccurrenceData.m_pExternalData);
				sExternalConnector.SetExternalType(true);
				sExternalConnector.TraversePO(m_sProductOccurrenceData.m_pExternalData, psVisitor, bVisitPrototype);
			}
		}


		unsigned int uSize = m_sProductOccurrenceData.m_uiPOccurrencesSize;

		for( uI = 0; uI < uSize; uI++)
		{
			apSons.push_back(m_sProductOccurrenceData.m_ppPOccurrences[uI]);
		}
	}

	for (uI = 0; uI < apSons.size(); uI++)
	{
		A3DProductOccurrenceConnector sPoConnector(apSons[uI]);
		sPoConnector.SetProductOccurrenceFather(this->GetA3DEntity());
		psVisitor->SetCurrentPoFather(pOccurrence);
		CHECK_RET(sPoConnector.TraversePO(apSons[uI], psVisitor, bVisitPrototype));
		psVisitor->SetCurrentPoFather(NULL);
	}
	CHECK_RET(psVisitor->visitLeave(*this));

	return A3D_SUCCESS;
}

A3DStatus A3DProductOccurrenceConnector::CollectSons(
	std::vector<A3DAsmProductOccurrence*>& apSons) const 
{
	//Get Children or children on Prototype's	....Prototype's
	A3DAsmProductOccurrence*	 pPrototype	= m_sProductOccurrenceData.m_pPrototype;
	A3DAsmProductOccurrence** ppPo = m_sProductOccurrenceData.m_ppPOccurrences;
	unsigned int uSize = m_sProductOccurrenceData.m_uiPOccurrencesSize;
	A3DStatus iRet = A3D_SUCCESS;

	while(uSize == 0 && pPrototype)
	{
		A3DAsmProductOccurrenceData sPrototypeData;
		A3D_INITIALIZE_DATA( A3DAsmProductOccurrenceData, sPrototypeData);
		CHECK_RET(A3DAsmProductOccurrenceGet(pPrototype, &sPrototypeData));
		pPrototype =sPrototypeData.m_pPrototype;

		uSize = sPrototypeData.m_uiPOccurrencesSize;
		ppPo = sPrototypeData.m_ppPOccurrences;

		//TODO : Management of prototype with external assembly  
		//Not sur this can append. we don't have test files for validation.
		/*if (pPrototype==NULL && sPrototypeData.m_pExternalData)
		{
			pPrototype=sPrototypeData.m_pExternalData;
		}*/

		CHECK_RET(A3DAsmProductOccurrenceGet(NULL, &sPrototypeData));
	}

	unsigned uI;
	for( uI = 0; uI < uSize; uI++)
		apSons.push_back(ppPo[uI]);

	//Get Children on Externals
	if(m_sProductOccurrenceData.m_pExternalData)
	{
		if(apSons.size() == 0)
		{
			A3DProductOccurrenceConnector sExternalConnector(m_sProductOccurrenceData.m_pExternalData);
			CHECK_RET(sExternalConnector.CollectSons(apSons));
		}
		else
			apSons.push_back(m_sProductOccurrenceData.m_pExternalData);
	}

	return A3D_SUCCESS;
}

A3DStatus A3DProductOccurrenceConnector::GetPart(
	A3DAsmPartDefinition*& pPart) const
{	
	pPart = NULL;
	A3DStatus iRet = A3D_SUCCESS;;
	if(m_sProductOccurrenceData.m_pPart)
	{
		pPart = m_sProductOccurrenceData.m_pPart;
		return A3D_SUCCESS;
	}

	// Search for first Part found, recursively
	A3DAsmProductOccurrence* pProductPrototype = m_sProductOccurrenceData.m_pPrototype;
	while(pProductPrototype)
	{
		A3DAsmProductOccurrenceData sProductPrototypeData;
		A3D_INITIALIZE_DATA( A3DAsmProductOccurrenceData, sProductPrototypeData);
		CHECK_RET(A3DAsmProductOccurrenceGet(pProductPrototype, &sProductPrototypeData));
		if(sProductPrototypeData.m_pPart)
		{
			// take current part and return it
			pPart =  sProductPrototypeData.m_pPart;
			CHECK_RET(A3DAsmProductOccurrenceGet(NULL, &sProductPrototypeData));
			return A3D_SUCCESS;
		}
		else if (sProductPrototypeData.m_pPrototype)
		{
			// recurse in prototype if exist
			pProductPrototype = sProductPrototypeData.m_pPrototype;
		}
		else if (sProductPrototypeData.m_pExternalData)
		{
			// recurse in external if exist
			pProductPrototype = sProductPrototypeData.m_pExternalData;
		}
		else
		{
			// end recursion
			pProductPrototype = NULL;
		}

		CHECK_RET(A3DAsmProductOccurrenceGet(NULL, &sProductPrototypeData));
	}

	if(m_sProductOccurrenceData.m_uiPOccurrencesSize == 0)
	{
		if(m_sProductOccurrenceData.m_pExternalData)
		{
			A3DProductOccurrenceConnector sExternalconnector(m_sProductOccurrenceData.m_pExternalData);
			CHECK_RET(sExternalconnector.GetPart(pPart));
		}
	}
	return A3D_SUCCESS; 
}

A3DStatus A3DPartConnector::TraversePart(A3DVisitorContainer*  psVisitor) const
{
	A3DStatus iRet = A3D_SUCCESS;

	CHECK_RET(psVisitor->visitEnter(*this));

	//Traverse AnnotationEntity
	A3DUns32 uI;
#ifdef CONNECT_PMI
	if (psVisitor->GetFlagElementToConnect() & CONNECT_PMI)
	{
		A3DUns32 uNbAnnotationEntity = m_sPartData.m_uiAnnotationsSize;
		for (uI = 0; uI < uNbAnnotationEntity; uI++)
		{
			A3DMkpAnnotationEntityConnector sAnnotationEntityConnector(m_sPartData.m_ppAnnotations[uI]);
			sAnnotationEntityConnector.TraverseAnnotationEntity(psVisitor);
		}
	}
#endif

#ifdef CONNECT_VIEWS
	if (psVisitor->GetFlagElementToConnect() & CONNECT_VIEWS)
	{
		A3DUns32 uNbView = m_sPartData.m_uiViewsSize;
		for (uI = 0; uI < uNbView; uI++)
		{
			A3DMkpViewConnector sMkpViewConnector(m_sPartData.m_ppViews[uI]);
			sMkpViewConnector.TraverseView(psVisitor);
		}
	}
#endif

	//Traverse RI
	for(uI = 0; uI < m_sPartData.m_uiRepItemsSize; uI++)
	{
		A3DRiConnector sRiConnector(m_sPartData.m_ppRepItems[uI]);
		sRiConnector.TraverseRi(psVisitor);
	}

	CHECK_RET(psVisitor->visitLeave(*this));
	return A3D_SUCCESS;
}

A3DStatus A3DRiConnector::TraverseRi(A3DVisitorContainer* psVisitor)
{	
	A3DStatus iRet = A3D_SUCCESS;
	psVisitor->visitEnter(*this);

	const A3DEntity* pRi  = GetA3DEntity();
	A3DEEntityType eType=kA3DTypeUnknown;
	A3DEntityGetType(pRi,&eType);

	if (iRet==A3D_SUCCESS)
	{
		switch(eType)
		{
		case kA3DTypeRiBrepModel:
			{
				A3DRiBrepModelConnector sConnector( static_cast<A3DRiBrepModel const *>(pRi));
				CHECK_RET(sConnector.TraverseRiBrepModel(psVisitor));
				break;
			}
		case kA3DTypeRiSet:
		{
			
			A3DRiSetConnector sConnector(static_cast<A3DRiSet const *>(pRi));
			CHECK_RET(sConnector.TraverseRiSet(psVisitor));
			break;
		}
		case kA3DTypeRiPointSet:
		case kA3DTypeRiDirection:
		case kA3DTypeRiCurve:
		case kA3DTypeRiCoordinateSystem:
		case kA3DTypeRiPlane:
			break;
		case kA3DTypeRiPolyBrepModel:
			{
				A3DPolyRiBrepModelConnector sConnector(static_cast<A3DPolyRiBrepModelConnector const *>(pRi));
				CHECK_RET(sConnector.TraverseRiPolyBrepModel(psVisitor));
				break;
			}
		default:
			iRet=A3D_NOT_IMPLEMENTED;
			break;
		}
	}
	psVisitor->visitLeave(*this);

	return A3D_SUCCESS;
}

// RiBrepModel
A3DStatus A3DRiBrepModelConnector::TraverseRiBrepModel(A3DVisitorContainer*  psVisitor)	  const
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));

	//Traverse Mesh
#ifdef CONNECT_MESH
	if (psVisitor->GetFlagElementToConnect() & CONNECT_MESH)
	{
		A3DRiRepresentationItemData sRidata;
		A3D_INITIALIZE_DATA(A3DRiRepresentationItemData, sRidata);
		CHECK_RET(A3DRiRepresentationItemGet (this->GetA3DEntity(), &sRidata));
		A3DTessDataConnector sTessConnector(static_cast<A3DTess3D const *>(sRidata.m_pTessBase));
		CHECK_RET(sTessConnector.Traverse(psVisitor));
		CHECK_RET(A3DRiRepresentationItemGet (NULL, &sRidata));
	}
#endif

	//Traverse Brep Data
#ifdef CONNECT_BREP
	if (psVisitor->GetFlagElementToConnect() & CONNECT_BREP)
	{
		A3DBrepDataConnector sTopoConnector(m_sRiBrepModelData.m_pBrepData);
		CHECK_RET(sTopoConnector.Traverse(psVisitor));
	}

#endif	
	CHECK_RET(psVisitor->visitLeave(*this));

	return iRet;
}

// RiSet
A3DStatus A3DRiSetConnector::TraverseRiSet(A3DVisitorContainer*  psVisitor)
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));
	unsigned uI; 
	for(uI = 0; uI < m_sRiSetData.m_uiRepItemsSize; uI++)
	{
		A3DRiConnector sRiConnector(m_sRiSetData.m_ppRepItems[uI]);
		sRiConnector.TraverseRi(psVisitor);
	}

	CHECK_RET(psVisitor->visitLeave(*this));

	return iRet;
}

// RiPolyBrepModel
A3DStatus A3DPolyRiBrepModelConnector::TraverseRiPolyBrepModel( A3DVisitorContainer* psVisitor ) const
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));

	A3DRiRepresentationItemData sRiData;
	A3D_INITIALIZE_DATA(A3DRiRepresentationItemData, sRiData);
	CHECK_RET(A3DRiRepresentationItemGet(this->GetA3DEntity(), &sRiData));
	A3DTessDataConnector sTessConnector(static_cast<A3DTess3D const *>(sRiData.m_pTessBase));
	CHECK_RET(sTessConnector.Traverse(psVisitor));
	CHECK_RET(A3DRiRepresentationItemGet(NULL, &sRiData));

	CHECK_RET(psVisitor->visitLeave(*this));

	return iRet;
}

// A3DFRMParameter
A3DStatus A3DFRMFeatureConnector::TraverseFeature( A3DVisitorContainer* psVisitor ) const
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));
	unsigned uI;

	for (uI = 0; uI < m_sData.m_uiConnectionSize; uI++)
	{
		A3DFRMFeatureLinkedItemConnector sFeatureLinkedItemConnector(m_sData.m_ppConnections[uI]);
		sFeatureLinkedItemConnector.TraverseConnection(psVisitor);
	}


	for (uI = 0; uI < m_sData.m_uiParametersSize; uI++)
	{
		A3DFRMParameterConnector sParameterConnector(m_sData.m_ppParameters[uI]);
		sParameterConnector.TraverseParameter(psVisitor);
	}

	CHECK_RET(psVisitor->visitLeave(*this));

	return iRet;
}

// A3DFRMParameter
A3DStatus A3DFRMFeatureLinkedItemConnector::TraverseConnection( A3DVisitorContainer* psVisitor ) const
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));

	CHECK_RET(psVisitor->visitLeave(*this));
	return iRet;
}

// A3DFRMParameter
A3DStatus A3DFRMParameterConnector::TraverseParameter( A3DVisitorContainer* psVisitor ) const
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));
	unsigned uI;

	for (uI = 0; uI < m_sData.m_uiFeatureSize; uI++)
	{
		A3DFRMFeatureConnector sFeatureConnector(m_sData.m_ppFeatures[uI]);
		sFeatureConnector.TraverseFeature(psVisitor);
	}

	CHECK_RET(psVisitor->visitLeave(*this));

	return iRet;
}


// FeatureTree
A3DStatus A3DFRMFeatureTreeConnector::TraverseFeatureTree( A3DVisitorContainer* psVisitor ) const
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));
	unsigned uI;

	for (uI = 0; uI < m_sTreeData.m_uiIntermediateGeometriesSize; uI++)
	{
		A3DRiConnector sRiConnector( m_sTreeData.m_ppIntermediateGeometries[uI]);
		sRiConnector.TraverseRi(psVisitor);
	}

	for (uI = 0; uI < m_sTreeData.m_uiParametersSize; uI++)
	{
		A3DFRMParameterConnector sParameterConnector( m_sTreeData.m_ppsParameters[uI]);
		sParameterConnector.TraverseParameter(psVisitor);
	}

	CHECK_RET(psVisitor->visitLeave(*this));

	return iRet;
}
