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

#ifndef A3DMATRIX_H
#define A3DMATRIX_H

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <A3DSDKErrorCodes.h>
#include "A3DVector.h"

#define A3D_PI			3.1415926535897932384626433832795028841971693993751
#define A3D_HALF_PI	1.5707963267948966192313216916397514420985846996875



class A3DMatrix4x4
{
public:
	double m_adM[16];
	//column first
	//m_adM[0] = m00		m_adM[4] = m01		m_adM[8] = m02		m_adM[12] = m03
	//m_adM[1] = m10		m_adM[5] = m11		m_adM[9] = m12		m_adM[13] = m13
	//m_adM[2] = m20		m_adM[6] = m21		m_adM[10] = m22	m_adM[14] = m23
	//m_adM[3] = m30		m_adM[7] = m31		m_adM[11] = m32	m_adM[15] = m33
	A3DMatrix4x4(double* pdMat) { memcpy(&m_adM[0],  pdMat, sizeof(double) * 16);	}
	A3DMatrix4x4() { ResetToIdentity(); }

	static A3DStatus CreateIdentity(A3DMatrix4x4*& psIdentity);

	A3DMatrix4x4 operator * (const A3DMatrix4x4& matrix) const;

	A3DVector3d operator * (const A3DVector3d& pt) const;

	A3DStatus Invert(A3DMatrix4x4& sResult);

	A3DStatus GetTranslation(A3DVector3d& sTranslation);

	void ResetToIdentity();
	bool IsIdentity() const;

	void GetScales(double& rdSX, double& rdSY, double& rdSZ) const;
	void GetTranslation(double& rdTX, double& rdTY, double& rdTZ) const;
	void GetXYZRadianAngles(double& rdXAngle, double& rdYAngle, double& rdZAngle) const;

	double const& operator[](const unsigned& u) const { return m_adM[u]; }
	void setRotations(double const& dXAngle, double const& dYAngle, double const& dZAngle);

	A3DMatrix4x4& operator = (const A3DMatrix4x4& rFrom)
	{
		memcpy(m_adM, rFrom.m_adM, sizeof(double) * 16);
		return *this;
	}
	bool Compare(A3DMatrix4x4 const& rOther, double dTol = 0.0) const;

};


A3DStatus MultiplyMatrix(const double* padFather, const double* pdThisMatrix, double* pdResult);

A3DStatus Invert4x4(const double* pdMat, double* pdMatInv);

A3DStatus Calculate(double* pdX, double* pdY, double* pdZ, double* pdMat);

A3DStatus UnProject(	const double * pPt,
							const double* pdModelMatrix, 
							const double* pdProjMatrix,
							const int* viewport,
							double* pPtresult);

A3DStatus Project(double objx,
						double objy,
						double objz, 
						const double pdModelMatrix[16], 
						const double pdProjMatrix[16],
						const int viewport[4],
						double *winx,
						double *winy,
						double *winz);


A3DStatus Ortho(const double* pdCurrentMatrix,
					 double dLeft,
					 double dRight, 
					 double dTop, 
					 double dBottom, 
					 double dNear,
					 double dfar,
					 double* pdOutput);

A3DStatus Identity(double* pdIdentity);

A3DVector3dData CrossProduct(const A3DVector3dData* X, const A3DVector3dData* Y);

static const A3DMatrix4x4 stIdentity;
#endif
