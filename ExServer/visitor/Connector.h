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

#ifndef A3D_CONNECTOR
#define A3D_CONNECTOR

#include <A3DSDKIncludes.h>
//#include <A3DInternalExports.h>
#ifndef WIN32
#define strcpy_s(dst, dst_size, src) strcpy((dst), (src))
#endif
class A3DConnector
{
protected:

	const A3DEntity* m_pEntity;

	A3DConnector(const A3DEntity* pEntity) : m_pEntity(pEntity) {}
	A3DConnector() {};
public:

	const A3DEntity* GetA3DEntity() const { return m_pEntity; }
	virtual A3DEEntityType GetType() const { return kA3DTypeUnknown; }

};

#endif