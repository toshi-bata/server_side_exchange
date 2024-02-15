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
/**
\file TessConnector.cpp	
This file provide tools to get the tessellation of Cad Models

***********************************************************************************************************************/

#include "TessConnector.h"
#include "VisitorContainer.h"
#include <algorithm>

////////////////////////////////////////
//	A3DTessDataConnector
///////////////////////////////////////
A3DTessDataConnector::A3DTessDataConnector(const A3DTess3D* pTess3D) : A3DConnector(pTess3D) 
{
	A3D_INITIALIZE_DATA(A3DTess3DData, m_sTessData);
	A3DTess3DGet( pTess3D, &m_sTessData);
	
	A3D_INITIALIZE_DATA(A3DTessBaseData, m_sBaseTessData);
	A3DTessBaseGet((const A3DTessBase*) pTess3D,&m_sBaseTessData);
}

A3DTessDataConnector::~A3DTessDataConnector()
{
	A3DTess3DGet( NULL, &m_sTessData);
	A3DTessBaseGet(NULL, &m_sBaseTessData);
}

A3DStatus A3DTessDataConnector::Traverse(A3DVisitorContainer* psVisitor)
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));
	unsigned uNbFaces = FacesSize();

	for(unsigned uFace = 0; uFace < uNbFaces; uFace++)
	{
		A3DFaceTessDataConnector sFaceConnector(&m_sTessData.m_psFaceTessData[uFace], uFace, this);
		sFaceConnector.Traverse(psVisitor);
	}
	CHECK_RET(psVisitor->visitLeave(*this));
	return A3D_SUCCESS;
}

// Retrieve all the point of the Tess3DData
A3DStatus A3DTessDataConnector::Points(double*& pPoint, unsigned int& uSize) const
{
	pPoint = m_sBaseTessData.m_pdCoords;
	uSize = m_sBaseTessData.m_uiCoordSize;
	return A3D_SUCCESS;
}

// Retrieve all the normals of the Tess3DData
A3DStatus A3DTessDataConnector::Normals(double*& pNormal, unsigned int& uSize)	const
{
	pNormal = m_sTessData.m_pdNormals;
	uSize = m_sTessData.m_uiNormalSize;
	return A3D_SUCCESS;
}

// Retrieve all the textures of the Tess3DData
A3DStatus  A3DTessDataConnector::UV(double*& pUV, unsigned int& uSize) const
{
	pUV = m_sTessData.m_pdTextureCoords;
	uSize =  m_sTessData.m_uiTextureCoordSize;
	return A3D_SUCCESS;
}

#define COPY(dest,src,size_)	if(size_ != 0)	{\
		size_t uTempSize = dest.size();\
		dest.resize(uTempSize + size_);\
		std::copy(src ,src+ size_, dest.begin() + uTempSize);\
		src+= size_; \
		size_ = 0;\
	}

#define COPY_(dest,src,size_) /*if(size_)*/ {\
		size_t uTempSize = dest.size();\
		dest.resize(uTempSize + size_);\
		std::copy(src,src+size_, dest.begin() + uTempSize);\
	}


// Retrieve the index as triangle for each Face of the Tess3DData
A3DStatus A3DTessDataConnector::IndicesAsTriangle(
					std::vector<unsigned>& auTriangleWithPoint_Normals,
					std::vector<unsigned>& auTriangleWithPoint_Normals_UV_Indices) const
{	 
	A3DStatus iRet = A3D_SUCCESS;
	unsigned uFaceCount, uFaceSize = m_sTessData.m_uiFaceTessSize;
	for(uFaceCount = 0;uFaceCount < uFaceSize;	 uFaceCount++)
	{
		CHECK_RET(IndicesPerFaceAsTriangle(
					uFaceCount,
					auTriangleWithPoint_Normals,
					auTriangleWithPoint_Normals_UV_Indices));
	}
	return A3D_SUCCESS;
}

// Retrieve the index as triangle per face of the Tess3DData
A3DStatus A3DTessDataConnector::IndicesPerFaceAsTriangle(
					const unsigned& uFaceIndice,
					std::vector<unsigned>& auTriangleWithPoint_Normals_Indices,
					std::vector<unsigned>& auTriangleWithPoint_Normals_UV_Indices) const
{
	A3DTessFaceData* pFaceTessData =	&(m_sTessData.m_psFaceTessData[uFaceIndice]);

	if (!pFaceTessData->m_uiSizesTriangulatedSize)
		return A3D_SUCCESS;

	A3DUns32* puiTriangulatedIndexes = m_sTessData.m_puiTriangulatedIndexes 
									+ pFaceTessData->m_uiStartTriangulated;
	unsigned uiCurrentSize = 0;

	// Triangle One Normal Per vertex  
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangle)
	{
		unsigned uiNbIndice = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] * 6;
		COPY(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes, uiNbIndice);
	}

	// Fan One Normal per vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFan)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
		{	
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
			A3DUns32* pFanPointIndice = puiTriangulatedIndexes;
			puiTriangulatedIndexes+=2;
			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_Indices, pFanPointIndice, 2);
				COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes, 2);
				COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes+2, 2);
				puiTriangulatedIndexes+=2;
			}
			puiTriangulatedIndexes+=2;
		}
	}

	// stripe One normal per vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripe)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
		{
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
			puiTriangulatedIndexes+=2;
			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes,2);
				if(	uIPoint%2)
				{
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes - 2,2);
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 2,2);
				}
				else
				{
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 2,2);
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes - 2,2);
				}
				puiTriangulatedIndexes+=2;
			}
			puiTriangulatedIndexes+=2;
		}
	}

	//Triangle one Normal Per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormal)
	{
		A3DUns32 uNbTriangles = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++]/* & kA3DTessFaceDataNormalMask*/;
		for(A3DUns32 uI = 0; uI < uNbTriangles; uI++)
		{
			A3DUns32* pNormalIndice = puiTriangulatedIndexes;
			COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes,2);
			puiTriangulatedIndexes+=2;
			COPY_(auTriangleWithPoint_Normals_Indices, pNormalIndice,1);
			COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes,1);
			puiTriangulatedIndexes+=1;
			COPY_(auTriangleWithPoint_Normals_Indices, pNormalIndice,1);
			COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes,1);
			puiTriangulatedIndexes+=1;
		}
	}

	// Fan One normal per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormal)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
		{	
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			if(bIsOneNormal == false)
			{
				return A3D_ERROR;
			}
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
			A3DUns32* pFanPointIndice = puiTriangulatedIndexes;
			puiTriangulatedIndexes+=2;
			for (A3DUns32 uIPoint = 0; uIPoint < uiNbPoint-2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_Indices, pFanPointIndice, 2);
				
				COPY_(auTriangleWithPoint_Normals_Indices, pFanPointIndice, 1);
				COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes, 1);
			
				COPY_(auTriangleWithPoint_Normals_Indices, pFanPointIndice, 1);
				COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 1, 1);
				puiTriangulatedIndexes+=1;
			}
			puiTriangulatedIndexes++;
		}
	}

	// Stripe One normal per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormal)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
		{	
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			A3DUns32* pStripeNormalIndice = puiTriangulatedIndexes;
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
			puiTriangulatedIndexes+=2;

			// Is there only one normal for the entire stripe?
			if(bIsOneNormal == false)
			{
				A3DUns32 uIPoint;
				for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
				{
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes,2);
					if(	uIPoint%2)
					{
						COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes - 2,2);
						COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 2,2);
					}
					else
					{
						COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 2,2);
						COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes - 2,2);
					}
					puiTriangulatedIndexes+=2;
				}
				puiTriangulatedIndexes+=2;
				continue;
			}

			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_Indices, pStripeNormalIndice,1);
				COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes,1);
				if(uIPoint%2)
				{
					COPY_(auTriangleWithPoint_Normals_Indices, pStripeNormalIndice,1);
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes - 1,1);
					
					COPY_(auTriangleWithPoint_Normals_Indices, pStripeNormalIndice,1);
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 1,1);
				}
				else
				{
					COPY_(auTriangleWithPoint_Normals_Indices, pStripeNormalIndice,1);
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 1,1);
					
					COPY_(auTriangleWithPoint_Normals_Indices, pStripeNormalIndice,1);
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes - 1,1);
				}
				puiTriangulatedIndexes+=1;
			}
			puiTriangulatedIndexes+=1;
		}
	}

	// Textured triangle One normal per vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleTextured)
	{
		unsigned uiNbIndice = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] * 9;
		COPY(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes, uiNbIndice);
	}
	
	// Textured Fan One normal per vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanTextured)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
		{
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
			A3DUns32* pFanPointIndice = puiTriangulatedIndexes;
			puiTriangulatedIndexes+=3;
			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint-2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_UV_Indices, pFanPointIndice, 3);
				COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes, 3);
				COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes+3, 3);
				puiTriangulatedIndexes+=3;
			}
			puiTriangulatedIndexes+=3;
		}
	}

	//Textured Stripe One normal per vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeTextured)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
		{
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
			puiTriangulatedIndexes+=3;
			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes ,3);
				if(uIPoint%2)
				{
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes - 3,3);
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes + 3,3);
				}
				else
				{
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes + 3,3);
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes - 3,3);
				}
				puiTriangulatedIndexes+=3;
			}
			puiTriangulatedIndexes += 3;
		}
	}

	// Textured One normal per Triangle
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormalTextured)
	{
		A3DUns32 uNbTriangles = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		for(A3DUns32 uI = 0; uI < uNbTriangles; uI++)
		{
			A3DUns32* pNormalIndice = puiTriangulatedIndexes;
			COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes,3);
			puiTriangulatedIndexes+=3;
			COPY_(auTriangleWithPoint_Normals_UV_Indices, pNormalIndice,1);
			COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes,2);
			puiTriangulatedIndexes+=2;
			COPY_(auTriangleWithPoint_Normals_UV_Indices, pNormalIndice,1);
			COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes,2);
			puiTriangulatedIndexes+=2;
		}
	}
	
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormalTextured)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
		{
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			if(bIsOneNormal == false)
			{
				return A3D_ERROR;
			}
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
			A3DUns32* pFanPointIndice = puiTriangulatedIndexes;
			puiTriangulatedIndexes+=2;
			for (A3DUns32 uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_UV_Indices, pFanPointIndice, 3);
			
				COPY_(auTriangleWithPoint_Normals_UV_Indices, pFanPointIndice, 1);
				COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes, 2);

				COPY_(auTriangleWithPoint_Normals_UV_Indices, pFanPointIndice, 1);
				COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes + 2, 2);
				puiTriangulatedIndexes+=2;
			}
			puiTriangulatedIndexes+=2;
		}
	}

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormalTextured)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
		{
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			if(bIsOneNormal == false)
			{
				return A3D_ERROR;
			}
			A3DUns32* pStripeNormalIndice = puiTriangulatedIndexes;
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
			puiTriangulatedIndexes+=3;
			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_UV_Indices, pStripeNormalIndice,1)
				COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes,2);
				if(	uIPoint%2)
				{
					COPY_(auTriangleWithPoint_Normals_UV_Indices, pStripeNormalIndice,1)
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes - 2, 2);

					COPY_(auTriangleWithPoint_Normals_UV_Indices, pStripeNormalIndice,1)
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes + 2, 2);
				}
				else
				{
					COPY_(auTriangleWithPoint_Normals_UV_Indices, pStripeNormalIndice,1)
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes + 2, 2);

					COPY_(auTriangleWithPoint_Normals_UV_Indices, pStripeNormalIndice,1)
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes - 2, 2);
					
				}
				puiTriangulatedIndexes+=2;
			}
		}
	}
	
	return	 A3D_SUCCESS;
}

A3DStatus A3DTessDataConnector::IndicesPerFace(
					const unsigned& uFaceIndice,
					std::vector<unsigned>& /*auTrianglesOneNormal*/,			//(normal, point, point,..., point)
					std::vector<unsigned>& /*auFanOneNormal*/,
					std::vector<unsigned>& /*auStripeOneNormal*/,
					std::vector<unsigned>& auTrianglesWithNormals,			//normal, point, ... , normal, point
					std::vector<unsigned>& auFanWithNormals,	   
					std::vector<unsigned>& auStripeWithNormal,	
					std::vector<unsigned>& /*auTrianglesOneNormalTexture*/,		//normal,{texture...},point,{texture...},point,{texture...},point
					std::vector<unsigned>& /*auFanOneNormalTextured*/,		   
					std::vector<unsigned>& /*auStripeOneNormalTextured*/,
					std::vector<unsigned>& /*auTrianglesWithNormalsTextured*/,	//normal,{texture...},point,...,normal,{texture...},point
					std::vector<unsigned>& /*auFanWithNormalsTextured*/,
					std::vector<unsigned>& /*auStripeWithNormalsTextured*/) const
{
	A3DTessFaceData* pFaceTessData =	&(m_sTessData.m_psFaceTessData[uFaceIndice]);
	if (!pFaceTessData->m_uiSizesTriangulatedSize)
		return A3D_SUCCESS;

	A3DUns32 uiCurrentSize = 0;
	A3DUns32* puiTriangulatedIndexes = m_sTessData.m_puiTriangulatedIndexes 
									+ pFaceTessData->m_uiStartTriangulated;
    A3DUns32 uiNbIndice = 0;
	
	// One Normal Per Vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangle)
	{
		uiNbIndice = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] * 6;
		COPY(auTrianglesWithNormals, puiTriangulatedIndexes, uiNbIndice);
	}

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFan)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
			uiNbIndice+= pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++]*2;
	 	COPY(auFanWithNormals, puiTriangulatedIndexes, uiNbIndice);
	}
	
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripe)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
			uiNbIndice+= pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++]*2;
		COPY(auStripeWithNormal, puiTriangulatedIndexes, uiNbIndice);
	}

	//One Normal Per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormal)
	{
		uiNbIndice = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++]/* & kA3DTessFaceDataNormalMask*/) * 4;
	}
	COPY(auTrianglesWithNormals, puiTriangulatedIndexes, uiNbIndice);

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormal)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
		{
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			if(bIsOneNormal == false)
			{
				return A3D_ERROR;
			}
			uiNbIndice+= pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] + 1;
		}
		COPY(auFanWithNormals, puiTriangulatedIndexes, uiNbIndice);
	}

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormal)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
		{
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			if(bIsOneNormal == false)
			{
				return A3D_ERROR;
			}
			uiNbIndice+= pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] + 1;
		}
		COPY(auStripeWithNormal, puiTriangulatedIndexes, uiNbIndice);
	}
	
	// Textured
	//One Normal Per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleTextured)
		uiNbIndice = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] * 9;
	COPY(auTrianglesWithNormals, puiTriangulatedIndexes, uiNbIndice);

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanTextured)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
			uiNbIndice+= pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] * 3 ;
		COPY(auFanWithNormals, puiTriangulatedIndexes, uiNbIndice);
	}
	
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeTextured)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
			uiNbIndice+= pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] * 3;
		COPY(auStripeWithNormal, puiTriangulatedIndexes, uiNbIndice);
	}


	// Textured
	//One Normal Per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormalTextured)
	{
		uiNbIndice = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++]/* & kA3DTessFaceDataNormalMask*/) * 7;
	}
	COPY(auTrianglesWithNormals, puiTriangulatedIndexes, uiNbIndice);

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormalTextured)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
		{
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			if(bIsOneNormal == false)
			{
				return A3D_ERROR;
			}
			uiNbIndice+= (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask) * 2 + 1 ;
		}
		COPY(auFanWithNormals, puiTriangulatedIndexes, uiNbIndice);
	}

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormalTextured)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
		{
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			if(bIsOneNormal == false)
			{
				return A3D_ERROR;
			}
			uiNbIndice+= (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask) * 2 + 1;
		}
		COPY(auStripeWithNormal, puiTriangulatedIndexes, uiNbIndice);
	}

	return	 A3D_SUCCESS;
}

A3DStatus A3DTessDataConnector::Indices(	
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
					std::vector<unsigned>& auStripeWithNormalsTextured)	const
{
	A3DStatus iRet = A3D_SUCCESS;
	unsigned uFaceCount, uFaceSize = m_sTessData.m_uiFaceTessSize;
	for(uFaceCount = 0;uFaceCount < uFaceSize;	 uFaceCount++)
		CHECK_RET(IndicesPerFace(
					uFaceCount,
					auTrianglesOneNormal,		
					auFanOneNormal,
					auStripeOneNormal,
					auTrianglesWithNormals,		
					auFanWithNormals,	   
					auStripeWithNormal,	
					auTrianglesOneNormalTexture,		
					auFanOneNormalTextured,		   
					auStripeOneNormalTextured,
					auTrianglesWithNormalsTextured,	
					auFanWithNormalsTextured,
					auStripeWithNormalsTextured));
	return A3D_SUCCESS;
}

////////////////////////////////////////
//	A3DFaceTessDataConnector
///////////////////////////////////////
A3DFaceTessDataConnector::A3DFaceTessDataConnector(const A3DTessFaceData* pFaceTess3D,
																	unsigned uCurrentFaceIndex,
																	const A3DTessDataConnector* pTessConnector)
																	: A3DConnector(pFaceTess3D),
																	  m_pTessConnector(pTessConnector),
																	  m_uCurrentFaceIndex(uCurrentFaceIndex)
{
}

A3DFaceTessDataConnector::~A3DFaceTessDataConnector()
{
}

A3DStatus A3DFaceTessDataConnector::Traverse( A3DVisitorContainer* psVisitor )
{
	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));
	CHECK_RET(psVisitor->visitLeave(*this));
	return A3D_SUCCESS;
}


A3DStatus A3DFaceTessDataConnector::IndicesAsTriangle(
	std::vector<unsigned>& auTriangleWithPoint_Normals_Indices,
	std::vector<unsigned>& auTriangleWithPoint_Normals_UV_Indices) const
{
	A3DTessFaceData* pFaceTessData =	(A3DTessFaceData*)this->GetA3DEntity();
	if (!pFaceTessData->m_uiSizesTriangulatedSize)
		return A3D_SUCCESS;

	A3DUns32* puiTriangulatedIndexes = m_pTessConnector->GetTess3DData()->m_puiTriangulatedIndexes 
		+ pFaceTessData->m_uiStartTriangulated;
	unsigned uiCurrentSize = 0;

	// Triangle One Normal Per vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangle)
	{
		unsigned uiNbIndice = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++]) * 6;
		COPY(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes, uiNbIndice);
		puiTriangulatedIndexes += uiNbIndice;
	}

	// Fan One Normal per vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFan)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
		{	
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
			A3DUns32* pFanPointIndice = puiTriangulatedIndexes;
			puiTriangulatedIndexes+=2;
			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_Indices, pFanPointIndice, 2);
				COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes, 2);
				COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 2, 2);
				puiTriangulatedIndexes+=2;
			}
			puiTriangulatedIndexes+=2;
		}
	}

	// stripe One normal per vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripe)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
		{
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
			puiTriangulatedIndexes+=2;
			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes,2);
				if(	uIPoint%2)
				{
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes - 2,2);
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 2,2);
				}
				else
				{
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 2,2);
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes - 2,2);
				}
				puiTriangulatedIndexes+=2;
			}
			puiTriangulatedIndexes+=2;
		}
	}

	//Triangle one Normal Per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormal)
	{
		A3DUns32 uNbTriangles = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++]/* & kA3DTessFaceDataNormalMask*/;
		for(A3DUns32 uI = 0; uI < uNbTriangles; uI++)
		{
			A3DUns32* pNormalIndice = puiTriangulatedIndexes;
			COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes,2);
			puiTriangulatedIndexes+=2;
			COPY_(auTriangleWithPoint_Normals_Indices, pNormalIndice,1);
			COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes,1);
			puiTriangulatedIndexes+=1;
			COPY_(auTriangleWithPoint_Normals_Indices, pNormalIndice,1);
			COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes,1);
			puiTriangulatedIndexes+=1;
		}
	}

	// Fan One normal per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormal)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
		{	
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
			A3DUns32* pFanPointIndice = puiTriangulatedIndexes;
			puiTriangulatedIndexes+=1;

			if(bIsOneNormal == false)
			{
				A3DUns32 uIPoint;
				for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
				{
					COPY_(auTriangleWithPoint_Normals_Indices, pFanPointIndice, 2);
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes, 2);
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 2, 2);
					puiTriangulatedIndexes+=2;
				}
				puiTriangulatedIndexes+=2;
				continue;
			}

			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 1; uIPoint++)
			{			
				COPY_(auTriangleWithPoint_Normals_Indices, pFanPointIndice, 2);

				COPY_(auTriangleWithPoint_Normals_Indices, pFanPointIndice, 1);
				COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes, 1);

				COPY_(auTriangleWithPoint_Normals_Indices, pFanPointIndice, 1);
				COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 1, 1);
				puiTriangulatedIndexes+=1;
			}
			puiTriangulatedIndexes+=1;
		}
	}

	// Stripe One normal per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormal)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
		{	
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			A3DUns32* pStripeNormalIndice = puiTriangulatedIndexes;
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
			puiTriangulatedIndexes+=2;

 			if(bIsOneNormal == false)
 			{
				A3DUns32 uIPoint;
				for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
				{
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes,2);
					if(	uIPoint%2)
					{
						COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes - 2,2);
						COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 2,2);
					}
					else
					{
						COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 2,2);
						COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes - 2,2);
					}
					puiTriangulatedIndexes+=2;
				}
				puiTriangulatedIndexes+=2;
				continue;
 			}
			
			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_Indices, pStripeNormalIndice,1);
				COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes,1);
				if(uIPoint%2)
				{
					COPY_(auTriangleWithPoint_Normals_Indices, pStripeNormalIndice,1);
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes - 1,1);

					COPY_(auTriangleWithPoint_Normals_Indices, pStripeNormalIndice,1);
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 1,1);
				}
				else
				{
					COPY_(auTriangleWithPoint_Normals_Indices, pStripeNormalIndice,1);
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes + 1,1);

					COPY_(auTriangleWithPoint_Normals_Indices, pStripeNormalIndice,1);
					COPY_(auTriangleWithPoint_Normals_Indices, puiTriangulatedIndexes - 1,1);
				}
				puiTriangulatedIndexes+=1;
			}
			puiTriangulatedIndexes+=1;
		}
	}

	// Textured triangle One normal per vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleTextured)
	{
		unsigned uiNbIndice = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] * 9;
		COPY(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes, uiNbIndice);
		puiTriangulatedIndexes += uiNbIndice;
	}

	// Textured Fan One normal per vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanTextured)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
		{
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
			A3DUns32* pFanPointIndice = puiTriangulatedIndexes;
			puiTriangulatedIndexes+=3;
			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_UV_Indices, pFanPointIndice, 3);
				COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes, 3);
				COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes + 3, 3);
				puiTriangulatedIndexes+=3;
			}
			puiTriangulatedIndexes+=3;
		}
	}

	//Textured Stripe One normal per vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeTextured)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
		{
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
			puiTriangulatedIndexes+=3;
			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes ,3);
				if(uIPoint%2)
				{
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes - 3,3);
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes + 3,3);
				}
				else
				{
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes + 3,3);
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes - 3,3);
				}
				puiTriangulatedIndexes+=3;
			}
			puiTriangulatedIndexes+=3;
		}
	}

	// Textured One normal per Triangle
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormalTextured)
	{
		A3DUns32 uNbTriangles = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		for(A3DUns32 uI = 0; uI < uNbTriangles; uI++)
		{
			A3DUns32* pNormalIndice = puiTriangulatedIndexes;
			COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes,3);
			puiTriangulatedIndexes+=3;
			COPY_(auTriangleWithPoint_Normals_UV_Indices, pNormalIndice,1);
			COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes,2);
			puiTriangulatedIndexes+=2;
			COPY_(auTriangleWithPoint_Normals_UV_Indices, pNormalIndice,1);
			COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes,2);
			puiTriangulatedIndexes+=2;
		}
	}

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormalTextured)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
		{
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
			A3DUns32* pFanPointIndice = puiTriangulatedIndexes;
			puiTriangulatedIndexes+=2;

			if(bIsOneNormal == false)
			{
				A3DUns32 uIPoint;
				for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
				{
					COPY_(auTriangleWithPoint_Normals_UV_Indices, pFanPointIndice, 3);
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes, 3);
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes + 3, 3);
					puiTriangulatedIndexes+=3;
				}
				puiTriangulatedIndexes+=3;
				continue;
			}

			for (A3DUns32 uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_UV_Indices, pFanPointIndice, 3);

				COPY_(auTriangleWithPoint_Normals_UV_Indices, pFanPointIndice, 1);
				COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes, 2);

				COPY_(auTriangleWithPoint_Normals_UV_Indices, pFanPointIndice, 1);
				COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes + 2, 2);
				puiTriangulatedIndexes+=2;
			}
			puiTriangulatedIndexes+=2;
		}
	}

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormalTextured)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
		{	
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			A3DUns32* pStripeNormalIndice = puiTriangulatedIndexes;
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
			puiTriangulatedIndexes+=3;

			if(bIsOneNormal == false)
			{
				A3DUns32 uIPoint;
				for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
				{
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes ,3);
					if(uIPoint%2)
					{
						COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes - 3,3);
						COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes + 3,3);
					}
					else
					{
						COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes + 3,3);
						COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes - 3,3);
					}
					puiTriangulatedIndexes+=3;
				}
				puiTriangulatedIndexes+=3;
				continue;
			}

			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Normals_UV_Indices, pStripeNormalIndice,1)
				COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes,2);
				if(	uIPoint%2)
				{
					COPY_(auTriangleWithPoint_Normals_UV_Indices, pStripeNormalIndice,1)
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes - 2, 2);

					COPY_(auTriangleWithPoint_Normals_UV_Indices, pStripeNormalIndice,1)
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes + 2, 2);
				}
				else
				{
					COPY_(auTriangleWithPoint_Normals_UV_Indices, pStripeNormalIndice,1)
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes + 2, 2);

					COPY_(auTriangleWithPoint_Normals_UV_Indices, pStripeNormalIndice,1)
					COPY_(auTriangleWithPoint_Normals_UV_Indices, puiTriangulatedIndexes - 2, 2);

				}
				puiTriangulatedIndexes+=2;
			}
			puiTriangulatedIndexes+=2;
		}
	}

	return	 A3D_SUCCESS;
}

A3DStatus A3DFaceTessDataConnector::ColorsAsTriangle(std::vector<A3DUns8>& auTriangleWithPoint_Colors, bool &bIsRGBA) const
{
	A3DTessFaceData* pFaceTessData = (A3DTessFaceData*)this->GetA3DEntity();
	if (!pFaceTessData->m_uiSizesTriangulatedSize || pFaceTessData->m_uiRGBAVerticesSize == 0)
		return A3D_SUCCESS;

	A3DUns8* puiTriangulatedColors = pFaceTessData->m_pucRGBAVertices;
		//+ pFaceTessData->m_uiStartTriangulated;

	bIsRGBA = (pFaceTessData->m_bIsRGBA) > 0;
	unsigned uiColorComponents = bIsRGBA ? 4 : 3;
	unsigned uiCurrentSize = 0;

	// Triangle One Normal Per vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangle)
	{
		unsigned uiNbIndice = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++]) * 3 * uiColorComponents;
		COPY(auTriangleWithPoint_Colors, puiTriangulatedColors, uiNbIndice);
	}

	// Fan One Normal per vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFan)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
		{
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
			A3DUns8* pFanPointColors = puiTriangulatedColors;
			puiTriangulatedColors += uiColorComponents;
			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Colors, pFanPointColors, uiColorComponents);
				COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors, uiColorComponents);
				COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors + uiColorComponents, uiColorComponents);
				puiTriangulatedColors += uiColorComponents;
			}
			puiTriangulatedColors += uiColorComponents;
		}
	}

	// stripe One normal per vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripe)
	{
		int test = 0;
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
		{
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
			puiTriangulatedColors += uiColorComponents;

			for (A3DUns32 uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors, uiColorComponents);

				if (uIPoint % 2)
				{
					COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors - uiColorComponents, uiColorComponents);
					COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors + uiColorComponents, uiColorComponents);
				}
				else
				{
					COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors + uiColorComponents, uiColorComponents);
					COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors - uiColorComponents, uiColorComponents);
				}
				puiTriangulatedColors += uiColorComponents;
			}
			puiTriangulatedColors += uiColorComponents;
		}
		test = test;
	}

	//Triangle one Normal Per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormal)
	{
		unsigned uiNbIndice = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++]) * 3 * uiColorComponents;
		COPY(auTriangleWithPoint_Colors, puiTriangulatedColors, uiNbIndice);
	}

	// Fan One normal per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormal)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
		{
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
			A3DUns8* pFanPointColors = puiTriangulatedColors;
			puiTriangulatedColors += uiColorComponents;

			if (bIsOneNormal == false)
			{
				A3DUns32 uIPoint;
				for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
				{
					COPY_(auTriangleWithPoint_Colors, pFanPointColors, uiColorComponents);
					COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors, uiColorComponents);
					COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors + uiColorComponents, uiColorComponents);
					puiTriangulatedColors += uiColorComponents;
				}
				puiTriangulatedColors += uiColorComponents;
				continue;
			}

			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 1; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Colors, pFanPointColors, uiColorComponents);
				COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors, uiColorComponents);
				COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors + uiColorComponents, uiColorComponents);
				puiTriangulatedColors += uiColorComponents;
			}
			puiTriangulatedColors += uiColorComponents;
		}
	}

	// Stripe One normal per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormal)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
		{
			A3DUns32 uiNbPoint = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
			puiTriangulatedColors += uiColorComponents;

			A3DUns32 uIPoint;
			for (uIPoint = 0; uIPoint < uiNbPoint - 2; uIPoint++)
			{
				COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors, uiColorComponents);

				if (uIPoint % 2)
				{
					COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors - uiColorComponents, uiColorComponents);
					COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors + uiColorComponents, uiColorComponents);
				}
				else
				{
					COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors + uiColorComponents, uiColorComponents);
					COPY_(auTriangleWithPoint_Colors, puiTriangulatedColors - uiColorComponents, uiColorComponents);
				}
				puiTriangulatedColors += uiColorComponents;
			}
			puiTriangulatedColors += uiColorComponents;
		}
	}
	return	 A3D_SUCCESS;
}

A3DStatus A3DFaceTessDataConnector::Indices(
	std::vector<unsigned>& /*auTrianglesOneNormal*/,			//(normal, point, point,..., point)
	std::vector<unsigned>& /*auFanOneNormal*/,
	std::vector<unsigned>& /*auStripeOneNormal*/,
	std::vector<unsigned>& auTrianglesWithNormals,			//normal, point, ... , normal, point
	std::vector<unsigned>& auFanWithNormals,	   
	std::vector<unsigned>& auStripeWithNormal,	
	std::vector<unsigned>& /*auTrianglesOneNormalTexture*/,		//normal,{texture...},point,{texture...},point,{texture...},point
	std::vector<unsigned>& /*auFanOneNormalTextured*/,		   
	std::vector<unsigned>& /*auStripeOneNormalTextured*/,
	std::vector<unsigned>& /*auTrianglesWithNormalsTextured*/,	//normal,{texture...},point,...,normal,{texture...},point
	std::vector<unsigned>& /*auFanWithNormalsTextured*/,
	std::vector<unsigned>& /*auStripeWithNormalsTextured*/) const
{
	A3DTessFaceData* pFaceTessData =	(A3DTessFaceData*)this->GetA3DEntity();
	if (!pFaceTessData->m_uiSizesTriangulatedSize)
		return A3D_SUCCESS;
	
	A3DUns32 uiCurrentSize = 0;
	A3DUns32* puiTriangulatedIndexes = m_pTessConnector->GetTess3DData()->m_puiTriangulatedIndexes 
		+ pFaceTessData->m_uiStartTriangulated;
	A3DUns32 uiNbIndice = 0;

	// One Normal Per Vertex
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangle)
	{
		uiNbIndice = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] * 6;
		COPY(auTrianglesWithNormals, puiTriangulatedIndexes, uiNbIndice);
	}

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFan)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
			uiNbIndice+= pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++]*2;
		COPY(auFanWithNormals, puiTriangulatedIndexes, uiNbIndice);
	}

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripe)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
			uiNbIndice+= pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++]*2;
		COPY(auStripeWithNormal, puiTriangulatedIndexes, uiNbIndice);
	}

	//One Normal Per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormal)
	{
// 		bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
// 		if(bIsOneNormal == false)
// 		{
// 			return A3D_ERROR;
// 		}
		uiNbIndice = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++]/* & kA3DTessFaceDataNormalMask*/) * 4;
	}
	COPY(auTrianglesWithNormals, puiTriangulatedIndexes, uiNbIndice);

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormal)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
		{
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			if(bIsOneNormal == false)
			{
				return A3D_ERROR;
			}
			uiNbIndice+= (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask) + 1;
		}
		COPY(auFanWithNormals, puiTriangulatedIndexes, uiNbIndice);
	}

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormal)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
		{
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			if(bIsOneNormal == false)
			{
				return A3D_ERROR;
			}
			uiNbIndice+= pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] + 1;
		}
		COPY(auStripeWithNormal, puiTriangulatedIndexes, uiNbIndice);
	}

	// Textured
	//One Normal Per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleTextured)
		uiNbIndice = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] * 9;
	COPY(auTrianglesWithNormals, puiTriangulatedIndexes, uiNbIndice);

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanTextured)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
			uiNbIndice+= pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] * 3 ;
		COPY(auFanWithNormals, puiTriangulatedIndexes, uiNbIndice);
	}

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeTextured)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++];
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
			uiNbIndice+= pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] * 3;
		COPY(auStripeWithNormal, puiTriangulatedIndexes, uiNbIndice);
	}


	// Textured
	//One Normal Per entity
	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormalTextured)
	{
		uiNbIndice = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++]/* & kA3DTessFaceDataNormalMask*/) * 7;
	}
	COPY(auTrianglesWithNormals, puiTriangulatedIndexes, uiNbIndice);

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormalTextured)
	{
		A3DUns32 uiNbFan = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		for (A3DUns32 uiFan = 0; uiFan < uiNbFan; uiFan++)
		{
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			if(bIsOneNormal == false)
			{
				return A3D_ERROR;
			}
			uiNbIndice+= (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask) * 2 + 1 ;
		}
		COPY(auFanWithNormals, puiTriangulatedIndexes, uiNbIndice);
	}

	if (pFaceTessData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormalTextured)
	{
		A3DUns32 uiNbStripe = pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask;
		A3DUns32 uiStripe;
		for (uiStripe = 0; uiStripe < uiNbStripe; uiStripe++)
		{
			bool bIsOneNormal = (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize] & kA3DTessFaceDataNormalSingle) != 0;
			if(bIsOneNormal == false)
			{
				return A3D_ERROR;
			}
			uiNbIndice+= (pFaceTessData->m_puiSizesTriangulated[uiCurrentSize++] & kA3DTessFaceDataNormalMask) * 2 + 1;
		}
		COPY(auStripeWithNormal, puiTriangulatedIndexes, uiNbIndice);
	}

	return	 A3D_SUCCESS;
}

unsigned A3DFaceTessDataConnector::GetWireNumber() const
{
	A3DTessFaceData* pFaceTessData = (A3DTessFaceData*)this->GetA3DEntity();
	return pFaceTessData->m_uiSizesWiresSize;
}

A3DStatus A3DFaceTessDataConnector::GetEdgeIndices(unsigned uEdgeIndice, std::vector<unsigned>& auEdgeIndice) const
{
	A3DTessFaceData* pFaceTessData = (A3DTessFaceData*)this->GetA3DEntity();
	unsigned int uI, uNbVertices = pFaceTessData->m_puiSizesWires[uEdgeIndice];


	if((uNbVertices & kA3DTessFaceDataWireIsNotDrawn) != 0)
		uNbVertices = uNbVertices & (~kA3DTessFaceDataWireIsNotDrawn);

	bool bClosed = (uNbVertices & kA3DTessFaceDataWireIsClosing) != 0;
	if(bClosed)
		uNbVertices = uNbVertices & (~kA3DTessFaceDataWireIsClosing);


	//Compute Start Indice:
	unsigned uStartIndex = pFaceTessData->m_uiStartWire, uTemp;
	for(uI = 0; uI < uEdgeIndice; uI++)
	{
		uTemp = pFaceTessData->m_puiSizesWires[uI];
		if(uTemp & kA3DTessFaceDataWireIsNotDrawn)
			uTemp = uTemp & (~kA3DTessFaceDataWireIsNotDrawn);
		if(uTemp & kA3DTessFaceDataWireIsClosing)
			uTemp = uTemp & (~kA3DTessFaceDataWireIsClosing);
		uStartIndex += uTemp;

	}

	A3DUns32* puiStartWireIndexes = m_pTessConnector->GetTess3DData()->m_puiWireIndexes + uStartIndex;
	A3DUns32* puiCurrentWireIndexes = puiStartWireIndexes;
	auEdgeIndice.resize(uNbVertices);// +(bClosed ? 1 : 0));

	for(uI = 0; uI < uNbVertices; uI++)
	{
		auEdgeIndice[uI] = (*puiCurrentWireIndexes);
		puiCurrentWireIndexes++;
	}

	//if(bClosed)
	//	auEdgeIndice[uI] = (*puiStartWireIndexes);
	return A3D_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//A3DWireTessDataConnector
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
A3DWireTessDataConnector::A3DWireTessDataConnector(const A3DTess3DWire* psTessWire)
{
	A3D_INITIALIZE_DATA(A3DTess3DWireData, m_sWire3DTessData);
	A3DTess3DWireGet(psTessWire, &m_sWire3DTessData);
	A3D_INITIALIZE_DATA(A3DTessBaseData, m_sBaseTessData);
	A3DTessBaseGet((const A3DTessBase*)psTessWire, &m_sBaseTessData);
}


A3DWireTessDataConnector::~A3DWireTessDataConnector()
{
	A3DTess3DWireGet(NULL, &m_sWire3DTessData);
	A3DTessBaseGet(NULL, &m_sBaseTessData);
}


A3DStatus A3DWireTessDataConnector::Points(double*& pPoint, unsigned int& uSize) const
{
	pPoint = m_sBaseTessData.m_pdCoords;
	uSize = m_sBaseTessData.m_uiCoordSize;
	return A3D_SUCCESS;
}

A3DStatus A3DWireTessDataConnector::Indices(std::vector<unsigned>& auPolylineIndices) const
{
	if(!m_sWire3DTessData.m_uiSizesWiresSize)
	{
		if(m_sBaseTessData.m_uiCoordSize)
		{
			auPolylineIndices.push_back(0);
			auPolylineIndices.push_back(m_sBaseTessData.m_uiCoordSize / 3);
		}
		return A3D_SUCCESS;
	}

	unsigned int uTemp;
	auPolylineIndices.push_back(0);
	for(unsigned uI = 0; uI < m_sWire3DTessData.m_uiSizesWiresSize; uI++)
	{
		uTemp = m_sWire3DTessData.m_puiSizesWires[uI];
		auPolylineIndices.push_back(uTemp);
		uI += uTemp;
	}
	return A3D_SUCCESS;
}

A3DStatus A3DWireTessDataConnector::Traverse(A3DVisitorContainer* psVisitor)
{

	A3DStatus iRet = A3D_SUCCESS;
	CHECK_RET(psVisitor->visitEnter(*this));

	CHECK_RET(psVisitor->visitLeave(*this));

	return A3D_SUCCESS;

}
