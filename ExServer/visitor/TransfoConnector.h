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

#ifndef	  A3D_TRANSFO_CONNECTOR
#define	 A3D_TRANSFO_CONNECTOR
	
#include <vector>
#include "Connector.h"
#include "Matrix.h"

class A3DVisitor;


class A3DTransfoConnector	: public A3DConnector
{
	friend class A3DVisitorTransfo;
	A3DMatrix4x4 m_sWorldMatrix;
	A3DMatrix4x4 m_sLocalMatrix;

	void SetGlobalMatrix(A3DMiscTransformation const * pMiscTransformation);
	void SetGlobalMatrix(A3DMatrix4x4 const& rWorldMatrix);
	void ComputeGlobalMatrix(A3DMatrix4x4 const& rFatherWorldMatrix);

	A3DTransfoConnector(const A3DMiscTransformation* pMiscTransformation);

public :

	~A3DTransfoConnector() {}

	A3DStatus GetLocalMatrix(A3DMatrix4x4& rsMatrix);

	A3DStatus GetGlobalTransfo(A3DMatrix4x4& rsMatrix);

};

#endif