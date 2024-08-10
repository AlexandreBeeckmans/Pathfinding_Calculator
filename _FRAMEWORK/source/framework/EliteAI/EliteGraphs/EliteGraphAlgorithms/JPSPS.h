#pragma once
#include <algorithm>
#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraph/EGraphNode.h"
#include "framework/EliteAI/EliteGraphs/EliteTerrainGridGraph/ETerrainGridGraph.h"
#include "framework/EliteAI/EliteGraphs/EliteTerrainGridGraph/ETerrainGraphNode.h"
#include "EHeuristic.h"

#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EJPS.h"

namespace Elite
{

	class JPSPS final : public JPS
	{
	public:
		JPSPS(TerrainGridGraph* pGraph, Heuristic hFunction);

		virtual std::vector<GraphNode*> FindPath(GraphNode* pStartNode, GraphNode* pDestinationNode) override;
	};
}
