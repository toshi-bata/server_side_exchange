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

#ifndef A3D_CASCADED_ATTRIBUTE_CLASS
#define A3D_CASCADED_ATTRIBUTE_CLASS

#include "Connector.h"


class ColorMaterialsConnector  : public A3DConnector
{

public:
	A3DMiscCascadedAttributesData m_sCascadedAttributeData;

	ColorMaterialsConnector(A3DMiscCascadedAttributes* pAttr);

	virtual ~ColorMaterialsConnector();

	bool IsShow();
	
	bool IsRemoved();

	bool IsTexture();

	bool IsMaterial();
	//A3DStatus GetMaterial(A3DMaterial& sMaterial);
	enum eMaterialState
	{
		eBAD_Emissive = 1,
		eBAD_Diffuse = 2,
		eBAD_Ambient = 4,
		eBAD_Specular = 8
	};


	int GetMaterial(double dEmisive[4], double dDiffuse[4], double dAmbient[4], double dSpecular[4], double& dShininess);

	bool IsRGBColor();
	A3DStatus GetRGB(double adRG[4]);
	//From 0 (transparent) to 255 (opaque).
	bool IsTranparent();
	A3DStatus Transparency(unsigned char& cTransparenty);

	A3DStatus Compare(
					ColorMaterialsConnector const & sOtherCascadedAttribute,
					bool& bSameColor, 
					bool&  bSameFaceColor);

};
#endif
