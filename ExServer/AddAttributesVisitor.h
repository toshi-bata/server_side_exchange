#pragma once
#include "visitor/VisitorTree.h"
#include <vector>

class AddAttributesVisitor :
	public A3DTreeVisitor
{
public:
	AddAttributesVisitor(A3DVisitorContainer* psContainer, const bool addEntityIds);
	~AddAttributesVisitor();

private:
	A3DTopoBodyData m_topoBodyData;

public:
	bool m_bAddEntityIds;
	int m_iPartCnt;
	int m_iBodyCnt;
	int m_iFaceCnt;
	int m_iLoopCnt;
	int m_iEdgeCnt;
	int m_iFaceId;
	int m_iEdgeId;
	std::vector<bool> m_abShellIsClosed;

	virtual A3DStatus visitEnter(const A3DPartConnector& sConnector) override;
	virtual A3DStatus visitEnter(const A3DBrepDataConnector& sConnector) override;
	virtual A3DStatus visitEnter(const A3DShellConnector& sConnector) override;
	virtual A3DStatus visitEnter(const A3DFaceConnector& sConnector) override;
	virtual A3DStatus visitEnter(const A3DLoopConnector& sConnector) override;
	virtual A3DStatus visitEnter(const A3DEdgeConnector& sConnector) override;

	virtual A3DStatus visitLeave(const A3DEdgeConnector& sConnector) override;
	virtual A3DStatus visitLeave(const A3DFaceConnector& sConnector) override;
	virtual A3DStatus visitLeave(const A3DBrepDataConnector& sConnector) override;
	virtual A3DStatus visitLeave(const A3DRiConnector& sConnector) override;
	virtual A3DStatus visitLeave(const A3DPartConnector& sConnector) override;

};

