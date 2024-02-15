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

#include "VisitorTessellation.h"
#include "TessConnector.h"
#include "VisitorTessellation.h"
#include "TessConnector.h"


A3DStatus A3DVisitorTessellation::visitEnter(const A3DTessDataConnector& sTessConnector)
{
	m_uCurrentFaceIndice = 0;
	//Get point Array
	A3DStatus iRet = A3D_SUCCESS;
	unsigned int uPointSize, uNormalSize, uUVSize, uFaceSize;
	double* pdPoint, *pdNormal, *pdUV;
	CHECK_RET(sTessConnector.Points(pdPoint, uPointSize));
	CHECK_RET(sTessConnector.Normals(pdNormal, uNormalSize));
	CHECK_RET(sTessConnector.UV(pdUV, uUVSize));
	uFaceSize = sTessConnector.FacesSize();
	unsigned int uIFaceCount;
	for(uIFaceCount = 0; uIFaceCount<uFaceSize; uIFaceCount++)
	{
		std::vector<unsigned> auTriangleWithPointNormalInidces;
		std::vector<unsigned> auTrinagleWithPointNromalUVindices;
		CHECK_RET(sTessConnector.IndicesPerFaceAsTriangle(uIFaceCount,
			auTriangleWithPointNormalInidces,
			auTrinagleWithPointNromalUVindices));

		/****************************************************************************
		Please create Your mesh here
		*****************************************************************************/
	}
	return A3D_SUCCESS;
}

A3DStatus A3DVisitorTessellation::visitLeave(const A3DTessDataConnector& /*sTessConnector*/)
{
	return A3D_SUCCESS;
}

A3DStatus A3DVisitorTessellation::visitEnter(const A3DFaceTessDataConnector& /*sTessConnector*/)
{
	return A3D_SUCCESS;
}

A3DStatus A3DVisitorTessellation::visitLeave(const A3DFaceTessDataConnector& /*sTessConnector*/)
{
	m_uCurrentFaceIndice++;
	return A3D_SUCCESS;
}

A3DStatus A3DVisitorTessellation::visitEnter(const A3DRiConnector& /*sTessConnector*/)
{
	return A3D_SUCCESS;
}

A3DStatus A3DVisitorTessellation::visitLeave(const A3DRiConnector& /*sTessConnector*/)
{
	return A3D_SUCCESS;
}

A3DStatus A3DVisitorTessellation::visitEnter(const A3DMarkupTessConnector& /*sMarkupTessConnector*/)
{
	return A3D_SUCCESS;
}

A3DStatus A3DVisitorTessellation::visitLeave(const A3DMarkupTessConnector& /*sMarkupTessConnector*/)
{
	return A3D_SUCCESS;
}

A3DStatus A3DVisitorTessellation::visitEnter(const A3DWireTessDataConnector& /*sWireTessConnector*/)
{
	return A3D_SUCCESS;
}


A3DStatus A3DVisitorTessellation::visitLeave(const A3DWireTessDataConnector& /*sWireTessConnector*/)
{
	return A3D_SUCCESS;
}
