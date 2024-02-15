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

#ifndef	 A3D_VIEW_TRAVERSE
#define	 A3D_VIEW_TRAVERSE

#include <vector>
#include "Connector.h"


class A3DVisitorContainer;

class A3DMkpViewConnector : public  A3DConnector
{
public:
	A3DMkpViewConnector(const A3DMkpView *pView) : A3DConnector(pView) 
	{
		A3D_INITIALIZE_DATA( A3DMkpViewData, m_sViewData);
		m_pView = pView;
		A3DMkpViewGet(pView,&m_sViewData);
	}

	~A3DMkpViewConnector()
	{
		A3DMkpViewGet(NULL, &m_sViewData);
	}

	A3DStatus TraverseView(A3DVisitorContainer*  pVisitor);

	A3DMkpViewData const& GetViewData() const { return m_sViewData; }
	A3DMkpView const* GetView() const { return m_pView; }

private:
	A3DMkpViewData m_sViewData;
	A3DMkpView const* m_pView;
};
#endif // A3D_VIEW_TRAVERSE