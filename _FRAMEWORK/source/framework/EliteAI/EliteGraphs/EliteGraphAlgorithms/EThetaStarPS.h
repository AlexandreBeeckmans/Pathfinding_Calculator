#pragma once
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EThetaStar.h"

namespace Elite
{
	class ThetaStarPS : public ThetaStar
	{
	public:
		ThetaStarPS(Graph* pGraph, Heuristic hFunction);
		virtual std::vector<GraphNode*> FindPath(GraphNode* pStartNode, GraphNode* pDestinationNode) override;
	};
}


