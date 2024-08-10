#pragma once
#include <algorithm>
#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraph/EGraphNode.h"
#include "framework/EliteAI/EliteGraphs/EliteTerrainGridGraph/ETerrainGridGraph.h"
#include "framework/EliteAI/EliteGraphs/EliteTerrainGridGraph/ETerrainGraphNode.h"
#include "EHeuristic.h"

#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EThetaStar.h"

namespace Elite
{

	class JPS : public AStar
	{
	public:
		JPS(TerrainGridGraph* pGraph, Heuristic hFunction);

		virtual std::vector<GraphNode*> FindPath(GraphNode* pStartNode, GraphNode* pDestinationNode) override;



	private:

		void CheckHorizontal(GraphNode* pBaseNode, GraphNode* pEndNode, std::vector<GraphNode*>& finalPath, const int direction) const;
		void CheckVertical(GraphNode* pBaseNode, GraphNode* pEndNode, std::vector<GraphNode*>& finalPath, const int direction) const;


		GraphNode* GetNodeFromBase(GraphNode* pBaseNode, const int horizontalDecal, const int verticalDecal) const;

		bool HasForcedNeighboursHorizontal(GraphNode* pBaseNode) const;
		bool HasForcedNeighboursVertical(GraphNode* pBaseNode) const;



		bool HasObstacleOnTop(GraphNode* pBaseNode) const;
		bool HasObstacleOnBottom(GraphNode* pBaseNode) const;

		bool HasObstacleOnLeft(GraphNode* pBaseNode) const;
		bool HasObstacleOnRight(GraphNode* pBaseNode) const;


		void ConnectJumpingPoints(std::vector<GraphNode*>& finalPath) const;


	};
}

