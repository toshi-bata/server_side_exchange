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

#ifndef A3D_VISIST_TESSELLATION
#define A3D_VISIST_TESSELLATION

#include "Visitors.h"
#include "TessConnector.h"

class A3DVisitorTessellation : public A3DVisitor
{
	friend class A3DVisitorContainer;

protected:
	bool m_bShow;
	unsigned int m_uCurrentFaceIndice;
public:
	A3DVisitorTessellation(A3DVisitorContainer* psContainer = NULL) : A3DVisitor("Tessellation", psContainer) {};
	virtual ~A3DVisitorTessellation() {}

	virtual A3DStatus visitEnter(const A3DFaceTessDataConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DFaceTessDataConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DTessDataConnector& sTessConnector);
	virtual A3DStatus visitLeave(const A3DTessDataConnector& sTessConnector);

	virtual A3DStatus visitEnter(const A3DRiConnector& sTessConnector);
	virtual A3DStatus visitLeave(const A3DRiConnector& sTessConnector);

	virtual A3DStatus visitEnter(const A3DMarkupTessConnector& sMarkupTessConnector);
	virtual A3DStatus visitLeave(const A3DMarkupTessConnector& sMarkupTessConnector);

	virtual A3DStatus visitEnter(const A3DWireTessDataConnector& sTessConnector);
	virtual A3DStatus visitLeave(const A3DWireTessDataConnector& sTessConnector);
};
#endif

