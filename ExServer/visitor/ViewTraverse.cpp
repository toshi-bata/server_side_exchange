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
\file ViewTraverse.cpp	

This file contains functionalities to traverse a PRC View  

***********************************************************************************************************************/

#include "VisitorContainer.h"
#include "ViewTraverse.h"

class A3DVisitorContainer;

A3DStatus A3DMkpViewConnector::TraverseView(A3DVisitorContainer*  pVisitor)
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(pVisitor->visitEnter(*this));
	
	A3DUns32 uiI;
	for (uiI = 0; uiI<m_sViewData.m_uiAnnotationsSize; uiI++)
	{
		A3DMkpAnnotationEntityConnector sAnnotationEntityConnector(m_sViewData.m_ppAnnotations[uiI]);
		CHECK_RET(sAnnotationEntityConnector.TraverseAnnotationEntity(pVisitor));
	}
	
	CHECK_RET(pVisitor->visitLeave(*this));
	
	return A3D_SUCCESS;
}
