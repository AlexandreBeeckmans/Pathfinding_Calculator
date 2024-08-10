#pragma once

#include <algorithm>
#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraph/EGraphNode.h"
#include "EHeuristic.h"
#include "EAStar.h"


namespace Elite
{
	class ThetaStar : public AStar
	{
	public:
		ThetaStar(Graph* pGraph, Heuristic hFunction);

		virtual std::vector<GraphNode*> FindPath(GraphNode* pStartNode, GraphNode* pDestinationNode) override;

	protected:
		bool IsLineOfSight(GraphNode* pStartNode, GraphNode* pEndNode) const;

	private:
		void AddNodesToPath(std::vector<GraphNode*>& path, NodeRecord& currentNodeRecord, std::list<NodeRecord>& closedList, GraphNode* pStartNode) const override;
	};
}


