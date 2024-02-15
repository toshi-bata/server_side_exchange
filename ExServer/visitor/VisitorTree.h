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

#ifndef VISIT_TREE
#define VISIT_TREE
#include "Visitors.h"
#include "VisitorContainer.h"


class A3DConnector;
class A3DVisitorContainer;

class A3DTreeVisitor  : public A3DVisitor
{
	friend class A3DVisitorContainer;
protected:

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)	// Suppress the warning C4251 because it leads to have that kind of warning : "warning 
	// C4251: 'A3DVisitorColorMaterials::m_apsCascadedAttribute' : class 'std::vector<_Ty>' needs to have dll-interface to 
	// be used by clients of class 'A3DVisitorColorMaterials"
	// This is because of the "dllexport" on the visitor classes, as they use stl members and it seems that stl causes 
	// warning with dllexport with microsoft compiler
#endif // _MSC_VER

	unsigned m_uOption;
	void* m_pYourEntity;

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

public:
	
	A3DTreeVisitor(A3DVisitorContainer* psContainer = NULL) : A3DVisitor("Tree", psContainer)
	{
		m_uOption = 0;
		m_pYourEntity = NULL;
	}

	virtual ~A3DTreeVisitor() {};

	virtual A3DStatus visitEnter(const A3DRiBrepModelConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DRiBrepModelConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DRiSetConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DRiSetConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DRiConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DRiConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DPartConnector& sConnector );
	virtual A3DStatus visitLeave(const A3DPartConnector& sConnector);	

	virtual A3DStatus visitEnter(const A3DProductOccurrenceConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DProductOccurrenceConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DMkpViewConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DMkpViewConnector& sConnector);

	void SetYourEntity(void* pYourEnity);
	
};
#endif