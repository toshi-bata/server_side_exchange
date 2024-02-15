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

#ifndef VISIT_BREP
#define VISIT_BREP
#include "Visitors.h"
#include "BrepTraverse.h"
#include <map>

class A3DVisitorBrep  : public A3DVisitor
{
private:
	std::map<A3DTopoEdge*,const A3DTopoCoEdge*> m_MapEdgeCoEdge;
	double m_dScale;

public:
	
	A3DVisitorBrep(A3DVisitorContainer* psContainer = NULL) : A3DVisitor("Brep", psContainer) { m_dScale = 1.0;}

	virtual A3DStatus visitEnter(const A3DRiConnector&) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DRiConnector&) { return A3D_SUCCESS; }
	
  	virtual A3DStatus visitEnter(const A3DBrepDataConnector&) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DBrepDataConnector&) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DConnexConnector&) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DConnexConnector&) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DShellConnector&) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DShellConnector&) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DFaceConnector&) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DFaceConnector&) { return A3D_SUCCESS; }		

	virtual A3DStatus visitEnter(const A3DLoopConnector&) { return A3D_SUCCESS; }	
	virtual A3DStatus visitLeave(const A3DLoopConnector&) { return A3D_SUCCESS; } 

	virtual A3DStatus visitEnter(const A3DCoEdgeConnector&) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DCoEdgeConnector&) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DEdgeConnector&)		{ return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DEdgeConnector&)		{ return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DUniqueVertexConnector&)   { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DUniqueVertexConnector&)	{ return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DMultipleVertexConnector&)   { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DMultipleVertexConnector&) { return A3D_SUCCESS; }

	double GetScale() const { return m_dScale;}

	std::map<A3DTopoEdge*,const A3DTopoCoEdge*> & GetMapEdgeCoEdge() { return m_MapEdgeCoEdge;}

};


#endif	//VISIT_TOPO