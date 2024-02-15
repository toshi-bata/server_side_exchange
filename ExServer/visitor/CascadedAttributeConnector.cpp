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

#include "CascadedAttributeConnector.h"
#include "Visitors.h"

ColorMaterialsConnector::ColorMaterialsConnector(A3DMiscCascadedAttributes* pAttr) : A3DConnector(pAttr) 
{		  
	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, m_sCascadedAttributeData);
	A3DMiscCascadedAttributesGet(pAttr, &(m_sCascadedAttributeData));
} 

ColorMaterialsConnector::~ColorMaterialsConnector()
{
	A3DMiscCascadedAttributesGet(NULL, &(m_sCascadedAttributeData));
}


bool ColorMaterialsConnector::IsShow()
{
	return m_sCascadedAttributeData.m_bShow ? true : false;
}

bool ColorMaterialsConnector::IsRemoved()
{
	return  m_sCascadedAttributeData.m_bRemoved ? true : false; 
}

bool ColorMaterialsConnector::IsMaterial()
{
	//Todo Test it's not a texture
	return m_sCascadedAttributeData.m_sStyle.m_bMaterial ? true : false;
}

bool ColorMaterialsConnector::IsRGBColor()
{
	return !(IsMaterial());
}

bool ColorMaterialsConnector::IsTranparent()
{
	return m_sCascadedAttributeData.m_sStyle.m_bIsTransparencyDefined ? true : false;
}

A3DStatus ColorMaterialsConnector::GetRGB(double adRGB[3])//, char& cInheritance = 0)
{
	A3DStatus iRet = A3D_SUCCESS;
	A3DGraphRgbColorData sA3DGraphRgbColorData;
	A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sA3DGraphRgbColorData);
	if(A3DGlobalGetGraphRgbColorData(m_sCascadedAttributeData.m_sStyle.m_uiRgbColorIndex, &sA3DGraphRgbColorData) == A3D_DEFAULT_COLOR)
	{
		adRGB[0] =  192.0 / 255.0;
		adRGB[1] =  192.0 / 255.0;
		adRGB[2] =  192.0 / 255.0;
	}
	else
	{
		CHECK_RET(iRet);
		adRGB[0] =  sA3DGraphRgbColorData.m_dRed;
		adRGB[1] =  sA3DGraphRgbColorData.m_dGreen;
		adRGB[2] =  sA3DGraphRgbColorData.m_dBlue;
	}

	return A3D_SUCCESS;
}

A3DStatus ColorMaterialsConnector::Transparency(unsigned char& cTransparenty)
{
	cTransparenty = m_sCascadedAttributeData.m_sStyle.m_ucTransparency;
	return A3D_SUCCESS;
}

A3DStatus ColorMaterialsConnector::Compare(
	ColorMaterialsConnector const & sOtherCascadedAttribute,
	bool& bSameDispalyParameter, 
	bool&  bSamebSameDispalyParameterOnFace)
{
	if( 
		sOtherCascadedAttribute.m_sCascadedAttributeData.m_bShow    != m_sCascadedAttributeData.m_bShow
		||  sOtherCascadedAttribute.m_sCascadedAttributeData.m_bRemoved != m_sCascadedAttributeData.m_bRemoved
		||  sOtherCascadedAttribute.m_sCascadedAttributeData.m_usLayer  != m_sCascadedAttributeData.m_usLayer
		||  sOtherCascadedAttribute.m_sCascadedAttributeData.m_sStyle.m_dWidth		!= m_sCascadedAttributeData.m_sStyle.m_dWidth
		||  sOtherCascadedAttribute.m_sCascadedAttributeData.m_sStyle.m_bVPicture   != m_sCascadedAttributeData.m_sStyle.m_bVPicture	
		||  sOtherCascadedAttribute.m_sCascadedAttributeData.m_sStyle.m_uiLinePatternIndex   != m_sCascadedAttributeData.m_sStyle.m_uiLinePatternIndex
		||  sOtherCascadedAttribute.m_sCascadedAttributeData.m_sStyle.m_bMaterial			 != m_sCascadedAttributeData.m_sStyle.m_bMaterial
		||  sOtherCascadedAttribute.m_sCascadedAttributeData.m_sStyle.m_uiRgbColorIndex	!= m_sCascadedAttributeData.m_sStyle.m_uiRgbColorIndex
		||  sOtherCascadedAttribute.m_sCascadedAttributeData.m_sStyle.m_bIsTransparencyDefined   != m_sCascadedAttributeData.m_sStyle.m_bIsTransparencyDefined
		||  sOtherCascadedAttribute.m_sCascadedAttributeData.m_sStyle.m_ucTransparency   != m_sCascadedAttributeData.m_sStyle.m_ucTransparency
		||  sOtherCascadedAttribute.m_sCascadedAttributeData.m_sStyle.m_bSpecialCulling  != m_sCascadedAttributeData.m_sStyle.m_bSpecialCulling
		||  sOtherCascadedAttribute.m_sCascadedAttributeData.m_sStyle.m_bFrontCulling    != m_sCascadedAttributeData.m_sStyle.m_bFrontCulling
		||  sOtherCascadedAttribute.m_sCascadedAttributeData.m_sStyle.m_bBackCulling     != m_sCascadedAttributeData.m_sStyle.m_bBackCulling
		||  sOtherCascadedAttribute.m_sCascadedAttributeData.m_sStyle.m_bNoLight         != m_sCascadedAttributeData.m_sStyle.m_bNoLight)

		bSameDispalyParameter = bSamebSameDispalyParameterOnFace = false;
	else 
		bSameDispalyParameter   = true;

	return	 A3D_SUCCESS;
}

int ColorMaterialsConnector::GetMaterial(double dEmmisive[4],
															double dDiffuse[4],
															double dAmbient[4],
															double dSpecular[4],
															double& dShininess)
{
	int iRet =  eBAD_Diffuse + eBAD_Emissive + eBAD_Specular + eBAD_Ambient;
	A3DStatus iErr = A3D_SUCCESS;
	A3DGraphMaterialData sMaterialData;

	memset(dEmmisive, 0, sizeof(double) * 4);
	memset(dDiffuse, 0, sizeof(double) * 4);
	memset(dAmbient, 0, sizeof(double) * 4);
	memset(dSpecular, 0, sizeof(double) * 4);
	dShininess = 0;

	A3D_INITIALIZE_DATA(A3DGraphMaterialData, sMaterialData);
	iErr = A3DGlobalGetGraphMaterialData(m_sCascadedAttributeData.m_sStyle.m_uiRgbColorIndex, &sMaterialData);
	if(iErr == A3D_SUCCESS)
	{
		iRet = 0;
		A3DGraphRgbColorData sColorData;
		A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sColorData);

		if(sMaterialData.m_uiDiffuse != A3D_DEFAULT_COLOR_INDEX)
		{
			A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiDiffuse, &sColorData);

			dDiffuse[0] = sColorData.m_dRed;
			dDiffuse[1] = sColorData.m_dGreen;
			dDiffuse[2] = sColorData.m_dBlue;
			if(m_sCascadedAttributeData.m_sStyle.m_bIsTransparencyDefined)
			{
				dDiffuse[3] = (double)(m_sCascadedAttributeData.m_sStyle.m_ucTransparency) / 255.0;
			}
			else
			{
				dDiffuse[3] = 1.0;
			}
		}
		else
			iRet |= eBAD_Diffuse;

		if(sMaterialData.m_uiEmissive != A3D_DEFAULT_COLOR_INDEX)
		{
			A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiEmissive, &sColorData);
			dEmmisive[0] = sColorData.m_dRed;
			dEmmisive[1] = sColorData.m_dGreen;
			dEmmisive[2] = sColorData.m_dBlue;
			if(m_sCascadedAttributeData.m_sStyle.m_bIsTransparencyDefined)
			{
				dEmmisive[3] = sMaterialData.m_dEmissiveAlpha;
			}
			else
			{
				dEmmisive[3] = 1.0;
			}
		}
		else
			iRet |= eBAD_Emissive;


		dShininess = sMaterialData.m_dShininess;

		if(sMaterialData.m_uiSpecular != A3D_DEFAULT_COLOR_INDEX)
		{
			A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiSpecular, &sColorData);

			dSpecular[0] = sColorData.m_dRed;
			dSpecular[1] = sColorData.m_dGreen;
			dSpecular[2] = sColorData.m_dBlue;			
			if(m_sCascadedAttributeData.m_sStyle.m_bIsTransparencyDefined)
			{
				dSpecular[3] = sMaterialData.m_dSpecularAlpha;
			}
			else
			{
				dSpecular[3] = 1.0;
			}
		}
		else
			iRet |= eBAD_Specular;

		if(sMaterialData.m_uiAmbient != A3D_DEFAULT_COLOR_INDEX)
		{
			A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiAmbient, &sColorData);

			dAmbient[0] = sColorData.m_dRed;
			dAmbient[1] = sColorData.m_dGreen;
			dAmbient[2] = sColorData.m_dBlue;
			if(m_sCascadedAttributeData.m_sStyle.m_bIsTransparencyDefined)
			{
				dAmbient[3] = sMaterialData.m_dAmbientAlpha;
			}
			else
			{
				dAmbient[3] = 1.0;
			}
		}
		else
			iRet |= eBAD_Ambient;
	}
	return iRet;
}

