#pragma once
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EThetaStarPS.h"

namespace Elite
{
	class ThetaStarAnyAnglePS final : public ThetaStarPS
	{
	public:
		ThetaStarAnyAnglePS(Graph* pGraph, Heuristic hFunction);
		std::vector<GraphNode*> FindPath(GraphNode* pStartNode, GraphNode* pDestinationNode) override;
	};
}


