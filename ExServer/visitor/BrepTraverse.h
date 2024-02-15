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

#ifndef	 TOPO_TRAVERSE
#define	 TOPO_TRAVERSE

#include "Connector.h"

class A3DVisitor;

////////////////////////////////////////////////
//			Brep
////////////////////////////////////////////////
class A3DBrepDataConnector : public  A3DConnector
{
public:

	A3DTopoBrepDataData m_sBrepData;

	A3DBrepDataConnector(const A3DTopoBrepData *pBrep);
	~A3DBrepDataConnector();

	A3DStatus Traverse(A3DVisitorContainer*  psVisitor) const;
};

////////////////////////////////////////////////
//			connex
////////////////////////////////////////////////
class A3DConnexConnector : public  A3DConnector
{
public:
	A3DTopoConnexData m_sConnexData;

	A3DConnexConnector(const A3DTopoConnex *pConnex);
	~A3DConnexConnector();

	A3DStatus Traverse(A3DVisitorContainer* psVisitor) const;
};

////////////////////////////////////////////////
//			Shell
////////////////////////////////////////////////
class A3DShellConnector : public  A3DConnector
{
public:
	A3DTopoShellData m_sShellData;

	A3DShellConnector(const A3DTopoShell *pShell);
	~A3DShellConnector();

	A3DStatus Traverse(A3DVisitorContainer*  psVisitor) const;
};

////////////////////////////////////////////////
//			Face
////////////////////////////////////////////////
class A3DFaceConnector : public  A3DConnector
{
public:
	A3DTopoFaceData m_sFaceData;

	A3DFaceConnector(const A3DTopoFace *pFace);
	~A3DFaceConnector();

	A3DStatus Traverse(A3DVisitorContainer*  psVisitor) const;
};

////////////////////////////////////////////////
//			Loop
////////////////////////////////////////////////
class A3DLoopConnector : public  A3DConnector
{
public:
	A3DTopoLoopData m_sLoopData;

	A3DLoopConnector(const A3DTopoLoop *pLoop);
	~A3DLoopConnector();
	
	A3DStatus Traverse(A3DVisitorContainer*  psVisitor) const;
};

////////////////////////////////////////////////
//			CoEdge
////////////////////////////////////////////////
class A3DCoEdgeConnector : public  A3DConnector
{
public:
	A3DTopoCoEdgeData m_sCoEdgeData;

	A3DCoEdgeConnector(const A3DTopoCoEdge *pCoEdge);
	~A3DCoEdgeConnector();

	A3DStatus Traverse(A3DVisitorContainer*  psVisitor) const;
};

////////////////////////////////////////////////
//			Edge
////////////////////////////////////////////////
class A3DEdgeConnector : public  A3DConnector
{
public:
	A3DTopoEdgeData m_sEdgeData;

	A3DEdgeConnector(const A3DTopoEdge *pEdge);
	~A3DEdgeConnector();

	A3DStatus Traverse(A3DVisitorContainer*  psVisitor) const;
};

////////////////////////////////////////////////
//			Vertex
////////////////////////////////////////////////
class A3DUniqueVertexConnector : public  A3DConnector
{
public:

	A3DTopoUniqueVertexData m_sVertexData;

	A3DUniqueVertexConnector(const A3DTopoVertex *pVertex);
	~A3DUniqueVertexConnector();

	A3DStatus Traverse(A3DVisitorContainer*  psVisitor) const;
};


class A3DMultipleVertexConnector : public  A3DConnector
{
public:
	A3DTopoMultipleVertexData m_sVertexData;

	A3DMultipleVertexConnector(const A3DTopoVertex *pVertex);
	~A3DMultipleVertexConnector();

	A3DStatus Traverse(A3DVisitorContainer*  psVisitor) const;
};


#endif