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

#include "Matrix.h"
#include <math.h>

#include <float.h>
#include "A3DVector.h"

////////////////////////////////////////////////////////////////////////
//							A3DMatrix4x4
///////////////////////////////////////////////////////////////////////


A3DStatus A3DMatrix4x4::CreateIdentity(A3DMatrix4x4*& psIdentity)
{
	// by default  A3DMatrix4x4() call a ResetToIdentity
	psIdentity = new A3DMatrix4x4();
	return A3D_SUCCESS;
}


A3DMatrix4x4 A3DMatrix4x4 :: operator * (const A3DMatrix4x4& matrix) const
{
	A3DMatrix4x4 result;
	for (int i=0;i<4;i++)
	{
		for (int j=0;j<4;j++)
		{
			result.m_adM[i+4*j] = 0.;
			for (int k=0;k<4;k++)
				result.m_adM[i+4*j] += m_adM[i+4*k] * matrix.m_adM[k+4*j];
		}
	}
	return result;
}


A3DVector3d A3DMatrix4x4 :: operator * (const A3DVector3d& v) const
{
	A3DVector3d Result;
	Result.x = m_adM[0] * v.x + m_adM[4] * v.y + m_adM[8] * v.z + m_adM[12];
	Result.y = m_adM[1] * v.x + m_adM[5] * v.y + m_adM[9] * v.z + m_adM[13];
	Result.z = m_adM[2] * v.x + m_adM[6] * v.y + m_adM[10] * v.z + m_adM[14];
	return Result;
}

A3DStatus A3DMatrix4x4::Invert(A3DMatrix4x4& sResult)
{
	return Invert4x4(m_adM, sResult.m_adM);
}

A3DStatus A3DMatrix4x4::GetTranslation(A3DVector3d& sTranslation)
{
	sTranslation.x = m_adM[12];
	sTranslation.y = m_adM[13];
	sTranslation.z = m_adM[14];
	return A3D_SUCCESS;
}

void A3DMatrix4x4::ResetToIdentity()
{
	memset(m_adM, 0, sizeof(double) * 16);
	m_adM[0] = m_adM[5] = m_adM[10] = m_adM[15] = 1;
}

bool A3DMatrix4x4::IsIdentity() const
{
	if((m_adM[0] != 1.0) || (m_adM[1] != 0.0) || (m_adM[2] != 0.0) || (m_adM[3] != 0.0)
	|| (m_adM[4] != 0.0) || (m_adM[5] != 1.0) || (m_adM[6] != 0.0) || (m_adM[7] != 0.0)
	|| (m_adM[8] != 0.0) || (m_adM[9] != 0.0) || (m_adM[10] != 1.0) || (m_adM[11] != 0.0)
	|| (m_adM[12] != 0.0) || (m_adM[13] != 0.0) || (m_adM[14] != 0.0) || (m_adM[15] != 1.0))
	{
		return false;
	}
	return true;
}

void A3DMatrix4x4::GetTranslation( double& rdTX, double& rdTY, double& rdTZ ) const
{
	rdTX = m_adM[12];
	rdTY = m_adM[13];
	rdTZ = m_adM[14];
}

void A3DMatrix4x4::GetScales( double& rdSX, double& rdSY, double& rdSZ ) const
{
	rdSX = sqrt(m_adM[0]*m_adM[0] + m_adM[1]*m_adM[1] + m_adM[2]*m_adM[2]);
	rdSY = sqrt(m_adM[4]*m_adM[4] + m_adM[5]*m_adM[5] + m_adM[6]*m_adM[6]);
	rdSZ = sqrt(m_adM[8]*m_adM[8] + m_adM[9]*m_adM[9] + m_adM[10]*m_adM[10]);
}

void A3DMatrix4x4::GetXYZRadianAngles( double& rdXAngle, double& rdYAngle, double& rdZAngle ) const
{
	//matrix3x3 of rotation without scale
	double dSx, dSy, dSz;
	GetScales(dSx, dSy, dSz);
	double rot[9];
	rot[0] = m_adM[0]/dSx;			rot[3] = m_adM[4]/dSy;			rot[6] = m_adM[8]/dSz;
	rot[1] = m_adM[1]/dSx;			rot[4] = m_adM[5]/dSy;			rot[7] = m_adM[9]/dSz;
	rot[2] = m_adM[2]/dSx;			rot[5] = m_adM[6]/dSy;			rot[8] = m_adM[10]/dSz;


// 	rdYAngle = asin(rot[2]);				//attitude : 
// 	if(rdYAngle < A3D_HALF_PI)
// 		if(rdYAngle > - A3D_HALF_PI)
// 		{
// 			rdXAngle = atan2(-rot[5], rot[8]);//bank : rotation along the "look at axis" (1 0 0)
// 			rdZAngle = atan2(-rot[1], rot[0]);//heading : rotation along up axis (0 0 1)
// 		}
// 		else
// 		{
// 			rdZAngle = 0.0; // multiple solutions
// 			rdXAngle = -atan2(rot[4], rot[5]);
// 		}
// 	else
// 	{
// 		rdZAngle = 0.0; // multiple solutions
// 		rdXAngle = atan2(rot[4], rot[5]);
// 	}
	if(rot[1] > 0.998)
	{
		rdZAngle = atan2(rot[6], rot[8]);
		rdYAngle = A3D_HALF_PI;
		rdXAngle = 0.0;
	}
	else if(rot[1] < -.998)
	{
		rdZAngle = atan2(rot[6], rot[8]);
		rdYAngle = -A3D_HALF_PI;
		rdXAngle = 0.0;
	}
	else
	{
		rdZAngle = atan2(-rot[2], rot[0]);
		rdYAngle = asin(rot[1]);
		if(rot[7] == 0.0 && rot[4] == 0)
		{
			int sign = (rot[7] > 0) - (rot[7] < 0);
			if(sign >= 0)
				rdXAngle = A3D_HALF_PI;
			else
				rdXAngle = -A3D_HALF_PI;
		}
		else
			rdXAngle = atan2(-rot[7], rot[4]);
	}

// 
// 	double cX = cos(rdXAngle);
// 	double sX = sin(rdXAngle);
// 	double cY = cos(rdYAngle);
// 	double sY = sin(rdYAngle);
// 	double cZ = cos(rdZAngle);
// 	double sZ = sin(rdZAngle);
// 	
// 	A3DMatrix4x4 testResult;
// 	testResult.m_adM[0] = cZ*cY;		testResult.m_adM[4] = -cZ*sY*cX + sZ*sX;	testResult.m_adM[8] = cZ*sY*sX + sZ*cX;
// 	testResult.m_adM[1] = sY;			testResult.m_adM[5] = cY*cX;					testResult.m_adM[9] = -cY*sX;
// 	testResult.m_adM[2] = -sZ*cY;		testResult.m_adM[6] = sZ*sY*cX + cZ*sX;	testResult.m_adM[10] = -sZ*sY*sX + cZ*cX;
// 	bool bRes = testResult.Compare(this);

}

void A3DMatrix4x4::setRotations(double const& dXAngle, double const& dYAngle, double const& dZAngle)
{
	//        Real fCos, fSin;

// 	fCos = Math::Cos(fYAngle);
// 	fSin = Math::Sin(fYAngle);
// 	Matrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);
// 
// 	fCos = Math::Cos(fPAngle);
// 	fSin = Math::Sin(fPAngle);
// 	Matrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);
// 
// 	fCos = Math::Cos(fRAngle);
// 	fSin = Math::Sin(fRAngle);
// 	Matrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);
// 
// 	*this = kXMat*(kYMat*kZMat);

	double dCos, dSin;
	dCos = cos(dXAngle);
	dSin = sin(dXAngle);

	A3DMatrix4x4 AX;
	AX.m_adM[5] = dCos;
	AX.m_adM[6] = -dSin;
	AX.m_adM[9] = dSin;
	AX.m_adM[10] = dCos;

	dCos = cos(dYAngle);
	dSin = sin(dYAngle);

	A3DMatrix4x4 AY;
	AY.m_adM[0] = dCos;
	AY.m_adM[2] = dSin;
	AY.m_adM[8] = -dSin;
	AY.m_adM[10] = dCos;

	dCos = cos(dZAngle);
	dSin = sin(dZAngle);

	A3DMatrix4x4 AZ;
	AZ.m_adM[0] = dCos;
	AZ.m_adM[1] = -dSin;
	AZ.m_adM[4] = dSin;
	AZ.m_adM[5] = dCos;


	*this = AX * (AY * AZ);
}

bool A3DMatrix4x4::Compare( A3DMatrix4x4 const& rOther, double dTol ) const
{
	if(dTol != 0.0)
	{
		for(int i =0; i < 16; i++)
		{
			if(m_adM[i] != rOther.m_adM[i])
				return false;
		}
	}
	else
	{
		for(int i =0; i < 16; i++)
		{
			if(fabs(m_adM[i] - rOther.m_adM[i]) < dTol)
				return false;
		}
	}
	return true;
}

static void MultMatrixVecd(const double matrix[16],
									const double in[4],
									double out[4])
{
	int i;

	for (i=0; i<4; i++) {
	out[i] = 
		in[0] * matrix[0*4+i] +
		in[1] * matrix[1*4+i] +
		in[2] * matrix[2*4+i] +
		in[3] * matrix[3*4+i];
	}
}

/*
** Invert 4x4 matrix.
** Contributed by David Moore (See Mesa bug #6748)
*/
static bool InvertMatrixd(const double m[16], double invOut[16])
{
	double inv[16], det;
	int i;

	inv[0] =   m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15]
				+ m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
	inv[4] =  -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15]
				- m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
	inv[8] =   m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15]
				+ m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
	inv[12] = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14]
				- m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];
	inv[1] =  -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15]
				- m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
	inv[5] =   m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15]
				+ m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
	inv[9] =  -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15]
				- m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
	inv[13] =  m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14]
				+ m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
	inv[2] =   m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15]
				+ m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
	inv[6] =  -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15]
				- m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
	inv[10] =  m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15]
				+ m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
	inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14]
				- m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
	inv[3] =  -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11]
				- m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
	inv[7] =   m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11]
				+ m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
	inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11]
				- m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];
	inv[15] =  m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10]
				+ m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];

	det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];
	if (det == 0)
		return false;

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		invOut[i] = inv[i] * det;

	return true;
}

static void MultMatricesd(const double a[16], const double b[16],
								  double r[16])
{
	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			r[i*4+j] = 
				a[i*4+0]*b[0*4+j] +
				a[i*4+1]*b[1*4+j] +
				a[i*4+2]*b[2*4+j] +
				a[i*4+3]*b[3*4+j];
		}
	}
}


A3DStatus UnProject(	double winx,
							double winy,
							double winz,
							const double modelMatrix[16], 
							const double projMatrix[16],
							const int viewport[4],
							double *objx, double *objy, double *objz)
{
	double finalMatrix[16];
	double in[4];
	double out[4];

	MultMatricesd(modelMatrix, projMatrix, finalMatrix);
	if (!InvertMatrixd(finalMatrix, finalMatrix)) 
		return A3D_ERROR;

	in[0]=winx;
	in[1]=winy;
	in[2]=winz;
	in[3]=1.0;

	/* Map x and y from window coordinates */
	in[0] = (in[0] - viewport[0]) / viewport[2];
	in[1] = (in[1] - viewport[1]) / viewport[3];

	/* Map to range -1 to 1 */
	in[0] = in[0] * 2 - 1;
	in[1] = in[1] * 2 - 1;
	in[2] = in[2] * 2 - 1;

	MultMatrixVecd(finalMatrix, in, out);
	if (out[3] == 0.0) 
		return A3D_ERROR;
	out[0] /= out[3];
	out[1] /= out[3];
	out[2] /= out[3];
	*objx = out[0];
	*objy = out[1];
	*objz = out[2];
	return A3D_SUCCESS;
}

A3DStatus Project(double objx,
						double objy,
						double objz,
						const double modelMatrix[16],
						const double projMatrix[16],
						const int viewport[4],
						double *winx,
						double *winy,
						double *winz)
{
	double in[4];
	double out[4];

	in[0]=objx;
	in[1]=objy;
	in[2]=objz;
	in[3]=1.0;
	MultMatrixVecd(modelMatrix, in, out);
	MultMatrixVecd(projMatrix, out, in);
	if (in[3] == 0.0) return A3D_ERROR;
	in[0] /= in[3];
	in[1] /= in[3];
	in[2] /= in[3];
	/* Map x, y and z to range 0-1 */
	in[0] = in[0] * 0.5 + 0.5;
	in[1] = in[1] * 0.5 + 0.5;
	in[2] = in[2] * 0.5 + 0.5;

	/* Map x,y to viewport */
	in[0] = in[0] * viewport[2] + viewport[0];
	in[1] = in[1] * viewport[3] + viewport[1];

	*winx=in[0];
	*winy=in[1];
	*winz=in[2];

	return A3D_SUCCESS;
}


/*******************************************************
			Multiply Matrix
 ********************************************************/
A3DStatus MultiplyMatrix(const double* padFather, const double* pdThisMatrix, double* pdResult)
{
	A3DStatus iRet = A3D_SUCCESS; 
	pdResult[0]  = padFather[0] * pdThisMatrix[0] + padFather[4] * pdThisMatrix[1] + padFather[8] * pdThisMatrix[2] + padFather[12] * pdThisMatrix[3];
	pdResult[1]  = padFather[1] * pdThisMatrix[0] + padFather[5] * pdThisMatrix[1] + padFather[9] * pdThisMatrix[2] + padFather[13] * pdThisMatrix[3];
	pdResult[2]  = padFather[2] * pdThisMatrix[0] + padFather[6] * pdThisMatrix[1] + padFather[10] * pdThisMatrix[2] + padFather[14] * pdThisMatrix[3];
	pdResult[3]  = padFather[3] * pdThisMatrix[0] + padFather[7] * pdThisMatrix[1] + padFather[11] * pdThisMatrix[2] + padFather[15] * pdThisMatrix[3];
	pdResult[4]  = padFather[0] * pdThisMatrix[4] + padFather[4] * pdThisMatrix[5] + padFather[8] * pdThisMatrix[6] + padFather[12] * pdThisMatrix[7];
	pdResult[5]  = padFather[1] * pdThisMatrix[4] + padFather[5] * pdThisMatrix[5] + padFather[9] * pdThisMatrix[6] + padFather[13] * pdThisMatrix[7];
	pdResult[6]  = padFather[2] * pdThisMatrix[4] + padFather[6] * pdThisMatrix[5] + padFather[10] * pdThisMatrix[6] + padFather[14] * pdThisMatrix[7];
	pdResult[7]  = padFather[3] * pdThisMatrix[4] + padFather[7] * pdThisMatrix[5] + padFather[11] * pdThisMatrix[6] + padFather[15] * pdThisMatrix[7];
	pdResult[8]  = padFather[0] * pdThisMatrix[8] + padFather[4] * pdThisMatrix[9] + padFather[8] * pdThisMatrix[10] + padFather[12] * pdThisMatrix[11];
	pdResult[9]  = padFather[1] * pdThisMatrix[8] + padFather[5] * pdThisMatrix[9] + padFather[9] * pdThisMatrix[10] + padFather[13] * pdThisMatrix[11];
	pdResult[10] = padFather[2] * pdThisMatrix[8] + padFather[6] * pdThisMatrix[9] + padFather[10] * pdThisMatrix[10] + padFather[14] * pdThisMatrix[11];
	pdResult[11] = padFather[3] * pdThisMatrix[8] + padFather[7] * pdThisMatrix[9] + padFather[11] * pdThisMatrix[10] + padFather[15] * pdThisMatrix[11];
	pdResult[12] = padFather[0] * pdThisMatrix[12] + padFather[4] * pdThisMatrix[13] + padFather[8] * pdThisMatrix[14] + padFather[12] * pdThisMatrix[15];
	pdResult[13] = padFather[1] * pdThisMatrix[12] + padFather[5] * pdThisMatrix[13] + padFather[9] * pdThisMatrix[14] + padFather[13] * pdThisMatrix[15];
	pdResult[14] = padFather[2] * pdThisMatrix[12] + padFather[6] * pdThisMatrix[13] + padFather[10] * pdThisMatrix[14] + padFather[14] * pdThisMatrix[15];
	pdResult[15] = padFather[3] * pdThisMatrix[12] + padFather[7] * pdThisMatrix[13] + padFather[11] * pdThisMatrix[14] + padFather[15] * pdThisMatrix[15];
	return iRet;
}


static void MatrixVectMultiply(const double* pdMatrix,
										 const double* pdVect,
										 double* pdresult)
{				
	unsigned int uI;

	for (uI = 0; uI < 4; uI++)
	{
		pdresult[uI] = 
			pdVect[0] * pdMatrix[uI] +
			pdVect[1] * pdMatrix[4+uI] +
			pdVect[2] * pdMatrix[8+uI] +
			pdVect[3] * pdMatrix[12+uI];
	}
}

/*******************************************************
			Invert	Matrix
 ********************************************************/
A3DStatus Calculate(double* pdX, double* pdY, double* pdZ, double* pdMat)
{
	double dLenght = sqrt(pdX[0] * pdX[0] + pdX[1] * pdX[1] + pdX[2] * pdX[2]);
	if(dLenght < 1e-12)
		return A3D_ERROR;
	pdX[0] /= dLenght;  pdX[1] /= dLenght; pdX[2] /= dLenght;

	dLenght = sqrt(pdY[0] * pdY[0] + pdY[1] * pdY[1] + pdY[2] * pdY[2]);
	if(dLenght < 1e-12)
		return A3D_ERROR;
	pdY[0] /= dLenght;  pdY[1] /= dLenght; pdY[2] /= dLenght;

	dLenght = sqrt(pdZ[0] * pdZ[0] + pdZ[1] * pdZ[1] + pdZ[2] * pdZ[2]);
	if(dLenght < 1e-12)
		return A3D_ERROR;
	pdZ[0] /= dLenght;  pdZ[1] /= dLenght; pdZ[2] /= dLenght;
	
	pdMat[0] = pdX[0];
	pdMat[1] = pdX[1];
	pdMat[2] = pdX[2];
	pdMat[4] = pdY[0];
	pdMat[5] = pdY[1];
	pdMat[6] = pdY[2];
	pdMat[8] = pdZ[0];
	pdMat[9] = pdZ[1];
	pdMat[10]= pdZ[2];

	pdMat[3] = pdMat[7] = pdMat[11] = pdMat[12] = pdMat[13] = pdMat[14] = pdMat[15] = 0;

	return A3D_SUCCESS;
}


/*******************************************************
			Invert	Matrix
********************************************************/
double det2x2(double a, double b, double c, double d)
{
	double det;
	det = a * d - b * c;
	return det;
}

double det3x3(double a1, double a2, double a3,
				  double b1, double b2, double b3,
				  double c1, double c2, double c3)
{
	double det;
	det =   a1 * det2x2(b2, b3, c2, c3) 
			- b1 * det2x2(a2, a3, c2, c3) 
			+ c1 * det2x2(a2, a3, b2, b3);
	return det;
}

 void adjoint(double* pdMatrix)
{
	double a1, a2, a3, a4, b1, b2, b3, b4;
	double c1, c2, c3, c4, d1, d2, d3, d4;
	

	a1 = pdMatrix[0]; b1 = pdMatrix[1]; 
	c1 = pdMatrix[2]; d1 = pdMatrix[3];
	a2 = pdMatrix[4]; b2 = pdMatrix[5]; 
	c2 = pdMatrix[6]; d2 = pdMatrix[7];
	a3 = pdMatrix[8]; b3 = pdMatrix[9];
	c3 = pdMatrix[10]; d3 = pdMatrix[11];
	a4 = pdMatrix[12]; b4 = pdMatrix[13]; 
	c4 = pdMatrix[14]; d4 = pdMatrix[15];

	pdMatrix[0]  =   det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4);
	pdMatrix[4]  = - det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4);
	pdMatrix[8]  =   det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4);
	pdMatrix[12]  = - det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);

	pdMatrix[1]  = - det3x3( b1, b3, b4, c1, c3, c4, d1, d3, d4);
	pdMatrix[5]  =   det3x3( a1, a3, a4, c1, c3, c4, d1, d3, d4);
	pdMatrix[9]  = - det3x3( a1, a3, a4, b1, b3, b4, d1, d3, d4);
	pdMatrix[13]  =   det3x3( a1, a3, a4, b1, b3, b4, c1, c3, c4);

	pdMatrix[2]  =   det3x3( b1, b2, b4, c1, c2, c4, d1, d2, d4);
	pdMatrix[6]  = - det3x3( a1, a2, a4, c1, c2, c4, d1, d2, d4);
	pdMatrix[10]  =   det3x3( a1, a2, a4, b1, b2, b4, d1, d2, d4);
	pdMatrix[14]  = - det3x3( a1, a2, a4, b1, b2, b4, c1, c2, c4);

	pdMatrix[3]  = - det3x3( b1, b2, b3, c1, c2, c3, d1, d2, d3);
	pdMatrix[7]  =   det3x3( a1, a2, a3, c1, c2, c3, d1, d2, d3);
	pdMatrix[11]  = - det3x3( a1, a2, a3, b1, b2, b3, d1, d2, d3);
	pdMatrix[15]  =   det3x3( a1, a2, a3, b1, b2, b3, c1, c2, c3);
}

A3DStatus Invert4x4(const double* pdMat, double* pdMatInv)
{
	double det;
	int i,j;
	double scale = pdMat[15];
	
	if (fabs(scale) < FLT_MIN)
	{
		/*"Matrice avec Scale == 0.: inversion impossible\n"*/
		return A3D_ERROR;
	}
	
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			pdMatInv[i*4 + j] = pdMat[i*4 + j];
	
	adjoint(pdMatInv);
	det = det3x3(pdMat[0], pdMat[4], pdMat[8],
					 pdMat[1], pdMat[5], pdMat[9],
					 pdMat[2], pdMat[6], pdMat[10]);
	
	if (fabs(det) < FLT_MIN)
	{
		/*"Matrice non reguliere: inversion impossible\n"*/
		return A3D_ERROR;
	}
	
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			pdMatInv[i * 4 +j] /= (scale*det);

	return A3D_SUCCESS;
}

  
A3DStatus UnProject(const double * pPt,
						  const double* pdModelMatrix, 
						  const double* pdProjMatrix,
						  const int* viewport,
						  double* pPtresult)
{
	double finalMatrix[16];
	double in[4];
	double out[4];

	MultiplyMatrix(pdModelMatrix, pdProjMatrix, finalMatrix);
	if (!Invert4x4(finalMatrix, finalMatrix))
		return A3D_ERROR;

	in[0]=pPt[0];
	in[1]=pPt[1];
	in[2]=pPt[2];
	in[3]=1.0;

	/* Map x and y from window coordinates */
	in[0] = (in[0] - viewport[0]) / viewport[2];
	in[1] = (in[1] - viewport[1]) / viewport[3];

	/* Map to range -1 to 1 */
	in[0] = in[0] * 2 - 1;
	in[1] = in[1] * 2 - 1;
	in[2] = in[2] * 2 - 1;

	MatrixVectMultiply(finalMatrix, in, out);
	if (out[3] == 0.0)
		return A3D_ERROR;

	out[0] /= out[3];
	out[1] /= out[3];
	out[2] /= out[3];

	*(pPtresult) = out[0];
	*(pPtresult+1) = out[1];
	*(pPtresult+2) = out[2];

	double objx, objy, objz;
	UnProject(pPt[0], pPt[1], pPt[2],
		pdModelMatrix, 
		pdProjMatrix,
		viewport,
		&objx, &objy, &objz);

	if(	fabs(pPtresult[0] - objx)> 1e-12
		|| fabs(pPtresult[1] - objx)> 1e-12
		|| fabs(pPtresult[2] - objx)> 1e-12)
		return A3D_ERROR;
	return A3D_SUCCESS;
}

A3DStatus Identity(double* pdIdentity)
{
	memset(&pdIdentity[0], 0, 16 * sizeof(double));
	pdIdentity[0] = pdIdentity[5] = pdIdentity[10] = pdIdentity[15] = 1;
	return A3D_SUCCESS;
}

A3DStatus Ortho(const double* pdCurrentMatrix,
				double dLeft,
				double dRight,
				double dTop,
				double dBottom,
				double dNear,
				double dFar,
				double* pdOutput)
{
	double adOrthoMatrix[16];
	double tx = - (dRight + dLeft)/(dRight - dLeft);
	double ty = - (dTop + dBottom)/(dTop - dBottom);
	double tz = - (dFar + dNear)/(dFar - dNear);
	
	adOrthoMatrix[0] = 2/(dRight - dLeft);
	adOrthoMatrix[1] = 0;
	adOrthoMatrix[2] = 0;
	adOrthoMatrix[3] = tx;

	adOrthoMatrix[4] = 0;
	adOrthoMatrix[5] =	2/(dTop - dBottom);
	adOrthoMatrix[6] = 0;
	adOrthoMatrix[7] = ty;
	
	adOrthoMatrix[8]  = 0;
	adOrthoMatrix[9]  = 0;
	adOrthoMatrix[10]  =  ( -2)/(dFar - dNear);
	adOrthoMatrix[11]   = tz;

	adOrthoMatrix[12] = 0;
	adOrthoMatrix[13] = 0;
	adOrthoMatrix[14] = 0;
	adOrthoMatrix[15] = 1;

	MatrixVectMultiply(	pdCurrentMatrix,adOrthoMatrix,pdOutput);
	return A3D_SUCCESS;
}

A3DVector3dData CrossProduct(const A3DVector3dData* X, const A3DVector3dData* Y)
{
	A3DVector3dData Z;
	Z.m_dX=X->m_dY*Y->m_dZ - X->m_dZ*Y->m_dY;
	Z.m_dY=X->m_dZ*Y->m_dX - X->m_dX*Y->m_dZ;
	Z.m_dZ=X->m_dX*Y->m_dY - X->m_dY*Y->m_dX;
	return Z;
}
