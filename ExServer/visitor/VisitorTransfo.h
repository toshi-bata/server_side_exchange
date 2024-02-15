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

#ifndef VISIT_TRANSFO
#define VISIT_TRANSFO
#include "Visitors.h"
#include <stdio.h>
#include "Matrix.h"


class A3DTransfoConnector;


//Visitor used to Get transfo on each ProductOccurrences and Representation Item Nodes
class A3DVisitorTransfo  : public A3DVisitor
{
	//friend class A3DVisitorContainer;

protected:

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)	// Suppress the warning C4251 because it leads to have that kind of warning : "warning 
	// C4251: 'A3DVisitorColorMaterials::m_apsCascadedAttribute' : class 'std::vector<_Ty>' needs to have dll-interface to 
	// be used by clients of class 'A3DVisitorColorMaterials"
	// This is because of the "dllexport" on the visitor classes, as they use stl members and it seems that stl causes 
	// warning with dllexport with microsoft compiler
#endif // _MSC_VER

	std::vector<A3DMatrix4x4> m_adPushTransfo;
	A3DMatrix4x4 m_adLocalMatrix;
	bool m_bUnitFormCad;
	double m_dUnit;

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

	A3DMiscTransformation* CreateTransfo(A3DMatrix4x4 const & rMatrix);

public:

	A3DVisitorTransfo(bool bModelFileUnitFormCAD, 
		double dUnitModelFile,
		A3DVisitorContainer* psContainer = NULL);

	A3DVisitorTransfo(A3DVisitorContainer* psContainer = NULL);

	virtual ~A3DVisitorTransfo();

	virtual A3DStatus visitEnter(const A3DRiConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DRiConnector& sConnector);
	virtual A3DStatus visitEnter(const A3DProductOccurrenceConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DProductOccurrenceConnector& sConnector);
	virtual A3DStatus visitEnter(const A3DModelFileConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DModelFileConnector& sConnector);

	virtual A3DStatus SetLocalTransfo(A3DMatrix4x4 const& rdLocalTransfo) { m_adLocalMatrix = rdLocalTransfo; return A3D_SUCCESS; }
	virtual A3DStatus SetGlobalTransfo(A3DMatrix4x4 const& /*rdLocalTransfo*/) { return A3D_SUCCESS; } 

	bool UnitFromCad() const
	{ 
		return m_bUnitFormCad;
	}

	double Unit() const
	{ 
		return m_dUnit;
	}

	A3DTransfoConnector* GetTransfoConnector(bool bUseInstance = false);

	A3DTransfoConnector* GetTransfoConnectorFromManager(A3DEntity const * pEntity);
};
#endif
