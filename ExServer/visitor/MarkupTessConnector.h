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

#ifndef	 A3D_MKP_CONNECTOR
#define	 A3D_MKP_CONNECTOR

#include <vector>
#include "Connector.h"
#include <assert.h>
#include <stdlib.h>

/*********************************************************************************************
	A3DMarkupTessDataConnectorCreator is in charge of creating new	A3DMarkupTessConnector
	each time a graphical state change when traversing markup tessellation structure
	
	At the end, A3DMarkupTessDataConnectorCreator ensure that
	Tesselated data (point, polyline, triangles, Cone, text) that have the same Graphical data 
	(color, linestyle , pattern) and same local Matrix are group in the same MarkupTessConnector

**********************************************************************************************/
class A3DMarkupTessConnector;
class A3DMarkupTessDataConnectorCreator
{
public:
	A3DMarkupTessDataConnectorCreator(std::vector<A3DMarkupTessConnector*>& a_rvMkpTessConnector);
	~A3DMarkupTessDataConnectorCreator();
	
	A3DStatus pushMatrix(const double* a_pdLocalMatrix);
 	A3DStatus popMatrix();
	const double* GetCurrentWorldMatrix() { return &m_adCurrentWorldMatrix[0]; }

	A3DMarkupTessConnector& GetMarkupTessConnector(const double* pdMatrix);
	A3DMarkupTessConnector& GetMarkupTessConnector(unsigned uiExtraDataValue, bool& bCameraDependant);
	
	bool HasStartToDrawSet() const { return m_bStartToDrawSet; }

private:
	// assignement operator can't work because of the ref member (std::vector<A3DMarkupTessConnector*>&)
	A3DMarkupTessDataConnectorCreator& operator= (const A3DMarkupTessDataConnectorCreator&) { assert(false); return *this; }

private:
	bool m_bStartToDrawSet;

	//Camera dependant Parameter:
	bool m_bFaceViewActivated;
	bool m_bFramedrawActivated;
	bool m_bFixedSizeActivated; 

	std::vector<A3DMarkupTessConnector*>& m_rvMarkupTessData;
	double m_adCurrentWorldMatrix[16];
	std::vector<double> m_adPushedWorldMatrices;
};

/*********************************************************************************************
					PMI Text entry
*********************************************************************************************/
class A3DPMITextEntry
{
public:

	A3DPMITextEntry() : m_dTextWidth(0.0), m_dTextHeight(0.0f), m_apcText(NULL) {}

	A3DPMITextEntry(A3DDouble a_dTextWidth, A3DDouble a_dTextHeight, 
					A3DFontKeyData const& a_rFontKeyData, A3DUTF8Char* a_apcText) : 
		m_dTextWidth(a_dTextWidth), m_dTextHeight(a_dTextHeight), m_oFontKeyData(a_rFontKeyData)
	{
		SetText(a_apcText);
	}

	~A3DPMITextEntry()
	{
		free(m_apcText);
	}

	void SetTextWidth(A3DDouble a_dValue) { m_dTextWidth = a_dValue; }
	void SetTextHeight(A3DDouble a_dValue) { m_dTextHeight = a_dValue; }
	A3DDouble GetTextWidth() const { return m_dTextWidth; }
	A3DDouble GetTextHeight() const { return m_dTextHeight; }
	void SetText(A3DUTF8Char const * a_apcText)
	{
		auto iSize = strlen(a_apcText);
		m_apcText = (A3DUTF8Char*)malloc(iSize + 1);
		strcpy_s(m_apcText, iSize + 1, a_apcText);
	}
	A3DUTF8Char* GetText() const { return m_apcText; }
	A3DFontKeyData const& GetFontKeyData() { return m_oFontKeyData; }

private:
	A3DDouble m_dTextWidth;
	A3DDouble m_dTextHeight;
	A3DFontKeyData m_oFontKeyData;
	A3DUTF8Char* m_apcText;
};

 /*********************************************************************************************
					Markup	Tessellation
**********************************************************************************************/
class A3DMarkupTessConnector : public A3DConnector
{
  friend class A3DMarkupConnector;
  friend class A3DMarkupTessDataConnectorCreator;

public:
	A3DMarkupTessConnector();
	A3DMarkupTessConnector(A3DMarkupTessConnector& a_PreviousTessConnector);
	~A3DMarkupTessConnector();

	bool HasAlreadySomethingToDisplay() const { return (m_adPoints.size() != 0) || (m_adPolylineCoords.size() != 0)
											 || (m_aiPolylineIndices.size() != 0)
											 || (m_adTriangleList.size() != 0) || (m_adCylinder.size() != 0)
											 || (m_apTexts.size() != 0); }

	// Geometrical Data
	std::vector<double> const& GetPolyLineCoords() const { return m_adPolylineCoords; }
	std::vector<int> const& GetPolyLineIndices() const { return m_aiPolylineIndices; }
	std::vector<A3DPMITextEntry*> const& GetTextEntries() const { return m_apTexts; }	
	std::vector<double> const& GetTriangles() const { return m_adTriangleList; }
	std::vector<double> const& GetCylinder() const { return m_adCylinder;}
	std::vector<double> const& GetPoints() const { return m_adPoints;}
	
	// Transfo
	double const * GetWorldMatrix() const { return &m_adWorldMatrix[0]; }

	void GetFrameDrawWorldOffset(double a_adOfs[3]) const
									{ memcpy(a_adOfs, m_adViewDependantParameter, sizeof(m_adViewDependantParameter)); }

	A3DStatus ComputeCameraDependantMatrix(	double const * pdProjMatrix, 
										int const * aiViewport, 
										long const * Rect, // TOP_LEFT(X,Y) BOTTOM_RIGHT(X,Y) 
										double* m_adPerViewMatrix);
	
	bool IsHidden()				const { return (m_ucBehaviour&0x01)		    == 0 ? false : true;	}
	bool HasFrame()				const { return (m_ucBehaviour&0x02)		    == 0 ? false : true;	}
	bool IsNotModifiable()		const { return (m_ucBehaviour&0x04)			== 0 ? false : true;	}
	bool IsZoomable()			const { return (m_ucBehaviour&0x08)			== 0 ? false : true;	}
	bool IsOnTop()				const { return (m_ucBehaviour&0x10)		    == 0 ? false : true;	}
	bool IsFacingView()			const { return m_bFaceView ; }
	bool IsFrameDraw()			const { return m_bFrameDraw ; }
	bool HasColorSetup()		const { return m_adRGB[0] >= 0.0; }

	// Graphic
	void GetColor(double a_adRGB[3]) const
	{
		memcpy(a_adRGB, m_adRGB, sizeof(m_adRGB));
	}
	double GetLineWidth() const { return m_dLineWidth;}
	int GetStyleIndexLineStyle() const { return m_iGraphStyleIndexLinsStyle;}
	int GetPicturePattennIndex() const { return m_iGraphVPicturePatternIndex;}

private:
	static A3DStatus GetMarkupTessellation(	const A3DTessBaseData& sTessBaseData,
											const A3DTessMarkupData* pTessMarkupData,
											const bool bIsText,
											std::vector<A3DMarkupTessConnector*>& asMarkupTessDataConnector,
											double a_dParallelToScreenPMIScale);

private:
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)	// Suppress the warning C4251 because it leads to have that kind of warning : "warning 
	// C4251: 'A3DVisitorColorMaterials::m_apsCascadedAttribute' : class 'std::vector<_Ty>' needs to have dll-interface to 
	// be used by clients of class 'A3DVisitorColorMaterials"
	// This is because of the "dllexport" on the visitor classes, as they use stl members and it seems that stl causes 
	// warning with dllexport with microsoft compiler
#endif // _MSC_VER

	// Parameter useful to compute camera dependant Matrix
	bool m_bFaceView;
	bool m_bFrameDraw;
	bool m_bSymbol;
	bool m_bFixedSize;

	bool m_bIsText;
	double m_adViewDependantParameter[3];
	unsigned m_uiFixedSizePatternIndex;

	char m_ucBehaviour;

	double m_adModelMatrix[16];
	double m_adProjMatrix[16];

	//Transfo
	double m_adWorldMatrix[16];

	//Graphic
	double m_adRGB[3];
	double m_dLineWidth;
	int m_iGraphStyleIndexLinsStyle;
	int m_iGraphVPicturePatternIndex;

	//Geometrical data
	std::vector<double> m_adPoints;
	std::vector<int> m_aiPolylineIndices;
	std::vector<double> m_adPolylineCoords;
	std::vector<double> m_adTriangleList;
	std::vector<double> m_adCylinder;	// n x (m_dRadiusBase, m_dRadiusTop, m_dHeight)

	//Text
	std::vector<A3DPMITextEntry*> m_apTexts;

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER
};

#endif

										