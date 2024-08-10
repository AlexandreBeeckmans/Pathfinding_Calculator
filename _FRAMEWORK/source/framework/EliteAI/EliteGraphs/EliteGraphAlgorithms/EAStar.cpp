#include "stdafx.h"
#include "EAStar.h"
#include <framework/EliteAI/EliteGraphs/EliteTerrainGridGraph/ETerrainGraphNode.h>

using namespace Elite;
AStar::AStar(Graph* pGraph, Heuristic hFunction)
	: m_pGraph(pGraph)
	, m_HeuristicFunction(hFunction)
{
}

std::vector<GraphNode*>AStar::FindPath(GraphNode* pStartNode, GraphNode* pGoalNode)
{
	std::vector<GraphNode*> path{};
	std::list<NodeRecord> closedList{};
	NodeRecord currentNodeRecord{};

	
	GenerateClosedList(closedList, currentNodeRecord, pStartNode, pGoalNode);

	//Add nodes to the path
	AddNodesToPath(path, currentNodeRecord, closedList, pStartNode);

	return path;
}


float AStar::GetHeuristicCost(GraphNode* pStartNode, GraphNode* pEndNode) const
{
	Vector2 toDestination = m_pGraph->GetNodePos(pEndNode->GetId()) - m_pGraph->GetNodePos(pStartNode->GetId());
	return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
}

bool AStar::IsObstacleOnTrack(GraphNode* pStart, GraphNode* pEnd) const
{
	const Elite::Vector2 direction{ (pEnd->GetPosition() - pStart->GetPosition()).GetNormalized() };

	Elite::Vector2 currentPositon{ pStart->GetPosition() };

	//If we dindt reach the end of the ray between start and end
	while (!HasReachedEndDestination(currentPositon, pEnd->GetPosition(), direction))
	{

		//We analyze different position on the ray to know if it is a wall
		const float detectionPrecision{ 155 };
		currentPositon += direction / detectionPrecision;


		//If we leave the graph it is considered as an obstacle
		TerrainGraphNode* pCurrentNode{ static_cast<TerrainGraphNode*>(m_pGraph->GetNodeAtPosition(currentPositon)) };
		if (pCurrentNode == nullptr)
		{
			return true;
		}

		//if there is water on the ray --> there is an obstacle on Track
		if (pCurrentNode->GetTerrainType() == TerrainType::Water)
		{
			return true;
		}
	}

	return false;

}

bool AStar::HasReachedEndDestination(const Vector2& currentPos, const Vector2& endPos, const Vector2& direction) const
{
	//Top Right direction
	if (direction.x >= 0 && direction.y >= 0 && currentPos.x >= endPos.x && currentPos.y >= endPos.y) return true;

	//Top Left direction
	if (direction.x <= 0 && direction.y >= 0 && currentPos.x <= endPos.x && currentPos.y >= endPos.y) return true;

	//Bottom Right direction
	if (direction.x >= 0 && direction.y <= 0 && currentPos.x >= endPos.x && currentPos.y <= endPos.y) return true;

	//Bottom Left Direction
	if (direction.x <= 0 && direction.y <= 0 && currentPos.x <= endPos.x && currentPos.y <= endPos.y) return true;

	return false;
}

void AStar::GenerateClosedList(std::list<NodeRecord>& closedList, NodeRecord& currentNodeRecord, GraphNode* pStartNode, GraphNode* pGoalNode) const
{
	std::list<NodeRecord> openList{};

	NodeRecord startRecord;
	startRecord.pNode = pStartNode;
	startRecord.pConnection = nullptr;
	startRecord.estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode);

	openList.push_back(startRecord);

	while (!openList.empty())
	{
		currentNodeRecord = *(std::min_element(openList.begin(), openList.end()));
		openList.remove(currentNodeRecord);

		//if we reached the end, we leave
		if (currentNodeRecord.pNode == pGoalNode) break;

		for (GraphConnection* connection : m_pGraph->GetConnectionsFromNode(currentNodeRecord.pNode))
		{
			//Create a record for the neigbor node
			GraphNode* pNextNode = { m_pGraph->GetNode(connection->GetToNodeId()) };
			NodeRecord nextNodeRecord
			{
				pNextNode,
				connection,
				currentNodeRecord.costSoFar + connection->GetCost(),
				GetHeuristicCost(pNextNode, pGoalNode)
			};

			bool isInAList{ false };

			for (NodeRecord& record : closedList)
			{
				if (record.pNode == pNextNode)
				{
					if (record.costSoFar > nextNodeRecord.costSoFar)
					{
						record = nextNodeRecord;
					}

					isInAList = true;
					break;
				}
			}
			if (isInAList) continue;


			for (NodeRecord record : openList)
			{
				if (record.pNode == pNextNode)
				{
					if (record.costSoFar > nextNodeRecord.costSoFar)
					{
						record = nextNodeRecord;
					}
					break;
				}
			}
			if (isInAList) continue;

			openList.push_back(nextNodeRecord);
		}

		for (const NodeRecord& record : openList)
		{
			if (record.pNode == currentNodeRecord.pNode) openList.remove(record);
			break;
		}
		closedList.push_back(currentNodeRecord);
	}
}

void AStar::AddNodesToPath(std::vector<GraphNode*>& path, NodeRecord& currentNodeRecord, std::list<NodeRecord>& closedList, GraphNode* pStartNode)const
{
	do
	{
		path.push_back(currentNodeRecord.pNode);
		for (const NodeRecord& record : closedList)
		{
			Elite::GraphConnection* currentConnection{ currentNodeRecord.pConnection };

			if (!currentConnection) continue;

			if (record.pNode == m_pGraph->GetNode(currentConnection->GetFromNodeId()))
			{
				currentNodeRecord = record;
				closedList.remove(record);
				break;
			}
		}
	} while (currentNodeRecord.pNode != pStartNode);


	path.push_back(pStartNode);
	std::reverse(path.begin(), path.end());
}