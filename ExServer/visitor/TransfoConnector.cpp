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

#include "TransfoConnector.h"
#include "TreeTraverse.h"
#include "Matrix.h"

static void stInitMatrixFromMiscTransformation(A3DMiscTransformation const * pMiscTransformation, A3DMatrix4x4 &sMatrix)
{
	if (pMiscTransformation)
	{
		A3DStatus iRet = A3D_SUCCESS;
		A3DEEntityType eType = kA3DTypeUnknown;
		A3DEntityGetType (pMiscTransformation, &eType);

		switch (eType)
		{
		case kA3DTypeMiscCartesianTransformation:
		{
			A3DMiscCartesianTransformationData sData;
			A3D_INITIALIZE_DATA(A3DMiscCartesianTransformationData, sData);

			iRet = A3DMiscCartesianTransformationGet (pMiscTransformation, &sData);

			double dMirror = (sData.m_ucBehaviour & kA3DTransformationMirror) ? -1. : 1.;

			A3DVector3dData sZVector;
			memset(sMatrix.m_adM, 0, 16 * sizeof(double));
			sZVector = CrossProduct(&(sData.m_sXVector), &(sData.m_sYVector));

			sMatrix.m_adM[12] = sData.m_sOrigin.m_dX;
			sMatrix.m_adM[13] = sData.m_sOrigin.m_dY;
			sMatrix.m_adM[14] = sData.m_sOrigin.m_dZ;

			sMatrix.m_adM[0] = sData.m_sXVector.m_dX*sData.m_sScale.m_dX;
			sMatrix.m_adM[1] = sData.m_sXVector.m_dY*sData.m_sScale.m_dX;
			sMatrix.m_adM[2] = sData.m_sXVector.m_dZ*sData.m_sScale.m_dX;

			sMatrix.m_adM[4] = sData.m_sYVector.m_dX*sData.m_sScale.m_dY;
			sMatrix.m_adM[5] = sData.m_sYVector.m_dY*sData.m_sScale.m_dY;
			sMatrix.m_adM[6] = sData.m_sYVector.m_dZ*sData.m_sScale.m_dY;

			sMatrix.m_adM[8] = dMirror*sZVector.m_dX*sData.m_sScale.m_dZ;
			sMatrix.m_adM[9] = dMirror*sZVector.m_dY*sData.m_sScale.m_dZ;
			sMatrix.m_adM[10] = dMirror*sZVector.m_dZ*sData.m_sScale.m_dZ;

			sMatrix.m_adM[15] = 1.;

			iRet = A3DMiscCartesianTransformationGet (NULL, &sData);
		}
		break;

		case kA3DTypeMiscGeneralTransformation:
		{
			A3DMiscGeneralTransformationData sData;
			A3D_INITIALIZE_DATA(A3DMiscGeneralTransformationData, sData);

			iRet = A3DMiscGeneralTransformationGet (pMiscTransformation, &sData);

			memcpy(&(sMatrix.m_adM[0]), sData.m_adCoeff, 16 * sizeof(double));

			iRet = A3DMiscGeneralTransformationGet (NULL, &sData);
		}
		break;

		default:
			break;
		}
	}
}

A3DTransfoConnector::A3DTransfoConnector(const A3DMiscTransformation* pMiscTransformation)
:	A3DConnector(pMiscTransformation)
{
	if (pMiscTransformation)
	{
		stInitMatrixFromMiscTransformation(pMiscTransformation, m_sLocalMatrix);
	}
	else
		m_sLocalMatrix = stIdentity;
}

void A3DTransfoConnector::ComputeGlobalMatrix(A3DMatrix4x4 const& rFatherWorldMatrix) 
{
	m_sWorldMatrix = rFatherWorldMatrix * m_sLocalMatrix;
}

void A3DTransfoConnector::SetGlobalMatrix(A3DMatrix4x4 const& rWorldMatrix) 
{
	m_sWorldMatrix = rWorldMatrix;
}

void A3DTransfoConnector::SetGlobalMatrix(A3DMiscTransformation const * pMiscTransformation)
{
	stInitMatrixFromMiscTransformation(pMiscTransformation, m_sWorldMatrix);
}

A3DStatus A3DTransfoConnector::GetLocalMatrix( A3DMatrix4x4& rsMatrix )
{
	rsMatrix = m_sLocalMatrix;
	return A3D_SUCCESS;
}

A3DStatus A3DTransfoConnector::GetGlobalTransfo(A3DMatrix4x4& rsMatrix)
{
	rsMatrix = m_sWorldMatrix;
	return A3D_SUCCESS;
}

