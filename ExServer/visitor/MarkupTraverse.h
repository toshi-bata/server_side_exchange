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

#ifndef	 A3D_MARKUP_TRAVERSE
#define	 A3D_MARKUP_TRAVERSE

#include <vector>
#include "Connector.h"

/*########################################################################
					Markup	 Hierarchy
##########################################################################*/

class A3DVisitorContainer;
class A3DMkpAnnotationEntityConnector : public  A3DConnector
{
public:

	A3DMkpAnnotationEntityConnector(const A3DMkpAnnotationEntity *pAnnotEntity)
		: A3DConnector(pAnnotEntity) {}
	
	~A3DMkpAnnotationEntityConnector(){}
	
	A3DStatus TraverseAnnotationEntity(A3DVisitorContainer*  psVisitor);

};

class A3DMkpAnnotationSetConnector : public  A3DConnector
{
public:

	A3DMkpAnnotationSetConnector(const A3DMkpAnnotationSet *pAnnotSet) : A3DConnector(pAnnotSet) 
	{
		A3D_INITIALIZE_DATA( A3DMkpAnnotationSetData, m_sAnnotSetData);
		A3DMkpAnnotationSetGet(pAnnotSet,&m_sAnnotSetData);
	}

	~A3DMkpAnnotationSetConnector()
	{
		A3DMkpAnnotationSetGet(NULL, &m_sAnnotSetData);
	}

	A3DStatus TraverseAnnotationSet(A3DVisitorContainer*  pVisitor);

public :

	A3DMkpAnnotationSetData m_sAnnotSetData;
};

class A3DMkpAnnotationReferenceConnector :	public  A3DConnector
{
public:
	A3DMkpAnnotationReferenceConnector(const A3DMkpAnnotationReference *pAnnotReference) : A3DConnector(pAnnotReference) 
	{
		A3D_INITIALIZE_DATA( A3DMkpAnnotationReferenceData, m_sAnnotReferenceData);
		A3DMkpAnnotationReferenceGet(pAnnotReference,&m_sAnnotReferenceData);
	}

	~A3DMkpAnnotationReferenceConnector()
	{
		A3DMkpAnnotationReferenceGet(NULL, &m_sAnnotReferenceData);
	}

	A3DStatus TraverseAnnotationReference(A3DVisitorContainer*  pVisitor);
public :

	A3DMkpAnnotationReferenceData m_sAnnotReferenceData;
};

class A3DMkpAnnotationItemConnector : public  A3DConnector
{

public:

	A3DMkpAnnotationItemConnector(const A3DMkpAnnotationItem *pAnnotItem) : A3DConnector(pAnnotItem) 
	{
		A3D_INITIALIZE_DATA( A3DMkpAnnotationItemData, m_sAnnotItemData);
		A3DMkpAnnotationItemGet(pAnnotItem,&m_sAnnotItemData);
	}

	~A3DMkpAnnotationItemConnector()
	{
		A3DMkpAnnotationItemGet(NULL, &m_sAnnotItemData);
	}

	A3DStatus TraverseAnnotationItem(A3DVisitorContainer* pVisitor);

public:

	A3DMkpAnnotationItemData m_sAnnotItemData;
};

class A3DMarkupTessConnector;
class A3DMarkupConnector : public  A3DConnector
{

public:

	A3DMarkupConnector(const A3DMkpMarkup *pMarkup) : A3DConnector(pMarkup) 
	{
		A3D_INITIALIZE_DATA( A3DMkpMarkupData, m_sMarkupData);
		A3DMkpMarkupGet(pMarkup,&m_sMarkupData);
	}

	~A3DMarkupConnector()
	{
		A3DMkpMarkupGet(NULL, &m_sMarkupData);
	}

	A3DStatus TraverseMarkup(A3DVisitorContainer* pVisitor);
	A3DStatus TraverseMarkupTessellation(std::vector<A3DMarkupTessConnector*>& asMarkupTessConnector,
										 double a_dParallelToScreenPMIScale);

public:

	A3DMkpMarkupData m_sMarkupData;

};


class A3DMarkupDimensionConnector : public  A3DConnector
{

public:

	A3DMarkupDimensionConnector(const A3DMarkupDimension *pMarkup) : A3DConnector(pMarkup) 
	{
		A3D_INITIALIZE_DATA( A3DMarkupDimensionData, m_sDimensionData);
		A3DMarkupDimensionGet(pMarkup,&m_sDimensionData);
	}

	~A3DMarkupDimensionConnector()
	{
		A3DMarkupDimensionGet(NULL, &m_sDimensionData);
	}

	A3DStatus TraverseDimension(A3DVisitorContainer* pVisitor);

public:

	A3DMarkupDimensionData m_sDimensionData;
};

class A3DMarkupDatumConnector : public  A3DConnector
{

public:

	A3DMarkupDatumConnector(const A3DMarkupDatum *pMarkup) : A3DConnector(pMarkup) 
	{
		A3D_INITIALIZE_DATA( A3DMarkupDatumData, m_sDatumData);
		A3DMarkupDatumGet(pMarkup,&m_sDatumData);
	}

	~A3DMarkupDatumConnector()
	{
		A3DMarkupDatumGet(NULL, &m_sDatumData);
	}

	A3DStatus TraverseDatum(A3DVisitorContainer* pVisitor);

public:

	A3DMarkupDatumData m_sDatumData;
};

class A3DMarkupGDTConnector : public  A3DConnector
{

public:

	A3DMarkupGDTConnector(const A3DMarkupGDT *pMarkup) : A3DConnector(pMarkup) 
	{
		A3D_INITIALIZE_DATA( A3DMarkupGDTData, m_sGDTData);
		A3DMarkupGDTGet(pMarkup,&m_sGDTData);
	}

	~A3DMarkupGDTConnector()
	{
		A3DMarkupGDTGet(NULL, &m_sGDTData);
	}

	A3DStatus TraverseGDT(A3DVisitorContainer* pVisitor);

public:

	A3DMarkupGDTData m_sGDTData;
};

#endif //A3D_MARKUP_TRAVERSE