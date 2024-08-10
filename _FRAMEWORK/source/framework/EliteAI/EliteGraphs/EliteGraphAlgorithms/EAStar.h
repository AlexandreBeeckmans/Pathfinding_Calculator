#pragma once
#include <algorithm>
#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraph/EGraphNode.h"
#include "EHeuristic.h"

namespace Elite
{

	class AStar
	{
	public:
		AStar(Graph* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			GraphNode* pNode = nullptr;
			GraphConnection* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		virtual std::vector<GraphNode*> FindPath(GraphNode* pStartNode, GraphNode* pDestinationNode);

	private:
		
		//This function is used for the detection 'Ray' created by the IsObstacleOnTrack
		bool HasReachedEndDestination(const Vector2& currentPos, const Vector2& endPos, const Vector2& direction) const;
		
		Heuristic m_HeuristicFunction;

	protected:
		Graph* m_pGraph;
		float GetHeuristicCost(GraphNode* pStartNode, GraphNode* pEndNode) const;

		//Check if there is an obstacle on the line between the agent and the destination
		bool IsObstacleOnTrack(GraphNode* pStart, GraphNode* pEnd) const;

		void GenerateClosedList(std::list<NodeRecord>& closedList, NodeRecord& currentNodeRecord, GraphNode* pStartNode, GraphNode* pGoalNode) const;
		void virtual AddNodesToPath(std::vector<GraphNode*>& path, NodeRecord& currentNodeRecord, std::list<NodeRecord>& closedList, GraphNode* pStartNode)const;
	};
}