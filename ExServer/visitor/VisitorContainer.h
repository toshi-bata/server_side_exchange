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

#ifndef A3DVISITOR_CONTAINER
#define A3DVISITOR_CONTAINER
#include <map>
#include "Visitors.h"

//This class allow to use several vistor when traversing 3DX Tree.
//It allow also to exchange parameters between visitors

#define NODE_INSTANCES 0x0001

class A3DStepEntityRefManager
{
public:
	unsigned int				m_uiPOLevel;	// Level of creation
	unsigned int				m_uiPushLevel;	// if not ~0U , then is activated
	bool							m_bRefOnTopoItem;;
	A3DMiscEntityReference* m_pStepEntityRef;
	A3DEntity*					m_pA3DEntityRef;
	//	std::vector<void*>		m_apYourEntity;

	A3DStepEntityRefManager() : m_uiPOLevel(~0U), m_uiPushLevel(~0U), m_pStepEntityRef(NULL), m_bRefOnTopoItem(false),
		m_pA3DEntityRef(NULL){}
	void GetMatrix();

	bool IsFirst( A3DStepEntityRefManager const & sOther) const { return (m_uiPOLevel<sOther.m_uiPOLevel) || (m_uiPOLevel==sOther.m_uiPOLevel && m_uiPushLevel>sOther.m_uiPushLevel);}
};



class A3DViewLinkedItemManager

{
public:
	unsigned int					m_uiPOLevel;
	unsigned int					m_uiPushLevel;	// if not ~0U , then is activated
	bool								m_bRefOnTopoItem;
	A3DMiscMarkupLinkedItem *	m_pMarkupLinkedItem;
	A3DAsmProductOccurrence *	m_POOwner;
	A3DEntity *						m_pA3DEntityRef;
	//	std::vector<void*>			m_apYourEntity;

	A3DViewLinkedItemManager() : m_uiPOLevel(~0U), m_uiPushLevel(~0U), m_pMarkupLinkedItem(NULL),m_bRefOnTopoItem(false),
		m_pA3DEntityRef(NULL){}
	void GetMatrix();

	bool IsFirst( A3DViewLinkedItemManager const & sOther) const { return (m_uiPOLevel<sOther.m_uiPOLevel) || (m_uiPOLevel==sOther.m_uiPOLevel && m_uiPushLevel>sOther.m_uiPushLevel);}
};

class A3DVisitorContainer  : public A3DVisitor
{
#ifdef _MSC_VER

#pragma warning(push)
#pragma warning(disable:4251)	// Suppress the warning C4251 because it leads to have that kind of warning : "warning 
// C4251: 'A3DVisitorColorMaterials::m_apsCascadedAttribute' : class 'std::vector<_Ty>' needs to have dll-interface to 
// be used by clients of class 'A3DVisitorColorMaterials"
// This is because of the "dllexport" on the visitor classes, as they use stl members and it seems that stl causes 
// warning with dllexport with microsoft compiler
#endif // _MSC_VER

	std::map<const A3DEntity*, void*>	m_apA3DEntityYourEntityMap;
	std::vector<A3DVisitor*>			m_apVisitor;
	bool m_bTraverseInstance;

	A3DVisitor*							m_pTreeVisitor;
	A3DAsmProductOccurrence const *		m_pCurrentPOFather;
	A3DMkpView const *					m_psActivatedView;
	bool								m_bTraverseActivatedViewOnly;

	std::vector<A3DStepEntityRefManager>	m_asStepEntityRefManager;
	std::vector<A3DViewLinkedItemManager>	m_asViewLinkedItemManager; 
	unsigned int							m_uiCurrentLevel;
	unsigned int							m_uFlagElementToConnect;

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

public:
	A3DVisitorContainer(unsigned int uFlagElementToconnect =
		CONNECT_TRANSFO | CONNECT_COLORS | CONNECT_MESH | CONNECT_ASSEMBLY_TREE | CONNECT_BREP | CONNECT_PMI | CONNECT_VIEWS| CONNECT_FEATURE);

	virtual ~A3DVisitorContainer();

	void push(	A3DVisitor* psVisitor);
	
	//A3DConnector
	virtual A3DStatus visitEnter(const A3DConnector& /*sConnector*/) { return A3D_SUCCESS; }
	virtual A3DStatus visitLeave(const A3DConnector& /*sConnector*/) { return A3D_SUCCESS; }

	//Assmebly Tree
#ifdef CONNECT_ASSEMBLY_TREE
	virtual A3DStatus visitEnter(const A3DModelFileConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DModelFileConnector& sConnector);
	
	virtual A3DStatus visitEnter(const A3DProductOccurrenceConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DProductOccurrenceConnector& sConnector);	
	
	virtual A3DStatus visitEnter(const A3DPartConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DPartConnector& sConnector);	
  	
	virtual A3DStatus visitEnter(const A3DRiConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DRiConnector& sConnector);


	virtual A3DStatus visitEnter(const A3DRiBrepModelConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DRiBrepModelConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DRiSetConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DRiSetConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DPolyRiBrepModelConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DPolyRiBrepModelConnector& sConnector);

#endif
#ifdef CONNECT_PMI
	//Markup Tree
	virtual A3DStatus visitEnter(const A3DMkpAnnotationEntityConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DMkpAnnotationEntityConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DMkpAnnotationSetConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DMkpAnnotationSetConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DMkpAnnotationItemConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DMkpAnnotationItemConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DMarkupConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DMarkupConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DMarkupDimensionConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DMarkupDimensionConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DMarkupGDTConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DMarkupGDTConnector& sConnector);
#endif	

#ifdef CONNECT_BREP
	//Brep
	virtual A3DStatus visitEnter(const A3DBrepDataConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DBrepDataConnector& sConnector);
	
	virtual A3DStatus visitEnter(const A3DConnexConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DConnexConnector& sConnector);
	
	virtual A3DStatus visitEnter(const A3DShellConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DShellConnector& sConnector);
	
	virtual A3DStatus visitEnter(const A3DFaceConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DFaceConnector& sConnector);
	
	virtual A3DStatus visitEnter(const A3DLoopConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DLoopConnector& sConnector);
	
	virtual A3DStatus visitEnter(const A3DCoEdgeConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DCoEdgeConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DEdgeConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DEdgeConnector& sConnector);
	
	virtual A3DStatus visitEnter(const A3DUniqueVertexConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DUniqueVertexConnector& sConnector);
	
	virtual A3DStatus visitEnter(const A3DMultipleVertexConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DMultipleVertexConnector& sConnector);
#endif
#ifdef CONNECT_MESH
	//Mesh
	virtual A3DStatus visitEnter(const A3DTessDataConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DTessDataConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DFaceTessDataConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DFaceTessDataConnector& sConnector);
	 
	virtual A3DStatus visitEnter(const A3DMarkupDatumConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DMarkupDatumConnector& sConnector);
#endif
	//Views
#ifdef CONNECT_VIEWS
	virtual A3DStatus visitEnter(const A3DMkpViewConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DMkpViewConnector& sConnector);
#endif
#ifdef CONNECT_FEATURE
	virtual A3DStatus visitEnter(const A3DFRMFeatureConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DFRMFeatureConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DFRMParameterConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DFRMParameterConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DFRMFeatureTreeConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DFRMFeatureTreeConnector& sConnector);

	virtual A3DStatus visitEnter(const A3DFRMFeatureLinkedItemConnector& sConnector);
	virtual A3DStatus visitLeave(const A3DFRMFeatureLinkedItemConnector& sConnector);
#endif
	void* FindInMap(const A3DEntity* pA3DEntity);
	void SetInMap(const A3DEntity* pA3DEntity, void* pYourEntity);
	void RemoveFromMap(const A3DEntity* pA3DEntity);

	std::vector<A3DVisitor*>& GetVisitor() { return m_apVisitor; }


	bool TraverseInstances() { return m_bTraverseInstance; }
	void SetTraverseInstance(bool bTraverseInstance) { m_bTraverseInstance = bTraverseInstance; }

	A3DVisitor* GetVisitorByName(std::string strName);
	A3DVisitor* GetTreeVisitor() const;

	void SetCurrentPoFather(A3DAsmProductOccurrence const * pCurrentPOFather) { m_pCurrentPOFather = pCurrentPOFather; }
	A3DAsmProductOccurrence const * GetCurrentPoFather() { return m_pCurrentPOFather; }

	void SetActivateView(A3DMkpView const * psActivatedView) { m_psActivatedView = psActivatedView; }
	A3DMkpView const * GetActivatedView() { return m_psActivatedView; }

	void SetTraverseActivatedViewOnly(bool bTraverseActivatedViewOnly) { m_bTraverseActivatedViewOnly = bTraverseActivatedViewOnly; }

	// StepEntityReference and ViewLinkedItem treatment
	A3DStepEntityRefManager const* GetActiveStepEntityRefManager(A3DEntity const * pEntity) const;
	A3DViewLinkedItemManager const* GetActiveViewLinkedItemManager(A3DEntity const * pEntity) const;
	
	void ActivateEntityReference( A3DEntity const* pEntity );
	void DeactivateEntityReference(A3DEntity const* pEntity);

	unsigned int GetFlagElementToConnect() const { return m_uFlagElementToConnect; }
};	
#endif
