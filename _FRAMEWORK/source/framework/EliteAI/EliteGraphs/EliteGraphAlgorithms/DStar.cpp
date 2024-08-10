#include "stdafx.h"
#include "DStar.h"



//using namespace Elite;
//DStar::DStar(Graph* pGraph, Heuristic hFunction)
//	: m_pGraph(pGraph)
//	, m_HeuristicFunction(hFunction)
//{
//}
//
//float DStar::GetHeuristicCost(GraphNode* pStartNode, GraphNode* pEndNode) const
//{
//	Vector2 toDestination = m_pGraph->GetNodePos(pEndNode->GetId()) - m_pGraph->GetNodePos(pStartNode->GetId());
//	return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
//}
//
//std::vector<GraphNode*>DStar::FindPath(GraphNode* pStartNode, GraphNode* pGoalNode)
//{
//	std::vector<GraphNode*> path{};
//	std::list<NodeRecord> openList{};
//	std::list<NodeRecord> closedList{};
//	NodeRecord currentNodeRecord{};
//
//	NodeRecord startRecord;
//	startRecord.pNode = pStartNode;
//	startRecord.pConnection = nullptr;
//	startRecord.estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode);
//
//	openList.push_back(startRecord);
//
//	while (!openList.empty())
//	{
//		currentNodeRecord = *(std::min_element(openList.begin(), openList.end()));
//		openList.remove(currentNodeRecord);
//
//		//if we reached the end, we leave
//		if (currentNodeRecord.pNode == pGoalNode) break;
//
//		for (GraphConnection* connection : m_pGraph->GetConnectionsFromNode(currentNodeRecord.pNode))
//		{
//			//Create a record for the neigbor node
//			GraphNode* pNextNode = { m_pGraph->GetNode(connection->GetToNodeId()) };
//			NodeRecord nextNodeRecord
//			{
//				pNextNode,
//				connection,
//				currentNodeRecord.costSoFar + connection->GetCost(),
//				GetHeuristicCost(pNextNode, pGoalNode)
//			};
//
//			bool isInAList{ false };
//
//			for (NodeRecord& record : closedList)
//			{
//				if (record.pNode == pNextNode)
//				{
//					if (record.costSoFar > nextNodeRecord.costSoFar)
//					{
//						record = nextNodeRecord;
//					}
//
//					isInAList = true;
//					break;
//				}
//			}
//			if (isInAList) continue;
//
//
//			for (NodeRecord record : openList)
//			{
//				if (record.pNode == pNextNode)
//				{
//					if (record.costSoFar > nextNodeRecord.costSoFar)
//					{
//						record = nextNodeRecord;
//					}
//					break;
//				}
//			}
//			if (isInAList) continue;
//
//			openList.push_back(nextNodeRecord);
//		}
//
//		for (const NodeRecord& record : openList)
//		{
//			if (record.pNode == currentNodeRecord.pNode) openList.remove(record);
//			break;
//		}
//		closedList.push_back(currentNodeRecord);
//	}
//
//
//	//Add nodes to the path
//	do
//	{
//		path.push_back(currentNodeRecord.pNode);
//		for (const NodeRecord& record : closedList)
//		{
//			Elite::GraphConnection* currentConnection{ currentNodeRecord.pConnection };
//
//			if (!currentConnection) continue;
//
//			if (record.pNode == m_pGraph->GetNode(currentConnection->GetFromNodeId()))
//			{
//				currentNodeRecord = record;
//				closedList.remove(record);
//				break;
//			}
//		}
//	} while (currentNodeRecord.pNode != pStartNode);
//
//
//	path.push_back(pStartNode);
//	std::reverse(path.begin(), path.end());
//	return path;
//
//
//}
//
//float DStar::CalculateKey(NodeRecord& currentNodeRecord, NodeRecord& startRecord) const
//{
//	const float val1 = std::min_element(currentNodeRecord.costSoFar, currentNodeRecord.estimatedTotalCost);
//
//
//	//+GetHeuristicCost(startRecord.pNode, currentNodeRecord.pNode)
//}
