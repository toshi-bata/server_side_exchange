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
\file BrepTraverse.cpp	
This file demonstrates how to programmatically traverse the Brep structure 

***********************************************************************************************************************/

#include "VisitorContainer.h"
#include "BrepTraverse.h"
#include <map>


//////////////////////////////////////////////////////////////////////////////
//					Traverse Brep
//////////////////////////////////////////////////////////////////////////////

A3DBrepDataConnector::A3DBrepDataConnector(const A3DTopoBrepData *pBrep) : A3DConnector(pBrep) 
{
	A3D_INITIALIZE_DATA(A3DTopoBrepDataData, m_sBrepData);
	A3DTopoBrepDataGet(pBrep, &m_sBrepData);
}

A3DBrepDataConnector::~A3DBrepDataConnector()
{
	A3DTopoBrepDataGet(NULL, &m_sBrepData);
}

A3DStatus A3DBrepDataConnector::Traverse(A3DVisitorContainer*  psVisitor) const
{
	A3DStatus iRet = A3D_SUCCESS;
	
	//TopoContext
	A3DTopoBody *psTopoBody = (A3DTopoBody*) GetA3DEntity();
	A3DTopoBodyData sA3DTopoBodyData;
	A3D_INITIALIZE_DATA( A3DTopoBodyData, sA3DTopoBodyData);
	CHECK_RET(A3DTopoBodyGet(psTopoBody, &sA3DTopoBodyData));
	A3DTopoContext* psContext = sA3DTopoBodyData.m_pContext;

	A3DTopoContextData sA3DTopoContextData;
	A3D_INITIALIZE_DATA( A3DTopoContextData, sA3DTopoContextData);
	CHECK_RET(A3DTopoContextGet(psContext, &sA3DTopoContextData));
	//m_dScale = sA3DTopoContextData.m_dScale;
	
	CHECK_RET(psVisitor->visitEnter(*this));
	A3DUns32 uNbConnex = m_sBrepData.m_uiConnexSize;
	for(A3DUns32 uI = 0; uI < uNbConnex; uI++)
	{
		A3DConnexConnector sConnecConnector(m_sBrepData.m_ppConnexes[uI]);
		CHECK_RET(sConnecConnector.Traverse(psVisitor));
	}

	CHECK_RET(psVisitor->visitLeave(*this));
	return A3D_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//					Connex
//////////////////////////////////////////////////////////////////////////////

A3DConnexConnector::A3DConnexConnector(const A3DTopoConnex *pConnex) : A3DConnector(pConnex) 
{
	A3D_INITIALIZE_DATA(A3DTopoConnexData, m_sConnexData);
	A3DTopoConnexGet(pConnex, &m_sConnexData);
}

A3DConnexConnector::~A3DConnexConnector()
{
	A3DTopoConnexGet(NULL, &m_sConnexData);
}

A3DStatus A3DConnexConnector::Traverse(A3DVisitorContainer*  psVisitor) const
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));
	unsigned uI,  uNbShell = m_sConnexData.m_uiShellSize;
	for(uI = 0; uI < uNbShell; uI++)
	{
		A3DShellConnector sShellConnector(m_sConnexData.m_ppShells[uI]);
		CHECK_RET(sShellConnector.Traverse(psVisitor)); 
	}
	CHECK_RET(psVisitor->visitLeave(*this));
	return A3D_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//					Shell
//////////////////////////////////////////////////////////////////////////////

A3DShellConnector::A3DShellConnector(const A3DTopoShell *pShell) : A3DConnector(pShell) 
{
	A3D_INITIALIZE_DATA(A3DTopoShellData, m_sShellData);
	A3DTopoShellGet(pShell, &m_sShellData);
}

A3DShellConnector::~A3DShellConnector()
{
	A3DTopoShellGet(NULL, &m_sShellData);
}

A3DStatus A3DShellConnector::Traverse(A3DVisitorContainer*  psVisitor) const
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));
	unsigned int uI, uFaceSize = m_sShellData.m_uiFaceSize;
	for(uI = 0; uI < uFaceSize; uI++)
	{
		A3DFaceConnector sFaceConnector(m_sShellData.m_ppFaces[uI]);
		CHECK_RET(sFaceConnector.Traverse(psVisitor));
	}
	CHECK_RET(psVisitor->visitLeave(*this));
	return A3D_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//					Face
//////////////////////////////////////////////////////////////////////////////

A3DFaceConnector::A3DFaceConnector(const A3DTopoFace* pFace) : A3DConnector(pFace) 
{
	A3D_INITIALIZE_DATA(A3DTopoFaceData, m_sFaceData);
	A3DTopoFaceGet(pFace, &m_sFaceData);
}

A3DFaceConnector::~A3DFaceConnector()
{
	A3DTopoFaceGet(NULL, &m_sFaceData);
}

A3DStatus A3DFaceConnector::Traverse(A3DVisitorContainer*  psVisitor) const
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));
	unsigned int uI, uLoopSize = m_sFaceData.m_uiLoopSize;
	for(uI = 0; uI < uLoopSize; uI++)
	{
		A3DLoopConnector sLoopConnector(m_sFaceData.m_ppLoops[uI]);
		CHECK_RET(sLoopConnector.Traverse(psVisitor));
	}
	CHECK_RET(psVisitor->visitLeave(*this));
	return A3D_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//					Loop
//////////////////////////////////////////////////////////////////////////////

A3DLoopConnector::A3DLoopConnector(const A3DTopoLoop *pLoop) : A3DConnector(pLoop) 
{
	A3D_INITIALIZE_DATA(A3DTopoLoopData, m_sLoopData);
	A3DTopoLoopGet(pLoop, &m_sLoopData);
}

A3DLoopConnector::~A3DLoopConnector()
{
	A3DTopoLoopGet(NULL, &m_sLoopData);
}
	
A3DStatus A3DLoopConnector::Traverse(A3DVisitorContainer*  psVisitor) const
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));
	
	unsigned uI, uCoEdgeSize = m_sLoopData.m_uiCoEdgeSize;
	for(uI = 0; uI < uCoEdgeSize; uI++)
	{
		A3DCoEdgeConnector sCoEdgeConnector(m_sLoopData.m_ppCoEdges[uI]);
		CHECK_RET(sCoEdgeConnector.Traverse(psVisitor));
	}

	CHECK_RET(psVisitor->visitLeave(*this));
	return A3D_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//					CoEdge
//////////////////////////////////////////////////////////////////////////////

A3DCoEdgeConnector::A3DCoEdgeConnector(const A3DTopoCoEdge *pCoEdge) : A3DConnector(pCoEdge) 
{
	A3D_INITIALIZE_DATA(A3DTopoCoEdgeData, m_sCoEdgeData);
	A3DTopoCoEdgeGet(pCoEdge, &m_sCoEdgeData);
}

A3DCoEdgeConnector::~A3DCoEdgeConnector()
{
	A3DTopoCoEdgeGet(NULL, &m_sCoEdgeData);
}

A3DStatus A3DCoEdgeConnector::Traverse(A3DVisitorContainer*  psVisitor) const
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));

	if(!(psVisitor->FindInMap(m_sCoEdgeData.m_pEdge)))
	{
		psVisitor->SetInMap(m_sCoEdgeData.m_pEdge,(void*) this->GetA3DEntity());
		A3DEdgeConnector sEdgeConnector(m_sCoEdgeData.m_pEdge);
		CHECK_RET(sEdgeConnector.Traverse(psVisitor));
	}
	CHECK_RET(psVisitor->visitLeave(*this));
	
	return A3D_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//					Edge
//////////////////////////////////////////////////////////////////////////////

A3DEdgeConnector::A3DEdgeConnector(const A3DTopoEdge *pEdge)
	: A3DConnector(pEdge)
{
	A3D_INITIALIZE_DATA(A3DTopoEdgeData, m_sEdgeData);
	A3DTopoEdgeGet(pEdge, &m_sEdgeData);
}

A3DEdgeConnector::~A3DEdgeConnector()
{
	A3DTopoEdgeGet(NULL, &m_sEdgeData);
}

A3DStatus A3DEdgeConnector::Traverse(A3DVisitorContainer*  psVisitor) const
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));
	
	A3DEEntityType eType;
	//Start Vertex
	if(m_sEdgeData.m_pStartVertex != NULL)
	{
		CHECK_RET(A3DEntityGetType(m_sEdgeData.m_pStartVertex,&eType));
		if(eType ==	kA3DTypeTopoMultipleVertex)
		{	
			A3DMultipleVertexConnector sStart(m_sEdgeData.m_pStartVertex);
			sStart.Traverse(psVisitor);
		}
		else if(eType == kA3DTypeTopoUniqueVertex)
		{
			A3DUniqueVertexConnector sStart(m_sEdgeData.m_pStartVertex);
			sStart.Traverse(psVisitor);
		}
	}

	//End Vertex
	if(m_sEdgeData.m_pEndVertex != NULL)
	{
		CHECK_RET(A3DEntityGetType(m_sEdgeData.m_pEndVertex,&eType));
		if(eType ==	kA3DTypeTopoMultipleVertex)
		{	
			A3DMultipleVertexConnector sStart(m_sEdgeData.m_pEndVertex);
			sStart.Traverse(psVisitor);
		}
		else if(eType == kA3DTypeTopoUniqueVertex)
		{
			A3DUniqueVertexConnector sStart(m_sEdgeData.m_pEndVertex);
			sStart.Traverse(psVisitor);
		}
	}
	
	CHECK_RET(psVisitor->visitLeave(*this));
	return A3D_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//					Vertex
//////////////////////////////////////////////////////////////////////////////

A3DUniqueVertexConnector::A3DUniqueVertexConnector(const A3DTopoVertex *pVertex)
	: A3DConnector(pVertex)
{
	A3D_INITIALIZE_DATA(A3DTopoUniqueVertexData, m_sVertexData);
	A3DTopoUniqueVertexGet(pVertex, &m_sVertexData);
}

A3DUniqueVertexConnector::~A3DUniqueVertexConnector()
{
	A3DTopoUniqueVertexGet(NULL, &m_sVertexData);
}

A3DStatus A3DUniqueVertexConnector::Traverse(A3DVisitorContainer*  psVisitor) const
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));
	return A3D_SUCCESS;
}


A3DMultipleVertexConnector::A3DMultipleVertexConnector(const A3DTopoVertex *pVertex) : A3DConnector(pVertex) 
{
	A3D_INITIALIZE_DATA(A3DTopoMultipleVertexData, m_sVertexData);
	A3DTopoMultipleVertexGet(pVertex, &m_sVertexData);
}

A3DMultipleVertexConnector::~A3DMultipleVertexConnector()
{
	A3DTopoMultipleVertexGet(NULL, &m_sVertexData);
}

A3DStatus A3DMultipleVertexConnector::Traverse(A3DVisitorContainer*  psVisitor) const
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));
	return A3D_SUCCESS;
}
