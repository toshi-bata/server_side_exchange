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
\file MarkupConnector.cpp	

This file provides tools to get the visual representation of markup

***********************************************************************************************************************/
#include "MarkupTessConnector.h"
#include "Visitors.h"
#include <algorithm>
#include "Matrix.h"
#include <assert.h>

 #define DEFAULT_OFFSET {\
	pData += *(puiStart+1); \
	puiStart += (uiCount + 1); \
	}
#define MAKE_OFFSET(CountInt, CountFloats) {\
	pData += CountFloats; \
	puiStart += (CountInt + 1); \
	}

A3DMarkupTessDataConnectorCreator::A3DMarkupTessDataConnectorCreator(std::vector<A3DMarkupTessConnector*>& a_rvMkpTessConnector)
	: m_bStartToDrawSet(false), m_bFaceViewActivated(false), m_bFramedrawActivated(false), m_bFixedSizeActivated(false),
	m_rvMarkupTessData(a_rvMkpTessConnector)
{
	m_rvMarkupTessData.push_back(new A3DMarkupTessConnector());
	
	//Set Indentity On Matrix
	memset(&m_adCurrentWorldMatrix[0], 0, sizeof(double) * 16);
	m_adCurrentWorldMatrix[0] = m_adCurrentWorldMatrix[5] = m_adCurrentWorldMatrix[10] = m_adCurrentWorldMatrix[15] = 1;
	m_adPushedWorldMatrices.resize(16);
	memcpy(&m_adPushedWorldMatrices[0], m_adCurrentWorldMatrix, sizeof(double) * 16);

}

A3DMarkupTessDataConnectorCreator::~A3DMarkupTessDataConnectorCreator()
{
}
	
A3DStatus A3DMarkupTessDataConnectorCreator::pushMatrix(const double* a_pdLocalMatrix)
{	
	size_t uSize = m_adPushedWorldMatrices.size();
	if(uSize < 16)
		return A3D_ERROR;
	m_adPushedWorldMatrices.resize(uSize + 16);
	
	MultiplyMatrix(&m_adCurrentWorldMatrix[0], a_pdLocalMatrix, &m_adPushedWorldMatrices[uSize]);
	memcpy( &m_adCurrentWorldMatrix[0],&m_adPushedWorldMatrices[uSize], sizeof(double) * 16); 
	return A3D_SUCCESS;
}

A3DStatus A3DMarkupTessDataConnectorCreator::popMatrix()
{
   size_t uSize = m_adPushedWorldMatrices.size() -16;
   m_adPushedWorldMatrices.resize(uSize);
   memcpy( &m_adCurrentWorldMatrix[0],&m_adPushedWorldMatrices[uSize - 16], sizeof(double) * 16); 
   return A3D_SUCCESS;
}

A3DMarkupTessConnector& A3DMarkupTessDataConnectorCreator::GetMarkupTessConnector(const double* pdMatrix)
{
	bool bSameMatrix = true;
	size_t uI, uSize = m_rvMarkupTessData.size(); 
	assert(uSize > 0);
	for(uI = 0; uI < 16; uI++)
	{
		if(m_adCurrentWorldMatrix[uI] != pdMatrix[uI])
		{
			bSameMatrix = false;
			break;
		}
	}
	if(!bSameMatrix/* && m_bStartToDrawSet*/)
	{
		m_rvMarkupTessData.push_back(new A3DMarkupTessConnector(*m_rvMarkupTessData[uSize - 1]));
		m_bStartToDrawSet = false;
		uSize++;
	}
	return *m_rvMarkupTessData[uSize - 1];
}

	 
A3DMarkupTessConnector& A3DMarkupTessDataConnectorCreator::GetMarkupTessConnector(unsigned uiExtraDataValue,bool& bCameraDependant )
{
	size_t uSize =  m_rvMarkupTessData.size();
	assert(uSize > 0);
	bCameraDependant = false;
	m_bStartToDrawSet = m_rvMarkupTessData.back()->HasAlreadySomethingToDisplay();
	switch (uiExtraDataValue)
	{
		case A3D_DECODE_EXTRA_DATA(kA3DMarkupFaceViewMask): // 6 :  Camera Dependant -  Faceview
			m_bFaceViewActivated = !m_bFaceViewActivated;
		case A3D_DECODE_EXTRA_DATA(kA3DMarkupFrameDrawMask): // 7 :  CameraDependant  -  Frame draw
			m_bFramedrawActivated = !m_bFramedrawActivated;		
		case A3D_DECODE_EXTRA_DATA(kA3DMarkupFixedSizeMask): // 8 :  CameraDependant  -  Fixedsize
			m_bFixedSizeActivated = !m_bFixedSizeActivated;
		case A3D_DECODE_EXTRA_DATA(kA3DMarkupSymbolMask): // 9 :  Camera Dependant -  Symbol
		case A3D_DECODE_EXTRA_DATA(kA3DMarkupColorMask): // 11 : color
		case A3D_DECODE_EXTRA_DATA(kA3DMarkupLineStippleMask): // 12 : line stipple
		case A3D_DECODE_EXTRA_DATA(kA3DMarkupLineWidthMask): // 17 : line width
		case 0 :  // polyline
		case A3D_DECODE_EXTRA_DATA(kA3DMarkupTrianglesMask): // 2 :  triangles
		case A3D_DECODE_EXTRA_DATA(kA3DMarkupCylinderMask): // 10 : cylinder
		case A3D_DECODE_EXTRA_DATA(kA3DMarkupFontMask): // 13 : font
		case A3D_DECODE_EXTRA_DATA(kA3DMarkupTextMask): // 14 : text
		case A3D_DECODE_EXTRA_DATA(kA3DMarkupPointsMask): // 15 : points
		case A3D_DECODE_EXTRA_DATA(kA3DMarkupPolygonMask): // 16 : polygon
		{
			if(m_bStartToDrawSet)
			{
				A3DMarkupTessConnector *pNewConnector = new A3DMarkupTessConnector(*m_rvMarkupTessData[uSize - 1]);
				m_rvMarkupTessData.push_back(pNewConnector);
				uSize++;
				m_bStartToDrawSet = false;
				memcpy(&(pNewConnector->m_adWorldMatrix[0]),
				   &m_adCurrentWorldMatrix[0],
				   sizeof(double)*16);	
			}
		}
		break;
	}
	if(m_bFaceViewActivated || m_bFramedrawActivated  || m_bFixedSizeActivated)
		bCameraDependant = true;
	

	return *m_rvMarkupTessData[uSize - 1];
}

/********************************************************************************************************************

			 A3DMarkupTessConnector

*********************************************************************************************************************/
A3DMarkupTessConnector::A3DMarkupTessConnector() : m_bFaceView(false) ,m_bFrameDraw(false),  m_bSymbol(false), m_bFixedSize(false),
m_bIsText(false), m_uiFixedSizePatternIndex(0), m_iGraphStyleIndexLinsStyle(-1), m_dLineWidth(1.0), m_iGraphVPicturePatternIndex(-1)
{
	memset(m_adViewDependantParameter, 0, sizeof(m_adViewDependantParameter));	
	memset(m_adWorldMatrix, 0, sizeof(m_adWorldMatrix));
	m_adWorldMatrix[0] = m_adWorldMatrix[5] = m_adWorldMatrix[10] = m_adWorldMatrix[15] = 1.;
	memcpy(&m_adModelMatrix[0], &m_adWorldMatrix[0], sizeof(double) * 16);
	memcpy(&m_adProjMatrix[0], &m_adWorldMatrix[0], sizeof(double) * 16);
	m_adRGB[0] = m_adRGB[1] = m_adRGB[2] = -1.0;
} 

A3DMarkupTessConnector::A3DMarkupTessConnector(A3DMarkupTessConnector& a_PreviousTessConnector)
{
	m_bFaceView = a_PreviousTessConnector.m_bFaceView;
	m_bFrameDraw = a_PreviousTessConnector.m_bFrameDraw;
	m_bSymbol = a_PreviousTessConnector.m_bSymbol;
	m_bFixedSize = a_PreviousTessConnector.m_bFixedSize;

	m_bIsText = a_PreviousTessConnector.m_bIsText;
	memcpy(m_adViewDependantParameter, a_PreviousTessConnector.m_adViewDependantParameter, sizeof(m_adViewDependantParameter));
	m_uiFixedSizePatternIndex = a_PreviousTessConnector.m_uiFixedSizePatternIndex;

	m_ucBehaviour = a_PreviousTessConnector.m_ucBehaviour;

	//Graphic
	memcpy(m_adRGB, a_PreviousTessConnector.m_adRGB, sizeof(m_adRGB));
	m_dLineWidth = a_PreviousTessConnector.m_dLineWidth;
	m_iGraphStyleIndexLinsStyle = a_PreviousTessConnector.m_iGraphStyleIndexLinsStyle;
	m_iGraphVPicturePatternIndex = a_PreviousTessConnector.m_iGraphVPicturePatternIndex;

	//Geometrical data
	//std::vector<double> m_adPoints;
	//std::vector<int> m_aiPolylineIndices;
	//std::vector<double> m_adPolylineCoords;
	//std::vector<double> m_adTriangleList;
	//std::vector<double> m_adCylinder;	// n x (m_dRadiusBase, m_dRadiusTop, m_dHeight)

	//Text
	//std::vector<A3DPMITextEntry*> m_apTexts;
}

A3DMarkupTessConnector::~A3DMarkupTessConnector()
{
	std::vector<A3DPMITextEntry*>::iterator itCur = m_apTexts.begin();
	std::vector<A3DPMITextEntry*>::iterator itEnd = m_apTexts.end();
	for(; itCur < itEnd; ++itCur)
	{
		delete *itCur;
	}
}

#define SIZE_ARROW 15.0
A3DStatus A3DMarkupTessConnector::GetMarkupTessellation(const A3DTessBaseData& sTessBaseData,
										 const A3DTessMarkupData* pTessMarkupData,
										 const bool bIsText,
										 std::vector<A3DMarkupTessConnector*>& asMarkupTessDataConnector,
										 double a_dParallelToScreenPMIScale)
{
	A3DMarkupTessDataConnectorCreator sMarkupTessDataConnectorCreator(asMarkupTessDataConnector);
  	A3DStatus iRet = A3D_SUCCESS;
	float afDefaultcolor[3];
	afDefaultcolor[0] = afDefaultcolor[1] = afDefaultcolor[2] = 0.;

	bool bIsScreenMarkupZoomable = true;
	unsigned int kSize;
	unsigned int uiCount = 0;
	  	
	A3DGraphRgbColorData sRgbColorData;
	A3D_INITIALIZE_DATA( A3DGraphRgbColorData, sRgbColorData);
	A3DGraphStyleData sGraphStyleData;
	A3D_INITIALIZE_DATA( A3DGraphStyleData, sGraphStyleData);

	A3DGraphPictureData sPictureData;
	A3D_INITIALIZE_DATA( A3DGraphPictureData, sPictureData);

	A3DFontKeyData sFontKeyData;
	A3D_INITIALIZE_DATA(A3DFontKeyData, sFontKeyData);

	bIsScreenMarkupZoomable = (pTessMarkupData->m_cBehaviour & kA3DMarkupIsZoomable) != 0;
	const A3DDouble* pData = sTessBaseData.m_pdCoords;
	const A3DUns32* puiStart = &pTessMarkupData->m_puiCodes[0];
	const A3DUns32* puiEnd = &pTessMarkupData->m_puiCodes[pTessMarkupData->m_uiCodesSize-1];
	
	bool bShow = false;
	if((pData != NULL) && (puiStart != NULL) && (puiEnd != NULL)) for (; puiStart < puiEnd; puiStart++)
	{
		uiCount = *puiStart & kA3DMarkupIntegerMask;
		bShow = true;
		
		if (*puiStart & kA3DMarkupIsExtraData)
		{
			A3DUns32 uiExtraDataValue = A3D_DECODE_EXTRA_DATA(*puiStart);
		    bool bCameraDependant;
			A3DMarkupTessConnector& sCurMarkupTessConnector =
				sMarkupTessDataConnectorCreator.GetMarkupTessConnector(uiExtraDataValue, bCameraDependant);
			sCurMarkupTessConnector.m_ucBehaviour = pTessMarkupData->m_cBehaviour;
			switch (uiExtraDataValue)
			{
				case A3D_DECODE_EXTRA_DATA(kA3DMarkupTrianglesMask): // 2 :  // triangles
				{
					kSize = *(puiStart+1);// number of triangles * 9
					unsigned int uI;
					for(uI = 0; uI < kSize; uI++)
						sCurMarkupTessConnector.m_adTriangleList.push_back(*(pData+uI));
					DEFAULT_OFFSET;
					break;
				}
				case A3D_DECODE_EXTRA_DATA(kA3DMarkupFaceViewMask): // 6 :  // Camera Dependant -  Faceview
							 // Using by circle center/fastener/measurement point/spot welding
							 // geometrical symbol zoomable
				{
					if(*(puiStart+1) > 0) 
					{
						sCurMarkupTessConnector.m_bFaceView = true;
						sCurMarkupTessConnector.m_adViewDependantParameter[0] = * pData;
						sCurMarkupTessConnector.m_adViewDependantParameter[1] = *(pData+1);
						sCurMarkupTessConnector.m_adViewDependantParameter[2] = *(pData+2);
						MAKE_OFFSET(0, 3)
					}
					else
					{
						if(!(sMarkupTessDataConnectorCreator.HasStartToDrawSet()))
							sCurMarkupTessConnector.m_bFaceView = false;
						MAKE_OFFSET(0, 0);
					}
					break;
				}
				case A3D_DECODE_EXTRA_DATA(kA3DMarkupFrameDrawMask): // 7 :  // CameraDependant  -  Frame draw
					// Usually used for non-zoomable text, but it is possible to change to zoomable (option on CatiaV5)
				{
					if(*(puiStart+1) > 0)
					{
						sCurMarkupTessConnector.m_bFrameDraw = true;
						sCurMarkupTessConnector.m_adViewDependantParameter[0] = * pData;
						sCurMarkupTessConnector.m_adViewDependantParameter[1] = *(pData+1);
						sCurMarkupTessConnector.m_adViewDependantParameter[2] = /*bIsText ? 0 : */*(pData+2);	
						sCurMarkupTessConnector.m_bIsText =	 bIsText;
						MAKE_OFFSET(0, 3)
					}
					else
					{
						if(!(sMarkupTessDataConnectorCreator.HasStartToDrawSet()))
							sCurMarkupTessConnector.m_bFrameDraw = false;
						MAKE_OFFSET(0, 0)
					}
					break;
				}
				case A3D_DECODE_EXTRA_DATA(kA3DMarkupFixedSizeMask): // 8 :  // CameraDependant  -  Fixedsize
				{
					if(*(puiStart+1) > 0)
					{
						sCurMarkupTessConnector.m_bFixedSize = true;
						sCurMarkupTessConnector.m_adViewDependantParameter[0] = * pData;
						sCurMarkupTessConnector.m_adViewDependantParameter[1] = *(pData+1);
						sCurMarkupTessConnector.m_adViewDependantParameter[2] = /*bIsText ? 0 : */*(pData+2);
						sCurMarkupTessConnector.m_uiFixedSizePatternIndex = *(puiStart+2);
						MAKE_OFFSET(0, 3);
					}
					else
					{
						if(!(sMarkupTessDataConnectorCreator.HasStartToDrawSet()))
							sCurMarkupTessConnector.m_bFixedSize = false;
						MAKE_OFFSET(0, 0)
					}
					break;
				}
				case A3D_DECODE_EXTRA_DATA(kA3DMarkupSymbolMask): // 9 :  // Camera Dependant -  Symbol
				{
					sCurMarkupTessConnector.m_bSymbol = true;
					sCurMarkupTessConnector.m_adViewDependantParameter[0] = * pData;
					sCurMarkupTessConnector.m_adViewDependantParameter[1] = *(pData+1);
					sCurMarkupTessConnector.m_adViewDependantParameter[2] = /*bIsText ? 0 : */*(pData+2);
				   	//unsigned int uiPatternIndex = *(puiStart+2);
				   	DEFAULT_OFFSET
					break;	
				}
				case A3D_DECODE_EXTRA_DATA(kA3DMarkupCylinderMask): // 10 : cylinder
				{
					double dRadiusBase = *pData;
					double dRadiusTop = *(pData+1);
					double dHeight  = *(pData+2);
					sCurMarkupTessConnector.m_adCylinder.push_back(dRadiusBase);
					sCurMarkupTessConnector.m_adCylinder.push_back(dRadiusTop);
					sCurMarkupTessConnector.m_adCylinder.push_back(dHeight);
					DEFAULT_OFFSET
					break;
				}		
				case A3D_DECODE_EXTRA_DATA(kA3DMarkupPolygonMask): // 16 : polylines
				{
					if(sCurMarkupTessConnector.m_adPolylineCoords.empty() == true)
					{
						sCurMarkupTessConnector.m_aiPolylineIndices.push_back(0);
					}
					kSize = *(puiStart+1); // number of points
					int iNbIndicesBefore = 0;
					if(sCurMarkupTessConnector.m_aiPolylineIndices.size() > 0)
					{
						iNbIndicesBefore = sCurMarkupTessConnector.m_aiPolylineIndices.back();
					}
					unsigned int uI;
 					for(uI = 0; uI < kSize; uI++)
 						sCurMarkupTessConnector.m_adPolylineCoords.push_back(*(pData+uI));
					sCurMarkupTessConnector.m_aiPolylineIndices.push_back(iNbIndicesBefore + (kSize / 3));
					DEFAULT_OFFSET;
					break;
				}
				case A3D_DECODE_EXTRA_DATA(kA3DMarkupColorMask): // 11 : color
				{
					A3DGlobalGetGraphRgbColorData(*(puiStart+2), &sRgbColorData);
					sCurMarkupTessConnector.m_adRGB[0] = sRgbColorData.m_dRed;
					sCurMarkupTessConnector.m_adRGB[1] = sRgbColorData.m_dGreen;
					sCurMarkupTessConnector.m_adRGB[2] = sRgbColorData.m_dBlue;
					DEFAULT_OFFSET;
					break;
				}
				case A3D_DECODE_EXTRA_DATA(kA3DMarkupLineStippleMask): // 12 : line stipple
				{
					if (uiCount > 0)
					{
						sCurMarkupTessConnector.m_iGraphStyleIndexLinsStyle = *(puiStart+2);
					}
					else
						sCurMarkupTessConnector.m_iGraphStyleIndexLinsStyle = -1;
						//glDisable(GL_LINE_STIPPLE);
					DEFAULT_OFFSET;
					break;
				}
				
				case A3D_DECODE_EXTRA_DATA(kA3DMarkupLineWidthMask): // 17 : line width
					  // Linewidth are only used on markup arrow of drawings
				{
					if (*(puiStart+1) > 0)
					{
						sCurMarkupTessConnector.m_dLineWidth = *pData;
					}
					else
					{
						sCurMarkupTessConnector.m_dLineWidth = 1;
					}
					DEFAULT_OFFSET;
					break;
				}
				case A3D_DECODE_EXTRA_DATA(kA3DMarkupPointsMask): // 15 : points
				{
					kSize = *(puiStart+1); // number of points 
					unsigned int uI;
					for(uI = 0; uI < kSize; uI++)
					{
						sCurMarkupTessConnector.m_adPoints.push_back(*(pData+uI));
					}
					DEFAULT_OFFSET;
					break;
				}
				case A3D_DECODE_EXTRA_DATA(kA3DMarkupFontMask): // 13 : font
				{
					sFontKeyData.m_iFontFamilyIndex = *(puiStart+2);
					sFontKeyData.m_iFontStyleIndex = (*(puiStart+3) & kA3DFontKeyStyle) >> 24;
					sFontKeyData.m_iFontSizeIndex = (*(puiStart+3) & kA3DFontKeySize) >> 12;
					sFontKeyData.m_cAttributes = (A3DInt8)(*(puiStart+3) & kA3DFontKeyAttrib);
					DEFAULT_OFFSET;
					break;
				}
				case A3D_DECODE_EXTRA_DATA(kA3DMarkupTextMask): // 14:  text
				{
					A3DDouble dTextWidth = *pData;
					A3DDouble dTextHeight = *(pData+1);
					A3DUns32 uiTextIndex = *(puiStart+2);
					if (uiTextIndex < pTessMarkupData->m_uiTextsSize)
					{
						sCurMarkupTessConnector.m_apTexts.push_back(new A3DPMITextEntry(dTextWidth, dTextHeight, 
															sFontKeyData, pTessMarkupData->m_ppcTexts[uiTextIndex]));
					}
					DEFAULT_OFFSET;
					break;
			  	}
				
				default :
					DEFAULT_OFFSET;
			}
		}
		else if (*puiStart & kA3DMarkupIsMatrix)
		{
			if (*(puiStart+1) > 0) // if there are floats it is the beginning of a "matrix" mode
			{
			   	A3DMarkupTessConnector& sCurMarkupTessConnector = sMarkupTessDataConnectorCreator.GetMarkupTessConnector((const double*) pData);
				sCurMarkupTessConnector.m_ucBehaviour = pTessMarkupData->m_cBehaviour;
				CHECK_RET(sMarkupTessDataConnectorCreator.pushMatrix((const double*) pData));
			
				memcpy(&(sCurMarkupTessConnector.m_adWorldMatrix[0]),
					   sMarkupTessDataConnectorCreator.GetCurrentWorldMatrix(),
					   sizeof(double) *16);
				//dScaleText=stGetScaleText(pData);
				if(sCurMarkupTessConnector.IsFrameDraw())
				{
					sCurMarkupTessConnector.m_adWorldMatrix[0] *= a_dParallelToScreenPMIScale;
					sCurMarkupTessConnector.m_adWorldMatrix[1] *= a_dParallelToScreenPMIScale;
					sCurMarkupTessConnector.m_adWorldMatrix[2] *= a_dParallelToScreenPMIScale;
					sCurMarkupTessConnector.m_adWorldMatrix[4] *= a_dParallelToScreenPMIScale;
					sCurMarkupTessConnector.m_adWorldMatrix[5] *= a_dParallelToScreenPMIScale;
					sCurMarkupTessConnector.m_adWorldMatrix[6] *= a_dParallelToScreenPMIScale;
					sCurMarkupTessConnector.m_adWorldMatrix[7] *= a_dParallelToScreenPMIScale;
					sCurMarkupTessConnector.m_adWorldMatrix[8] *= a_dParallelToScreenPMIScale;
					sCurMarkupTessConnector.m_adWorldMatrix[9] *= a_dParallelToScreenPMIScale;
					sCurMarkupTessConnector.m_adWorldMatrix[11] *= a_dParallelToScreenPMIScale;
					sCurMarkupTessConnector.m_adWorldMatrix[12] *= a_dParallelToScreenPMIScale;
					sCurMarkupTessConnector.m_adWorldMatrix[13] *= a_dParallelToScreenPMIScale;
				}
				MAKE_OFFSET(0, 16);
			}
			else // if there is no float, the matrix mode ends
			{
				sMarkupTessDataConnectorCreator.popMatrix();
				MAKE_OFFSET(0, 0);
			}	 
		}
		else	// It's a polyline
		{
			bool bCameraDependant;
			A3DMarkupTessConnector& sCurMarkupTessConnector =
										sMarkupTessDataConnectorCreator.GetMarkupTessConnector(0, bCameraDependant);
			if(sCurMarkupTessConnector.m_adPolylineCoords.empty() == true)
			{
				sCurMarkupTessConnector.m_aiPolylineIndices.push_back(0);
			}
			kSize = *(puiStart+1); // Number of points
			unsigned int i = 0;
			for(i = 0; i < kSize; i++)
			{
				sCurMarkupTessConnector.m_adPolylineCoords.push_back(*(pData + i));
			}

			if(sCurMarkupTessConnector.IsFrameDraw())
			{
				std::vector<double>::iterator itRescaleCur = sCurMarkupTessConnector.m_adPolylineCoords.begin();
				std::vector<double>::iterator itRescaleEnd = sCurMarkupTessConnector.m_adPolylineCoords.end();
				for(; itRescaleCur != itRescaleEnd; ++itRescaleCur)
				{
					*itRescaleCur *= a_dParallelToScreenPMIScale;
				}
			}

			sCurMarkupTessConnector.m_aiPolylineIndices.push_back(int(sCurMarkupTessConnector.m_adPolylineCoords.size())/3);
			DEFAULT_OFFSET;
		}
	}
	return iRet;
}

/// Is the order important? Like if FaceView set after Framedraw
A3DStatus A3DMarkupTessConnector::ComputeCameraDependantMatrix(
										double const * pdProjMatrix, 
										int const * aiViewport, 
										long const * Rect, // TOP_LEFT(X,Y) BOTTOM_RIGHT(X,Y) 
										double* /*pdPerViewMatrix*/)
{

	A3DStatus iRet = A3D_SUCCESS;

	double dScaling = 1, dFixedSize = 1.0 * 15 / dScaling;
	if(m_bFaceView)	// faceview
					// using for circlecenter/fastener/measurementpoint/spotwelding
					// geometrical symbol zoomable
	{
		double sO[3];  sO[0]=0; sO[1] = 0; sO[2] = 0;
		double sX[3];  sX[0] = 1; sX[1] = 0;  sX[2] = 0; 
		double sY[3];  sY[0] = 0; sY[1] = 1;  sY[2] = 0;

		double adP0[3], adP1[3], adP2[3];
		UnProject(&sO[0], m_adModelMatrix, pdProjMatrix, aiViewport, &adP0[0]);
		UnProject(&sX[0], m_adModelMatrix, pdProjMatrix, aiViewport, &adP1[0]);
		UnProject(&sY[0], m_adModelMatrix, pdProjMatrix, aiViewport, &adP2[0]);

		double adVectX[3];
		adVectX[0] = adP1[0] - adP0[0]; 
		adVectX[1] = adP1[1] - adP0[1]; 
		adVectX[2] = adP1[2] - adP0[2]; 

		double adVectY[3];
		adVectY[0] = adP2[0] - adP0[0]; 
		adVectY[1] = adP2[1] - adP0[1]; 
		adVectY[2] = adP2[2] - adP0[2]; 
		double adPlaneNormal[3];
		//Cross Product
		adPlaneNormal[0] =	adVectX[1] * adVectY[2] - adVectX[2] * adVectY[1];
		adPlaneNormal[0] =	adVectX[2] * adVectY[0] - adVectX[0] * adVectY[2];
		adPlaneNormal[0] =	adVectX[0] * adVectY[1] - adVectX[1] * adVectY[0];
		double adMat[16];
		CHECK_RET(Calculate(&adVectX[0], &adVectY[0], &adPlaneNormal[0],&adMat[0]));
		
		adMat[12] =  m_adViewDependantParameter[0];
		adMat[13] =  m_adViewDependantParameter[1];
		adMat[14] =  m_adViewDependantParameter[2];
		adMat[15] = 1.;
		double adOldModelMatrix[16];
		memcpy(&adOldModelMatrix[0], &m_adModelMatrix[0], sizeof(double) * 16);
		CHECK_RET(MultiplyMatrix(&adOldModelMatrix[0], &adMat[0],&m_adModelMatrix[0]));

	}
	
	if(m_bFrameDraw) // Usually used for non-zoomable text, but it is possible to change to zoomable (option on CatiaV5)
	{
		double dXProj, dYProj ,dZProj;
		if (m_bIsText)
		{
			dXProj=m_adViewDependantParameter[0];
			dYProj=m_adViewDependantParameter[1];
			dZProj=0.;
		}
		else
		{
	
			// Ex : turbine de N2688
			//	if (GetMatrixOwnerGroup(sMatGroup))
			//		sMatGroup.TransformScaled(adViewDependantParameter[0], adViewDependantParameter[1], adViewDependantParameter[2]);
			Project(m_adViewDependantParameter[0], m_adViewDependantParameter[1], m_adViewDependantParameter[2],
					m_adModelMatrix, pdProjMatrix, aiViewport, &dXProj, &dYProj, &dZProj);
		}

		double adIentity[16];
		Identity(&adIentity[0]);
		double adOrthoMatrix[16];
		Ortho(&adIentity[0],
			  0.0, (double) (Rect[3] - Rect[1]),	// Right, Left
			  0.0, (double) (Rect[0] - Rect[2]),	//Top, Bottom
			  -1.0, 1.0,							//Near, Fast
			  &adOrthoMatrix[0]);
			
		double adTranslat[16];
		Identity(&adTranslat[0]);
		adTranslat[3]  = dXProj;
		adTranslat[7]  = dYProj;
		adTranslat[11] = dZProj;

		CHECK_RET(MultiplyMatrix(&adOrthoMatrix[0],&adTranslat[0],&m_adProjMatrix[0]));

		if(IsZoomable())
		{
			double adScale[16];
			Identity(&adScale[0]);
			adScale[0] = adScale[5] = adScale[10] = adScale[15]	= dFixedSize;
			double adOldModelMatrix[16];
			memcpy(&adOldModelMatrix[0], &m_adModelMatrix[0], sizeof(double) * 16);
			CHECK_RET(MultiplyMatrix(&adOldModelMatrix[0], &adScale[0], &m_adModelMatrix[0]));
			
		}
	}
		
	if(m_bFixedSize)
	{
		double adTranslat[16];
		Identity(&adTranslat[0]);
		adTranslat[3]  = m_adViewDependantParameter[0];
		adTranslat[7]  = m_adViewDependantParameter[1];
		adTranslat[11] = m_adViewDependantParameter[2];

		double adScale[16];
		Identity(&adScale[0]);
		adScale[0] = adScale[5] = adScale[10] = adScale[15]	 = dFixedSize;
	   	
		double adOldModelMatrix[16];
		memcpy(&adOldModelMatrix[0], &m_adModelMatrix[0], sizeof(double) * 16);
		CHECK_RET(MultiplyMatrix(&adOldModelMatrix[0], &adTranslat[0], &m_adModelMatrix[0]));

		memcpy(&adOldModelMatrix[0], &m_adModelMatrix[0], sizeof(double) * 16);
		CHECK_RET(MultiplyMatrix(&adOldModelMatrix[0], &adScale[0], &m_adModelMatrix[0]));
	}

	if(m_bSymbol)
	{
		double dXProj, dYProj ,dZProj;
		if (m_bIsText)
		{
			dXProj=m_adViewDependantParameter[0];
			dYProj=m_adViewDependantParameter[1];
			dZProj=0.;
		}
		else
		{
			// Ex : turbine de N2688
			//	if (GetMatrixOwnerGroup(sMatGroup))
			//		sMatGroup.TransformScaled(sPntFrame.x,sPntFrame.y,sPntFrame.z);
		  
			CHECK_RET(Project(m_adViewDependantParameter[0], m_adViewDependantParameter[1], m_adViewDependantParameter[2],
					m_adModelMatrix, pdProjMatrix, aiViewport, &dXProj, &dYProj, &dZProj));
		}

		double adIentity[16];
		Identity(&adIentity[0]);
		double adOrthoMatrix[16];
		CHECK_RET(Ortho(&adIentity[0],
			  0.0, (double) (Rect[3] - Rect[1]),	// Right, Left
			  0.0, (double) (Rect[0] - Rect[2]),	//Top, Bottom
			  -1.0, 1.0,							//Near, Fast
			  &adOrthoMatrix[0]));
			
		double adTranslat[16];
		Identity(&adTranslat[0]);
		adTranslat[3]  = dXProj;
		adTranslat[7]  = dYProj;
		adTranslat[11] = dZProj;

		CHECK_RET(MultiplyMatrix(&adOrthoMatrix[0],&adTranslat[0],&m_adProjMatrix[0]));

		//Pattern not used
	}
	return A3D_SUCCESS;
}



