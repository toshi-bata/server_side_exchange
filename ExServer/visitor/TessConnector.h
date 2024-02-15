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

#ifndef	 A3D_TESS_CONNECTOR
#define	 A3D_TESS_CONNECTOR
	
#include <vector>
#include "Connector.h"

class A3DVisitorContainer;
class A3DTessDataConnector : public A3DConnector
{
	A3DTess3DData m_sTessData;
	A3DTessBaseData m_sBaseTessData;

public :
	
	A3DTessDataConnector(const A3DTess3D* pTess3D);
	~A3DTessDataConnector();

	A3DStatus Traverse(A3DVisitorContainer* psVisitor);

	unsigned FacesSize() const
	{
		return m_sTessData.m_uiFaceTessSize;
	}
	
	A3DStatus Points(double*& pPoint, unsigned int& uSize)	const;
	
	A3DStatus Normals(double*& pNormal, unsigned int& uSize)	const;

	A3DStatus UV(double*& pUV, unsigned int& uSize) const;
	
	A3DTessBaseData const* GetBaseTessData() const { return &m_sBaseTessData; }
	A3DTess3DData const* GetTess3DData() const { return &m_sTessData; }

	A3DStatus IndicesAsTriangle(
					std::vector<unsigned>& auTriangleWithPoint_Normals_Indices,	  //normal, point,normal, point,..., point
					std::vector<unsigned>& auTriangleWithPoint_Normals_UV_Indices //normal, point, UV, normal, point,UV, ..., normal,point, UV
					) const;

	A3DStatus IndicesPerFaceAsTriangle(
					const unsigned& uFaceIndice,
					std::vector<unsigned>& auTriangleWithPoint_Normals_Indices,
					std::vector<unsigned>& auTriangleWithPoint_Normals_UV_Indices) const;

	A3DStatus Indices(	
					std::vector<unsigned>& auTrianglesOneNormal,			//(normal, point, point,..., point)
					std::vector<unsigned>& auFanOneNormal,
					std::vector<unsigned>& auStripeOneNormal,
					std::vector<unsigned>& auTrianglesWithNormals,		//normal, point, ... , normal, point
					std::vector<unsigned>& auFanWithNormals,	   
					std::vector<unsigned>& auStripeWithNormal,	
					std::vector<unsigned>& auTrianglesOneNormalTexture,	//normal,{texture...},point,{texture...},point,{texture...},point
					std::vector<unsigned>& auFanOneNormalTextured,		   
					std::vector<unsigned>& auStripeOneNormalTextured,
					std::vector<unsigned>& auTrianglesWithNormalsTextured, //normal,{texture...},point,...,normal,{texture...},point
					std::vector<unsigned>& auFanWithNormalsTextured,
					std::vector<unsigned>& auStripeWithNormalsTextured) const;

	A3DStatus IndicesPerFace(
					const unsigned& uFaceIndice,
					std::vector<unsigned>& auTrianglesOneNormal,		 //(normal, point, point,..., point)
					std::vector<unsigned>& auFanOneNormal,
					std::vector<unsigned>& auStripeOneNormal,
					std::vector<unsigned>& auTrianglesWithNormals,		 //normal, point, ... , normal, point
					std::vector<unsigned>& auFanWithNormals,	   
					std::vector<unsigned>& auStripeWithNormal,	
					std::vector<unsigned>& auTrianglesOneNormalTexture,	  //normal,{texture...},point,{texture...},point,{texture...},point
					std::vector<unsigned>& auFanOneNormalTextured,		   
					std::vector<unsigned>& auStripeOneNormalTextured,
					std::vector<unsigned>& auTrianglesWithNormalsTextured,//normal,{texture...},point,...,normal,{texture...},point
					std::vector<unsigned>& auFanWithNormalsTextured,
					std::vector<unsigned>& auStripeWithNormalsTextured) const;
};


class A3DFaceTessDataConnector : public A3DConnector
{
	const A3DTessDataConnector*	m_pTessConnector;
	unsigned								m_uCurrentFaceIndex;

public :

	A3DFaceTessDataConnector(const A3DTessFaceData* pFaceTess3D, unsigned uCurrentFaceIndex, const A3DTessDataConnector* pTessConnector);
	~A3DFaceTessDataConnector();

	const A3DTessDataConnector* GetTessDataConnector() const { return m_pTessConnector; }
	unsigned GetFaceIndex() const { return m_uCurrentFaceIndex; }
	unsigned GetFaceNumber() const { return m_pTessConnector != NULL ? m_pTessConnector->FacesSize() : 0; }

	
	A3DStatus Traverse(A3DVisitorContainer* psVisitor);

	A3DStatus IndicesAsTriangle(
		std::vector<unsigned>& auTriangleWithPoint_Normals_Indices,	  //normal, point,normal, point,..., point
		std::vector<unsigned>& auTriangleWithPoint_Normals_UV_Indices //normal, point, UV, normal, point,UV, ..., normal,point, UV
		) const;

	A3DStatus ColorsAsTriangle(std::vector<A3DUns8>& auTriangleWithPoint_Colors, bool &bIsRGBA) const;

	A3DStatus Indices(	
		std::vector<unsigned>& auTrianglesOneNormal,			//(normal, point, point,..., point)
		std::vector<unsigned>& auFanOneNormal,
		std::vector<unsigned>& auStripeOneNormal,
		std::vector<unsigned>& auTrianglesWithNormals,		//normal, point, ... , normal, point
		std::vector<unsigned>& auFanWithNormals,	   
		std::vector<unsigned>& auStripeWithNormal,	
		std::vector<unsigned>& auTrianglesOneNormalTexture,	//normal,{texture...},point,{texture...},point,{texture...},point
		std::vector<unsigned>& auFanOneNormalTextured,		   
		std::vector<unsigned>& auStripeOneNormalTextured,
		std::vector<unsigned>& auTrianglesWithNormalsTextured, //normal,{texture...},point,...,normal,{texture...},point
		std::vector<unsigned>& auFanWithNormalsTextured,
		std::vector<unsigned>& auStripeWithNormalsTextured) const;

	unsigned GetWireNumber() const;
	A3DStatus GetEdgeIndices(unsigned uEdgeIndice, std::vector<unsigned>& auEdgeIndice) const;
};


class A3DWireTessDataConnector : public A3DConnector
{

	A3DTess3DWireData m_sWire3DTessData;
	A3DTessBaseData m_sBaseTessData;

public:

	A3DWireTessDataConnector(const A3DTess3DWire* psTessWire);
	~A3DWireTessDataConnector();

	A3DStatus Traverse(A3DVisitorContainer* psVisitor);

	A3DStatus Points(double*& pPoint, unsigned int& uSize) const;
	A3DStatus Indices(std::vector<unsigned>& auPolylineIndices) const;
};

#endif

