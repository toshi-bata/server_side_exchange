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

#include "VisitorTree.h"

A3DStatus  A3DTreeVisitor::visitEnter(const A3DRiBrepModelConnector& /*sConnector*/)
{
	return A3D_SUCCESS; 
}

A3DStatus  A3DTreeVisitor::visitLeave(const A3DRiBrepModelConnector& /*sConnector*/)
{ 
	return A3D_SUCCESS; 
}

A3DStatus  A3DTreeVisitor::visitEnter(const A3DRiSetConnector& /*sConnector*/)
{
	return A3D_SUCCESS; 
}

A3DStatus  A3DTreeVisitor::visitLeave(const A3DRiSetConnector& /*sConnector*/)
{
	return A3D_SUCCESS; 
}

A3DStatus  A3DTreeVisitor::visitEnter(const A3DRiConnector& /*sConnector*/)
{
	return A3D_SUCCESS; 
}

A3DStatus  A3DTreeVisitor::visitLeave(const A3DRiConnector& /*sConnector*/)
{ 
	return A3D_SUCCESS; 
}

A3DStatus  A3DTreeVisitor::visitEnter(const A3DPartConnector& /*sConnector*/)
{
	return A3D_SUCCESS;
}

A3DStatus  A3DTreeVisitor::visitLeave(const A3DPartConnector& /*sConnector*/)
{
	return A3D_SUCCESS;
}

A3DStatus  A3DTreeVisitor::visitEnter(const A3DProductOccurrenceConnector& sConnector)
{
	if(m_uOption&NODE_INSTANCES)
	{
		m_pYourEntity = m_psContainer->FindInMap(sConnector.GetA3DEntity());
		if(m_pYourEntity)
		{
			((A3DProductOccurrenceConnector*) &sConnector)->SetIsInstanciated(true);
		}
	}

	m_psContainer->SetInMap(sConnector.GetA3DEntity(),(void*) sConnector.GetA3DEntity() );

	return A3D_SUCCESS;
}

A3DStatus  A3DTreeVisitor::visitLeave(const A3DProductOccurrenceConnector& /*sConnector*/)
{
	return A3D_SUCCESS;
}

A3DStatus A3DTreeVisitor::visitEnter(const A3DMkpViewConnector& sConnector)
{
	m_psContainer->SetInMap(sConnector.GetA3DEntity(), (void*)sConnector.GetA3DEntity());//m_pYourEntity);

	return A3D_SUCCESS;
}

A3DStatus A3DTreeVisitor::visitLeave(const A3DMkpViewConnector& /*sConnector*/)
{
	return A3D_SUCCESS;
}

void A3DTreeVisitor::SetYourEntity(void* /*pYourEnity*/)
{
	//std::vector<A3DStepEntityRefManager>::iterator sIter;
	//for(sIter = m_asStepEntityRefManager.begin(); sIter != m_asStepEntityRefManager.end(); ++sIter)
	//{
	//	if(sIter->m_bPushChildren	== true)
	//		sIter->m_apYourEntity.push_back(pYourEnity);
	//}
	//
	//return;
}
