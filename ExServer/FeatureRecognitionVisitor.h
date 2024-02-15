#pragma once
#include "visitor/VisitorTree.h"
#include <vector>
#include <map>

class FeatureRecognitionVisitor :
	public A3DTreeVisitor
{
public:
	FeatureRecognitionVisitor(A3DVisitorContainer* psContainer, const bool simplify, const double tol);
	~FeatureRecognitionVisitor();

private:
	const bool m_bSimplify;
	const double m_dTol;

	A3DEEntityType m_eCrrentFaceType;
	double m_dLoopRadius;
	int m_iFaceId;
	std::map<A3DTopoEdge*, int> m_mCirEdgeCylFace;

	struct BodyRoLoops {
		int bodyId;
		std::vector<A3DTopoLoop*> vRoundLoops;
		std::vector<float> vRadius;
	};
	std::vector<BodyRoLoops> m_vBodyRoLoops;
	int m_iCurrentBodyId;


public:
	virtual A3DStatus visitEnter(const A3DRiConnector& sConnector) override;
	virtual A3DStatus visitEnter(const A3DFaceConnector& sConnector) override;
	virtual A3DStatus visitEnter(const A3DLoopConnector& sConnector) override;
	virtual A3DStatus visitEnter(const A3DCoEdgeConnector& sConnector) override;

	virtual A3DStatus visitLeave(const A3DLoopConnector& sConnector) override;
	virtual A3DStatus visitLeave(const A3DFaceConnector& sConnector) override;

	std::vector<float> getRoundHoleFaces();
};

