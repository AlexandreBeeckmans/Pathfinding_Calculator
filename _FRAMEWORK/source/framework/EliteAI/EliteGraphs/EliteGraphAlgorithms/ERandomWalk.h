#pragma once
#include <algorithm>
#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraph/EGraphNode.h"
#include "EHeuristic.h"

#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EAStar.h"


namespace Elite
{
	class RandomWalk final : public AStar
	{
	public:
		RandomWalk(Graph* pGraph, Heuristic hFunction);
		~RandomWalk() = default;


		virtual std::vector<GraphNode*> FindPath(GraphNode* pStartNode, GraphNode* pDestinationNode) override;
	};
}


