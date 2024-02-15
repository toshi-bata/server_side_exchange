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
\file MarkupTraverse.cpp

This file contains functionalities to traverse a PRC Markup Tree

***********************************************************************************************************************/

#include "VisitorContainer.h"
#include "MarkupTraverse.h"
#include <A3DSDKTessellation.h>
#include <vector>
#include "MarkupTessConnector.h"

//////////////////////////////////////////////////////////////////////////////
//					Traverse Markup
//////////////////////////////////////////////////////////////////////////////
A3DStatus A3DMkpAnnotationEntityConnector::TraverseAnnotationEntity(A3DVisitorContainer* pVisitor)
{
	pVisitor->visitEnter(*this);

	A3DEEntityType eType=kA3DTypeUnknown;
	A3DStatus iRet = A3DEntityGetType(m_pEntity,&eType);
	CHECK_RET(iRet);
	switch(eType)
	{
		case kA3DTypeMkpAnnotationSet:
		{
			A3DMkpAnnotationSetConnector sMkpAnnotationSetConnector((A3DMkpAnnotationSet*) m_pEntity);
			iRet=sMkpAnnotationSetConnector.TraverseAnnotationSet(pVisitor);
			break;
		}
		case kA3DTypeMkpAnnotationReference:
		{
			A3DMkpAnnotationReferenceConnector sMkpAnnotationReferenceConnector((A3DMkpAnnotationReference*) m_pEntity);
			iRet=sMkpAnnotationReferenceConnector.TraverseAnnotationReference(pVisitor);
			break;
		}
		case kA3DTypeMkpAnnotationItem:
		{
			A3DMkpAnnotationItemConnector sMkpAnnotationItemConnector((A3DMkpAnnotationItem*) m_pEntity);
			iRet=sMkpAnnotationItemConnector.TraverseAnnotationItem(pVisitor);
			break;
		}
		default:
			iRet=A3D_ERROR;
			break;
	}
	pVisitor->visitLeave(*this);

	return A3D_SUCCESS;
}

A3DStatus A3DMkpAnnotationSetConnector::TraverseAnnotationSet(A3DVisitorContainer* pVisitor)
{
	pVisitor->visitEnter(*this);
	A3DUns32 uNbAnnotEntity = m_sAnnotSetData.m_uiAnnotationsSize;
	for(A3DUns32 uI = 0; uI < uNbAnnotEntity; uI++)
	{
		A3DMkpAnnotationEntityConnector sMkpAnnotationEntityConnector(m_sAnnotSetData.m_ppAnnotations[uI]);
		sMkpAnnotationEntityConnector.TraverseAnnotationEntity(pVisitor);
	}
	pVisitor->visitLeave(*this);
	return A3D_SUCCESS;
}

A3DStatus A3DMkpAnnotationReferenceConnector::TraverseAnnotationReference(A3DVisitorContainer*  pVisitor)
{
   pVisitor->visitEnter(*this);
   pVisitor->visitLeave(*this);
   return A3D_SUCCESS;
}

A3DStatus A3DMkpAnnotationItemConnector::TraverseAnnotationItem(A3DVisitorContainer* pVisitor)
{
	pVisitor->visitEnter(*this);
	if(m_sAnnotItemData.m_pMarkup == NULL)
	{
		pVisitor->visitLeave(*this);
		return A3D_SUCCESS;
	}

	A3DMarkupConnector sMarkupConnector((A3DMkpMarkup*) m_sAnnotItemData.m_pMarkup);
	/*A3DStatus iRet=*/sMarkupConnector.TraverseMarkup(pVisitor);

	pVisitor->visitLeave(*this);
	return A3D_SUCCESS;
}

A3DStatus A3DMarkupConnector::TraverseMarkupTessellation(std::vector<A3DMarkupTessConnector*>& asMarkupTessConnector,
														 double a_dParallelToScreenPMIScale)
{
	A3DStatus iRet = A3D_SUCCESS;
	A3DTessMarkupData sMarkupTessData;
	A3D_INITIALIZE_DATA(A3DTessMarkupData, sMarkupTessData);
	CHECK_RET(A3DTessMarkupGet(m_sMarkupData.m_pTessellation, &sMarkupTessData));

	A3DTessBaseData sTessBaseData;
	A3D_INITIALIZE_DATA(A3DTessBaseData, sTessBaseData);
	CHECK_RET(A3DTessBaseGet(m_sMarkupData.m_pTessellation, &sTessBaseData));

	bool bIsText = m_sMarkupData.m_eType == kA3DMarkupTypeText;

	CHECK_RET(A3DMarkupTessConnector::GetMarkupTessellation(sTessBaseData, &sMarkupTessData, bIsText,
															asMarkupTessConnector, a_dParallelToScreenPMIScale));

	CHECK_RET(A3DTessBaseGet(NULL, &sTessBaseData));
	CHECK_RET(A3DTessMarkupGet(NULL, &sMarkupTessData));

	unsigned int uI, uJ;
	for(uI = 0; uI < m_sMarkupData.m_uiLeadersSize; uI++)
	{
		A3DMkpLeader* psLeader = m_sMarkupData.m_ppLeaders[uI];
		if(psLeader == NULL)
		{
			continue;
		}
		A3DMkpLeaderData sMarkupLeaderData;
		A3D_INITIALIZE_DATA(A3DMkpLeaderData, sMarkupLeaderData);
		CHECK_RET(A3DMkpLeaderGet(psLeader, &sMarkupLeaderData));

		A3DTessMarkupData sMarkupLeaderTessData;
		A3D_INITIALIZE_DATA(A3DTessMarkupData, sMarkupLeaderTessData);
		CHECK_RET(A3DTessMarkupGet(sMarkupLeaderData.m_pTessellation, &sMarkupLeaderTessData));

		A3DTessBaseData sLeaderTessBaseData;
		A3D_INITIALIZE_DATA(A3DTessBaseData, sLeaderTessBaseData);
		CHECK_RET(A3DTessBaseGet(sMarkupLeaderData.m_pTessellation, &sLeaderTessBaseData));

		std::vector<A3DMarkupTessConnector*> asMarkupLeaderConnector;

		CHECK_RET(A3DMarkupTessConnector::GetMarkupTessellation(sLeaderTessBaseData, &sMarkupLeaderTessData, bIsText,
																asMarkupLeaderConnector, a_dParallelToScreenPMIScale));

		for(uJ= 0; uJ < asMarkupLeaderConnector.size(); uJ++)
			asMarkupTessConnector.push_back(asMarkupLeaderConnector[uJ]);

		CHECK_RET(A3DTessBaseGet(NULL, &sLeaderTessBaseData));
		CHECK_RET(A3DTessMarkupGet(NULL, &sMarkupLeaderTessData));
		CHECK_RET(A3DMkpLeaderGet(NULL, &sMarkupLeaderData));
	}

	return A3D_SUCCESS;
}

A3DStatus A3DMarkupConnector::TraverseMarkup(A3DVisitorContainer* pVisitor)
{
	pVisitor->visitEnter(*this);

	A3DEEntityType eType = kA3DTypeUnknown;
	A3DStatus iRet = A3DEntityGetType( this->GetA3DEntity(), &eType);
	switch (eType)
	{

	case kA3DTypeMarkupText:
			   break;
	case kA3DTypeMarkupRichText:
		break;
	case kA3DTypeMarkupCoordinate:
		break;
	case kA3DTypeMarkupBalloon:
		break;
	case kA3DTypeMarkupDatum:
		{
			A3DMarkupDatumConnector sMkpDatumConnector((A3DMkpMarkup*) this->GetA3DEntity());
			iRet=sMkpDatumConnector.TraverseDatum(pVisitor);
		}
		break;
	case kA3DTypeMarkupGDT:
		{
			A3DMarkupGDTConnector sMkpGDTConnector((A3DMkpMarkup*) this->GetA3DEntity());
			iRet=sMkpGDTConnector.TraverseGDT(pVisitor);
		}
		break;
	case kA3DTypeMarkupRoughness:
		break;
	case kA3DTypeMarkupDimension:
		{
			A3DMarkupDimensionConnector sMkpDimensionConnector((A3DMkpMarkup*) this->GetA3DEntity());
			iRet=sMkpDimensionConnector.TraverseDimension(pVisitor);
		}
		break;
	case kA3DTypeMarkupLocator:
		break;
	case kA3DTypeMarkupMeasurementPoint:
		break;
	case kA3DTypeMarkupFastener:
		break;
	case kA3DTypeMarkupSpotWelding:
		break;
	case kA3DTypeMarkupLineWelding:
		break;
	default:
		break;
	}
	pVisitor->visitLeave(*this);
	return A3D_SUCCESS;
}

A3DStatus A3DMarkupDimensionConnector::TraverseDimension(A3DVisitorContainer* pVisitor)
{
	pVisitor->visitEnter(*this);
	pVisitor->visitLeave(*this);
	return A3D_SUCCESS;
}
A3DStatus A3DMarkupGDTConnector::TraverseGDT(A3DVisitorContainer* pVisitor)
{
	pVisitor->visitEnter(*this);
	pVisitor->visitLeave(*this);
	return A3D_SUCCESS;
}

A3DStatus A3DMarkupDatumConnector::TraverseDatum(A3DVisitorContainer* pVisitor)
{
	pVisitor->visitEnter(*this);
	pVisitor->visitLeave(*this);
	return A3D_SUCCESS;
}

