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

#ifndef VISITOR_CASCADED_ATTRIBUTE
#define VISITOR_CASCADED_ATTRIBUTE
#include "Visitors.h"
#include <map>

class A3DCascadedAttribute;
class ColorMaterialsConnector;

class A3DVisitorColorMaterials : public A3DVisitor
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)	// Suppress the warning C4251 because it leads to have that kind of warning : "warning 
	// C4251: 'A3DVisitorColorMaterials::m_apsCascadedAttribute' : class 'std::vector<_Ty>' needs to have dll-interface to 
	// be used by clients of class 'A3DVisitorColorMaterials"
	// This is because of the "dllexport" on the visitor classes, as they use stl members and it seems that stl causes 
	// warning with dllexport with microsoft compiler
#endif // _MSC_VER

	//friend class A3DVisitorContainer;
	std::vector<A3DMiscCascadedAttributes*> m_apsCascadedAttribute;
	A3DRiRepresentationItem* m_pCurrentRi; //needed for FaceTessDataConnector

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

public:

	A3DVisitorColorMaterials(A3DVisitorContainer* psContainer = NULL);

	virtual ~A3DVisitorColorMaterials();
	
	virtual A3DStatus pushCascadedAttribute(const A3DConnector& sEntity);
	virtual A3DStatus visitEnter(const A3DProductOccurrenceConnector& sEntity);
	virtual A3DStatus visitEnter(const A3DPartConnector& sEntity);
	virtual A3DStatus visitEnter(const A3DRiConnector& sEntity);
	virtual A3DStatus visitEnter(const A3DFaceTessDataConnector& sEntity);
	
	virtual A3DStatus popCascadedAttribute(/*const A3DConnector& sEntity*/);
	virtual A3DStatus visitLeave(const A3DProductOccurrenceConnector& sEntity);
	virtual A3DStatus visitLeave(const A3DPartConnector& sEntity);
	virtual A3DStatus visitLeave(const A3DRiConnector& sEntity);
	virtual A3DStatus visitLeave(const A3DFaceTessDataConnector& sEntity);

	std::vector<A3DMiscCascadedAttributes*> _Get() const { return m_apsCascadedAttribute; }
	void _Set(std::vector<A3DMiscCascadedAttributes*> const & val) { m_apsCascadedAttribute = val; }
	void _SetCurrentRi(A3DRiRepresentationItem* const val) { m_pCurrentRi = val; }

public:

	///////////////////////////////////////////////////////////////////
	//   Please, redefined this function in a inherited class to connect
	//	 the meshes provide by 3DX in your application
	//////////////////////////////////////////////////////////////////

	virtual A3DStatus SetGlobalAttributeOnLeaf(const A3DMiscCascadedAttributes* /*psCascadedAttribute*/) { return A3D_SUCCESS; }

	A3DStatus GetColorMaterialConnector(ColorMaterialsConnector& rsColorConnector, bool bUseInstances = false);

	A3DMiscCascadedAttributes* GetLastCascadedAttributes() const;

};

#endif
