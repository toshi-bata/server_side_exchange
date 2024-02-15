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

#include "TransfoConnector.h"
#include "VisitorTransfo.h"
#include "TreeTraverse.h"
#include "Matrix.h"
#include "VisitorTree.h"

A3DVisitorTransfo::A3DVisitorTransfo(	bool bModelFileUnitFormCAD,
													double dUnitModelFile,
													A3DVisitorContainer* psContainer /*= NULL*/ )
:	A3DVisitor("Transformation", psContainer),
	m_dUnit(dUnitModelFile),
	m_bUnitFormCad(bModelFileUnitFormCAD)
{
}

A3DVisitorTransfo::A3DVisitorTransfo( A3DVisitorContainer* psContainer /*= NULL*/ )
:	A3DVisitor("Transformation", psContainer),
	m_dUnit(1.),
	m_bUnitFormCad(false)
{
}

A3DVisitorTransfo::~A3DVisitorTransfo()
{
}

A3DTransfoConnector* A3DVisitorTransfo::GetTransfoConnectorFromManager(A3DEntity const * pEntity)
{
	if(m_psContainer)
	{
		A3DViewLinkedItemManager const* pVLIM =  m_psContainer->GetActiveViewLinkedItemManager(pEntity);
		if (pVLIM)
		{
			A3DMiscEntityReferenceData sSERData;
			A3D_INITIALIZE_DATA(A3DMiscEntityReferenceData, sSERData);
			A3DStatus iErr = A3DMiscEntityReferenceGet (pVLIM->m_pMarkupLinkedItem, &sSERData);
			if (iErr == A3D_SUCCESS && sSERData.m_pCoordinateSystem)
			{
				A3DRiCoordinateSystemData sCSysData;
				A3D_INITIALIZE_DATA(A3DRiCoordinateSystemData, sCSysData);
				iErr = A3DRiCoordinateSystemGet (sSERData.m_pCoordinateSystem, &sCSysData);
				A3DTransfoConnector* pConnector = new A3DTransfoConnector(NULL);
				pConnector->SetGlobalMatrix(sCSysData.m_pTransformation);
				return pConnector;
			}
			A3DMiscEntityReferenceGet (NULL, &sSERData);
		}
		
		A3DStepEntityRefManager const* pSERM =  m_psContainer->GetActiveStepEntityRefManager(pEntity);
		if(pSERM)
		{
			A3DMiscEntityReferenceData sSERData;
			A3D_INITIALIZE_DATA(A3DMiscEntityReferenceData, sSERData);
			A3DStatus iErr = A3DMiscEntityReferenceGet (pSERM->m_pStepEntityRef, &sSERData);
			if(iErr == A3D_SUCCESS && sSERData.m_pCoordinateSystem)
			{
				A3DRiCoordinateSystemData sCSysData;
				A3D_INITIALIZE_DATA(A3DRiCoordinateSystemData, sCSysData);
				A3DRiCoordinateSystemGet (sSERData.m_pCoordinateSystem, &sCSysData);
				A3DTransfoConnector* pConnector = new A3DTransfoConnector(NULL);
				
				pConnector->SetGlobalMatrix(sCSysData.m_pTransformation);
				return pConnector;
			}
			A3DMiscEntityReferenceGet (NULL, &sSERData);
		}
	}
	
	return NULL;
}

A3DStatus A3DVisitorTransfo::visitEnter(const A3DRiConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	A3DMatrix4x4 sTransfo;
	size_t uSize = m_adPushTransfo.size();

		// Use ItemReference transformation instead of local transformation
	A3DTransfoConnector *pConnectorFromRef = this->GetTransfoConnectorFromManager(sConnector.GetA3DEntity());
	if (pConnectorFromRef)
	{
		pConnectorFromRef->GetGlobalTransfo(sTransfo);
		m_adPushTransfo.push_back(sTransfo);
		delete pConnectorFromRef;
	
	}
	else if(sConnector.m_sRiData.m_pCoordinateSystem)
	{
		A3DRiCoordinateSystemData sA3DRiCoordinateSystemData;
		A3D_INITIALIZE_DATA(A3DRiCoordinateSystemData, sA3DRiCoordinateSystemData);
		A3DRiCoordinateSystemGet(sConnector.m_sRiData.m_pCoordinateSystem,&sA3DRiCoordinateSystemData);

		A3DTransfoConnector sTransfoConnector(sA3DRiCoordinateSystemData.m_pTransformation);
		sTransfoConnector.ComputeGlobalMatrix(uSize!= 0 ? m_adPushTransfo[uSize-1] : stIdentity);
		//Local Matrix
		A3DMatrix4x4 sThisMatrix;
		CHECK_RET(sTransfoConnector.GetLocalMatrix(sThisMatrix)); 
		CHECK_RET(SetLocalTransfo(sThisMatrix));
		//GlobalMatrix
		sTransfoConnector.GetGlobalTransfo(sTransfo);
		m_adPushTransfo.push_back(sTransfo);
		A3DRiCoordinateSystemGet(NULL, &sA3DRiCoordinateSystemData);
	}
	else
	{
		m_adPushTransfo.push_back(uSize!= 0 ? m_adPushTransfo[uSize-1] : stIdentity);
		CHECK_RET(SetLocalTransfo(stIdentity));
	}

	CHECK_RET(SetGlobalTransfo(m_adPushTransfo[m_adPushTransfo.size() - 1]));

	return A3D_SUCCESS;
}

A3DStatus A3DVisitorTransfo::visitLeave(const A3DRiConnector& /*sConnector*/)
{
	// End drawing the Representation Items
	m_adPushTransfo.pop_back();

	return A3D_SUCCESS;
}

//XXX   Is There a way to integrate this function in TreeTravrse?
static A3DStatus stProductOccurrenceGetLocation(	const A3DAsmProductOccurrenceData* psPOccData,
																A3DMiscCartesianTransformation** ppLocation)
{
	if (psPOccData == NULL)
		return A3D_ERROR;

	A3DStatus iRet = A3D_SUCCESS;
	A3DAsmProductOccurrence* pExternal = psPOccData->m_pExternalData;

	if (pExternal != NULL)
	{
		A3DAsmProductOccurrenceData sExternalDataData;
		A3D_INITIALIZE_DATA( A3DAsmProductOccurrenceData, sExternalDataData);
		A3DAsmProductOccurrenceGet(pExternal, &sExternalDataData);

		CHECK_RET(stProductOccurrenceGetLocation(	&sExternalDataData  , ppLocation));
		A3DAsmProductOccurrenceGet(NULL, &sExternalDataData);
		if(*ppLocation)
			return A3D_SUCCESS;
	}

	if (psPOccData->m_pLocation == NULL && psPOccData->m_pPrototype != NULL)
	{
		A3DAsmProductOccurrenceData sPrototypeData;
		A3D_INITIALIZE_DATA( A3DAsmProductOccurrenceData, sPrototypeData);
		A3DAsmProductOccurrenceGet(psPOccData->m_pPrototype, &sPrototypeData);

		CHECK_RET(stProductOccurrenceGetLocation(&sPrototypeData, ppLocation));
		A3DAsmProductOccurrenceGet(NULL, &sPrototypeData);
		return A3D_SUCCESS;
	}

	*ppLocation = psPOccData->m_pLocation;

	return iRet;
}

A3DStatus A3DVisitorTransfo::visitEnter(const A3DProductOccurrenceConnector& sConnector)
{
	A3DStatus iRet = A3D_SUCCESS;

	if(sConnector.m_sProductOccurrenceData.m_bUnitFromCAD && !m_bUnitFormCad)
	{
		m_bUnitFormCad = true;
		m_dUnit = sConnector.m_sProductOccurrenceData.m_dUnit;
	}

	// Use ItemReference transformation instead of local transformation
	A3DTransfoConnector *pConnectorFromRef = this->GetTransfoConnectorFromManager(sConnector.GetA3DEntity());
	if (pConnectorFromRef)
	{
		A3DMatrix4x4 sTransfo;
		pConnectorFromRef->GetGlobalTransfo(sTransfo);
		m_adPushTransfo.push_back(sTransfo);
		delete pConnectorFromRef;
	}
	else
	{
		A3DMiscCartesianTransformation* pLocation = NULL;
		stProductOccurrenceGetLocation(&(sConnector.m_sProductOccurrenceData), &pLocation);

		A3DMatrix4x4 sTransfo;
		size_t uSize = m_adPushTransfo.size();

		if(pLocation)
		{
			A3DTransfoConnector sTransfoConnector(pLocation);
	
			//Local Transfo
			A3DMatrix4x4 sThisMatrix;
			CHECK_RET(sTransfoConnector.GetLocalMatrix(sThisMatrix)); 
			CHECK_RET(SetLocalTransfo(sThisMatrix));

			//Global Transfo
			sTransfoConnector.ComputeGlobalMatrix(	uSize != 0 ? m_adPushTransfo[uSize - 1]: stIdentity);
			sTransfoConnector.GetGlobalTransfo(sTransfo);
			m_adPushTransfo.push_back(sTransfo);
		}
		else
		{
			m_adPushTransfo.push_back(uSize != 0 ? m_adPushTransfo[uSize - 1] : stIdentity);
			CHECK_RET(SetLocalTransfo(stIdentity));
		}
	}

	CHECK_RET(SetGlobalTransfo(m_adPushTransfo[m_adPushTransfo.size() - 1]));

	return A3D_SUCCESS;
}

A3DStatus A3DVisitorTransfo::visitLeave(const A3DProductOccurrenceConnector& /*sConnector*/)
{
	// End drawing the Product Occurrences
	m_adPushTransfo.pop_back();
	
	return A3D_SUCCESS;
}

A3DStatus A3DVisitorTransfo::visitEnter(const A3DModelFileConnector& sConnector)
{
	if(sConnector.m_sModelFileData.m_bUnitFromCAD && !m_bUnitFormCad)
	{
		m_bUnitFormCad = true;
		m_dUnit = sConnector.m_sModelFileData.m_dUnit;
	}
	return A3D_SUCCESS;
}

A3DStatus A3DVisitorTransfo::visitLeave(const A3DModelFileConnector& /*sConnector*/)
{
	return A3D_SUCCESS;
}

A3DTransfoConnector* A3DVisitorTransfo::GetTransfoConnector(bool bUseInstance /*= false*/ )
{
	if(bUseInstance)
	{
		size_t iSize = m_adPushTransfo.size();
		A3DMatrix4x4 rLastMatrix = iSize != 0 ? m_adPushTransfo[iSize-1] : stIdentity;
		A3DMiscCartesianTransformation* pTransfo = CreateTransfo(m_adLocalMatrix);
		A3DTransfoConnector* pConnector = new A3DTransfoConnector(pTransfo);
		pConnector->SetGlobalMatrix(rLastMatrix);
		A3DEntityDelete (pTransfo);
		return pConnector;
	}
	else
	{
		//if(m_psContainer)
		//{
		//	A3DTreeVisitor* pTreeVisitor = (A3DTreeVisitor*)m_psContainer->GetTreeVisitor();
		//	if(pTreeVisitor)
		//	{
		//		A3DStepEntityRefManager const* pSERM =  pTreeVisitor->GetFirstActiveStepEntityRefManager();
		//		if(pSERM)
		//		{
		//			A3DMiscEntityReferenceData sSERData;
		//			A3D_INITIALIZE_DATA(A3DMiscEntityReferenceData, sSERData);
		//			A3DStatus iErr = A3DMiscEntityReferenceGet(pSERM->m_pStepEntityRef, &sSERData);
		//			if(iErr == A3D_SUCCESS && sSERData.m_pCoordinateSystem)
		//			{
		//				A3DRiCoordinateSystemData sCSysData;
		//				A3D_INITIALIZE_DATA(A3DRiCoordinateSystemData, sCSysData);
		//				iErr = A3DRiCoordinateSystemGet(sSERData.m_pCoordinateSystem, &sCSysData);
		//				A3DTransfoConnector* pConnector = new A3DTransfoConnector(NULL);
		//				pConnector->SetGlobalMatrix(sCSysData.m_pTransformation);
		//				return pConnector;
		//			}
		//			A3DMiscEntityReferenceGet(NULL, &sSERData);
		//		}
		//		
		//		A3DViewLinkedItemManager const* pVLIM =  pTreeVisitor->GetFirstActiveViewLinkedItemManager();
		//		if (pVLIM)
		//		{
		//			A3DMiscEntityReferenceData sSERData;
		//			A3D_INITIALIZE_DATA(A3DMiscEntityReferenceData, sSERData);
		//			A3DStatus iErr = A3DMiscEntityReferenceGet (pVLIM->m_pMarkupLinkedItem, &sSERData);
		//			if (iErr == A3D_SUCCESS && sSERData.m_pCoordinateSystem)
		//			{
		//				A3DRiCoordinateSystemData sCSysData;
		//				A3D_INITIALIZE_DATA(A3DRiCoordinateSystemData, sCSysData);
		//				iErr = A3DRiCoordinateSystemGet (sSERData.m_pCoordinateSystem, &sCSysData);
		//				A3DTransfoConnector* pConnector = new A3DTransfoConnector(NULL);
		//				pConnector->SetGlobalMatrix(sCSysData.m_pTransformation);
		//				return pConnector;
		//			}
		//			A3DMiscEntityReferenceGet (NULL, &sSERData);
		//		}
		//	}
		//}
		size_t iSize = m_adPushTransfo.size();
		A3DMatrix4x4 rLastMatrix = iSize != 0 ? m_adPushTransfo[iSize-1] : stIdentity;
		A3DMiscCartesianTransformation* pTransfo = CreateTransfo(m_adLocalMatrix);
		A3DTransfoConnector* pConnector = new A3DTransfoConnector(pTransfo);
		pConnector->SetGlobalMatrix(rLastMatrix);
		A3DEntityDelete (pTransfo);
		return pConnector;
	}
}

A3DMiscTransformation* A3DVisitorTransfo::CreateTransfo( A3DMatrix4x4 const & rMatrix )
{

	A3DMiscGeneralTransformationData sData;
	A3D_INITIALIZE_DATA( A3DMiscGeneralTransformationData, sData);
	memcpy(	sData.m_adCoeff ,rMatrix.m_adM, 16*sizeof(double));
	A3DMiscGeneralTransformation* pTransfo = NULL;
	A3DMiscGeneralTransformationCreate (&sData, &pTransfo);
	return (A3DMiscTransformation*)pTransfo;
}
