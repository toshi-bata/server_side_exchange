#pragma once
#include "visitor/VisitorTree.h"
#include <vector>
#include <string>

class ClassifyEntityVisitor :
	public A3DTreeVisitor
{
public:
	ClassifyEntityVisitor(A3DVisitorContainer* psContainer, const std::string entityType, const bool simplify, const double tol, const int bodyId, const int entityId);
	~ClassifyEntityVisitor();

private:
	const std::string m_sEntityType;
	const bool m_bSimplify;
	const double m_dTol;
	const int m_iTargetBodtId;
	const int m_iTargetEntityId;
	int m_iEntitySizePnt;
	bool m_bIsTargetBody = false;
	int m_iFaceId;
	int m_iEdgeId;

public:
	std::vector<float> m_vEntityType;

	virtual A3DStatus visitEnter(const A3DRiConnector& sConnector) override;
	virtual A3DStatus visitEnter(const A3DFaceConnector& sConnector) override;
	virtual A3DStatus visitEnter(const A3DEdgeConnector& sConnector) override;

	virtual A3DStatus visitLeave(const A3DEdgeConnector& sConnector) override;
	virtual A3DStatus visitLeave(const A3DFaceConnector& sConnector) override;

};

