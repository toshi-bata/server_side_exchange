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

#ifndef	  A3D_TREE_CONNECTOR
#define	 A3D_TREE_CONNECTOR
	
#include <vector>
#include "Connector.h"
#include <A3DSDKStructure.h>



class A3DVisitorContainer;
 class A3DModelFileConnector	: public A3DConnector
{
public :

	A3DModelFileConnector(const A3DAsmModelFile* pModelFile) :  A3DConnector(pModelFile)
	{
		A3D_INITIALIZE_DATA( A3DAsmModelFileData, m_sModelFileData);
		A3DAsmModelFileGet(pModelFile, &m_sModelFileData);
	}

	~A3DModelFileConnector()	{ A3DAsmModelFileGet(NULL,  &m_sModelFileData); }	

	A3DStatus Traverse(A3DVisitorContainer* psVisitor, bool bVisitPrototype = false);

public :

	A3DAsmModelFileData m_sModelFileData;
};


class A3DProductOccurrenceConnector	: public A3DConnector
{
	friend class A3DModelFileConnector;
	const A3DAsmProductOccurrence* m_pFather;
	bool m_bIsInstanciated;
	bool m_bIsPrototype;
	bool m_bIsExternal;

private :

	A3DProductOccurrenceConnector(const A3DAsmProductOccurrence* pProductOccurrence);
	~A3DProductOccurrenceConnector();

	A3DStatus TraversePO(const A3DAsmProductOccurrence* pOccurrence, A3DVisitorContainer* psVisitor, bool bVisitPrototype = false);

	void SetProductOccurrenceFather(const A3DAsmProductOccurrence* pFather)	{ m_pFather = pFather; }

public :
	A3DStatus GetPart(A3DAsmPartDefinition*& pPart) const;
	A3DStatus CollectSons(std::vector<A3DAsmProductOccurrence*>& apSons) const;

	A3DAsmProductOccurrenceData		 m_sProductOccurrenceData;
	A3DAsmProductOccurrenceDataSLW	*m_pProductOccurrenceDataSLW;
	A3DAsmProductOccurrenceDataCat	*m_pProductOccurrenceDataCat;
	A3DAsmProductOccurrenceDataCV5	*m_pProductOccurrenceDataCV5;
	A3DAsmProductOccurrenceDataUg	*m_pProductOccurrenceDataUg;
	A3DAsmProductOccurrenceDataProe *m_pProductOccurrenceDataProe;
	A3DAsmProductOccurrenceDataInv	*m_pProductOccurrenceDataInv;
	A3DAsmProductOccurrenceDataJT	*m_pProductOccurrenceDataJT;

	const A3DAsmProductOccurrence* GetProductOccurrenceFather() const		{ return m_pFather; }
	void SetIsInstanciated(bool bIsInstanciated)					{ m_bIsInstanciated = bIsInstanciated; }
	bool IsInstanciated() const  { return m_bIsInstanciated; }
	void SetPrototypeType(bool bIsPrototype) { m_bIsPrototype = bIsPrototype; }
	bool IsProtoType() const { return m_bIsPrototype; }
	void SetExternalType(bool bIsExternal) { m_bIsExternal = bIsExternal; }
	bool IsExternal() const { return m_bIsExternal; }

};


class A3DPartConnector : public  A3DConnector
{
	friend class A3DProductOccurrenceConnector;
	const A3DAsmProductOccurrence* m_pFather;	
	bool m_bIsInstanciated;


private:

	A3DPartConnector(const A3DAsmPartDefinition *pPart) : A3DConnector(pPart) 
	{
		m_pFather = NULL;
		m_bIsInstanciated = false;
		A3D_INITIALIZE_DATA(A3DAsmPartDefinitionData, m_sPartData);
		A3DAsmPartDefinitionGet(pPart, &m_sPartData);
	}

	~A3DPartConnector()
	{
		A3DAsmPartDefinitionGet(NULL, &m_sPartData);
	}


	void SetProductOccurrenceFather(const A3DAsmProductOccurrence* pFather)	{ m_pFather = pFather; }

public :
	A3DStatus TraversePart(A3DVisitorContainer*  psVisitor) const;
	A3DAsmPartDefinitionData m_sPartData;
	const A3DAsmProductOccurrence* GetProductOccurrenceFather()	const	{ return m_pFather; }
	void SetIsInstanciated(bool bIsInstanciated)  { m_bIsInstanciated = bIsInstanciated; }
    bool IsInstanciated()  { return m_bIsInstanciated; }

};

class A3DRiConnector : public  A3DConnector
{
	friend class A3DPartConnector;
	friend class A3DRiSetConnector;
	friend class A3DFRMFeatureTreeConnector;
	bool m_bIsInstanciated;
private:
	
	A3DRiConnector(const A3DRiRepresentationItem *pRi) : A3DConnector(pRi)
	{
		m_bIsInstanciated = false;
		A3D_INITIALIZE_DATA(A3DRiRepresentationItemData, m_sRiData);
		A3DRiRepresentationItemGet(pRi, &m_sRiData);
	}

	~A3DRiConnector()
	{
		A3DRiRepresentationItemGet(NULL, &m_sRiData);
	}

	A3DStatus TraverseRi(A3DVisitorContainer*  psVisitor);

public :

	A3DRiRepresentationItemData m_sRiData;
	void SetIsInstanciated(bool bIsInstanciated)  {  m_bIsInstanciated = bIsInstanciated; }
	bool IsInstanciated()  { return m_bIsInstanciated; }
};


class A3DRiBrepModelConnector : public  A3DConnector
{
	friend class A3DRiConnector;
public :
	
	A3DRiBrepModelConnector(const A3DRiBrepModel *pRi) : m_pRiBrepModel(pRi), A3DConnector(pRi)
	{
		A3D_INITIALIZE_DATA(A3DRiBrepModelData, m_sRiBrepModelData);		
		A3DRiBrepModelGet(m_pRiBrepModel, &m_sRiBrepModelData);
	}
	
	~A3DRiBrepModelConnector()
	{
		A3DRiBrepModelGet(NULL, &m_sRiBrepModelData);
	}

	A3DStatus TraverseRiBrepModel(A3DVisitorContainer*  psVisitor) const;

public :
	A3DRiBrepModel const* m_pRiBrepModel;
	A3DRiBrepModelData m_sRiBrepModelData;
};
 


class A3DRiSetConnector : public  A3DConnector
{
	friend class A3DRiConnector;
private:
	
	A3DRiSetConnector(const A3DRiSet *pRi) : A3DConnector(pRi)
	{
		A3D_INITIALIZE_DATA(A3DRiSetData, m_sRiSetData);
		A3DRiSetGet(pRi, &m_sRiSetData);
	}

	~A3DRiSetConnector()
	{
		A3DRiSetGet(NULL, &m_sRiSetData);
	}

	A3DStatus TraverseRiSet(A3DVisitorContainer*  psVisitor);

public :

	A3DRiSetData m_sRiSetData;
};

class A3DPolyRiBrepModelConnector : public A3DConnector
{
	friend class A3DRiConnector;
private:

	A3DPolyRiBrepModelConnector(const A3DRiPolyBrepModel* pRi) : A3DConnector(pRi)
	{
		A3D_INITIALIZE_DATA(A3DRiPolyBrepModelData, m_sRiPolyBrepModelData);
		A3DRiPolyBrepModelGet(pRi, &m_sRiPolyBrepModelData);
	}

	~A3DPolyRiBrepModelConnector()
	{
		A3DRiPolyBrepModelGet(NULL, &m_sRiPolyBrepModelData);
	}



public:
	A3DStatus TraverseRiPolyBrepModel(A3DVisitorContainer* psVisitor) const;
	A3DRiPolyBrepModelData m_sRiPolyBrepModelData;
};


class A3DFRMParameterConnector : public  A3DConnector
{
	friend class A3DFRMFeatureTreeConnector;
	friend class A3DFRMFeatureConnector;
private:

	A3DFRMParameterConnector(const A3DFRMParameter *pParameter) : A3DConnector(pParameter)
	{
		A3D_INITIALIZE_DATA(A3DFRMParameterData, m_sData);
		A3DFRMParameterGet (pParameter, &m_sData);
	}

	~A3DFRMParameterConnector()
	{
		A3DFRMParameterGet (NULL, &m_sData);
	}


public:

	A3DFRMParameterData m_sData;

	A3DStatus TraverseParameter(A3DVisitorContainer*  psVisitor) const;
};


class A3DFRMFeatureLinkedItemConnector : public  A3DConnector
{
	friend class A3DFRMFeatureConnector;
private:

	A3DFRMFeatureLinkedItemConnector(const A3DFRMLinkedItem *pLinkedItem) : A3DConnector(pLinkedItem)
	{
		A3D_INITIALIZE_DATA(A3DFRMLinkedItemData, m_sData);
		A3DFRMLinkedItemGet (pLinkedItem, &m_sData);
	}

	~A3DFRMFeatureLinkedItemConnector()
	{
		A3DFRMLinkedItemGet (NULL, &m_sData);
	}


public:

	A3DFRMLinkedItemData m_sData;

	A3DStatus TraverseConnection(A3DVisitorContainer*  psVisitor) const;
};



class A3DFRMFeatureConnector : public  A3DConnector
{
	friend class A3DFRMParameterConnector;
private:

	A3DFRMFeatureConnector(const A3DFRMFeature *pFeature) : A3DConnector(pFeature)
	{
		A3D_INITIALIZE_DATA(A3DFRMFeatureData, m_sData);
		A3DFRMFeatureGet (pFeature, &m_sData);
	}

	~A3DFRMFeatureConnector()
	{
		A3DFRMFeatureGet (NULL, &m_sData);
	}


public:

	A3DFRMFeatureData m_sData;
	A3DStatus TraverseFeature(A3DVisitorContainer*  psVisitor) const;


};

class A3DFRMFeatureTreeConnector : public  A3DConnector
{
	friend class A3DProductOccurrenceConnector;
protected:

	A3DFRMFeatureTreeConnector(const A3DFRMTree *pTree) : A3DConnector(pTree)
	{
		A3D_INITIALIZE_DATA(A3DFRMTreeData, m_sTreeData);
		A3DFRMTreeGet (pTree, &m_sTreeData);
	}

	~A3DFRMFeatureTreeConnector()
	{
		A3DFRMTreeGet (NULL, &m_sTreeData);
	}

	A3DStatus TraverseFeatureTree(A3DVisitorContainer*  psVisitor) const;

public:

	A3DFRMTreeData m_sTreeData;
};
#endif
