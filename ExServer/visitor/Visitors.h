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

#ifndef A3D_VISITOR
#define A3D_VISITOR

#define CONNECT_TRANSFO			0x0001
#define CONNECT_COLORS			0x0002
#define CONNECT_MESH			0x0004
#define CONNECT_ASSEMBLY_TREE	0x008
#define CONNECT_BREP			0x0010
#define CONNECT_PMI				0x00020
#define CONNECT_VIEWS			0x00040
#define CONNECT_FEATURE			0x00080


#include "TreeTraverse.h"
#ifdef CONNECT_PMI
#include "MarkupTraverse.h"
#endif
#ifdef CONNECT_BREP
#include "BrepTraverse.h"
#endif
#ifdef CONNECT_PMI
#include "MarkupTraverse.h"
#endif
#ifdef CONNECT_MESH
#include "TessConnector.h"
#endif
#ifdef CONNECT_VIEWS
#include "ViewTraverse.h"
#endif

#include <string>

#ifndef CHECK_RET 
#define CHECK_RET(function) { if ((iRet = function)!=A3D_SUCCESS) { /*__debugbreak();*/ return iRet; }}	
#endif


class A3DConnector;
class A3DVisitorContainer;
class A3DVisitor
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)	// Suppress the warning C4251 because it leads to have that kind of warning : "warning 
	// C4251: 'A3DVisitorColorMaterials::m_apsCascadedAttribute' : class 'std::vector<_Ty>' needs to have dll-interface to 
	// be used by clients of class 'A3DVisitorColorMaterials"
	// This is because of the "dllexport" on the visitor classes, as they use stl members and it seems that stl causes 
	// warning with dllexport with microsoft compiler
#endif // _MSC_VER

protected:
	A3DVisitorContainer* m_psContainer;
	std::string				m_strName;

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

public :
	A3DVisitor(std::string strName, A3DVisitorContainer* psContainer = NULL)
		: m_strName(strName), m_psContainer(psContainer){}

	virtual ~A3DVisitor() {}

	virtual std::string GetName() const { return m_strName; }

	//Assembly
#ifdef CONNECT_ASSEMBLY_TREE
	virtual A3DStatus visitEnter(const A3DModelFileConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DModelFileConnector& /*sConnector*/) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DProductOccurrenceConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DProductOccurrenceConnector& /*sConnector*/) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DPartConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DPartConnector& /*sConnector*/) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DRiConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DRiConnector& /*sConnector*/) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DRiBrepModelConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DRiBrepModelConnector& /*sConnector*/) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DRiSetConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DRiSetConnector& /*sConnector*/) { return A3D_SUCCESS;}
	
	virtual A3DStatus visitEnter(const A3DPolyRiBrepModelConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DPolyRiBrepModelConnector& /*sConnector*/) { return A3D_SUCCESS; }

#endif
	//Markup
#ifdef CONNECT_PMI
	virtual A3DStatus visitEnter(const A3DMkpAnnotationEntityConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DMkpAnnotationEntityConnector& /*sConnector*/) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DMkpAnnotationSetConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DMkpAnnotationSetConnector& /*sConnector*/) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DMkpAnnotationItemConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DMkpAnnotationItemConnector& /*sConnector*/) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DMarkupConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DMarkupConnector& /*sConnector*/) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DMarkupDimensionConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DMarkupDimensionConnector& /*sConnector*/) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DMarkupGDTConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DMarkupGDTConnector& /*sConnector*/) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DMarkupDatumConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DMarkupDatumConnector& /*sConnector*/) { return A3D_SUCCESS; }
#endif
	//Brep
#ifdef CONNECT_BREP
	virtual A3DStatus visitEnter(const A3DBrepDataConnector&) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DBrepDataConnector&) { return A3D_SUCCESS; }
	
	virtual A3DStatus visitEnter(const A3DConnexConnector&)   { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DConnexConnector&)   { return A3D_SUCCESS; }
	
	virtual A3DStatus visitEnter(const A3DShellConnector&)    { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DShellConnector&)    { return A3D_SUCCESS; }
	
	virtual A3DStatus visitEnter(const A3DFaceConnector&)	  { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DFaceConnector&)	  { return A3D_SUCCESS; }
	
	virtual A3DStatus visitEnter(const A3DLoopConnector&)	  { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DLoopConnector&)	  { return A3D_SUCCESS; }
	
	virtual A3DStatus visitEnter(const A3DCoEdgeConnector&) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DCoEdgeConnector&) { return A3D_SUCCESS; }
	
	virtual A3DStatus visitEnter(const A3DEdgeConnector&)	  { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DEdgeConnector&)	  { return A3D_SUCCESS; }
	
	virtual A3DStatus visitEnter(const A3DUniqueVertexConnector&)   { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DUniqueVertexConnector&)   { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DMultipleVertexConnector&)   { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DMultipleVertexConnector&)   { return A3D_SUCCESS; }
#endif
	//Mesh
#ifdef CONNECT_MESH
	virtual A3DStatus visitEnter(const A3DTessDataConnector&)   { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DTessDataConnector&)   { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DFaceTessDataConnector&) {return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DFaceTessDataConnector&) {return A3D_SUCCESS; }
#endif
	//Views
#ifdef CONNECT_VIEWS
	virtual A3DStatus visitEnter(const A3DMkpViewConnector&)   { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DMkpViewConnector&)   { return A3D_SUCCESS; }	
#endif
#ifdef CONNECT_FEATURE
	virtual A3DStatus visitEnter(const A3DFRMFeatureConnector&  /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DFRMFeatureConnector&  /*sConnector*/) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DFRMParameterConnector&  /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DFRMParameterConnector&  /*sConnector*/) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DFRMFeatureTreeConnector&  /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DFRMFeatureTreeConnector&  /*sConnector*/) { return A3D_SUCCESS; }

	virtual A3DStatus visitEnter(const A3DFRMFeatureLinkedItemConnector&  /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DFRMFeatureLinkedItemConnector&  /*sConnector*/) { return A3D_SUCCESS; }
#endif
};
 

#endif
