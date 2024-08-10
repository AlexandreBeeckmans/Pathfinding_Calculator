#include "stdafx.h"
#include "EThetaStar.h"

#include <algorithm>

using namespace Elite;

ThetaStar::ThetaStar(Graph* pGraph, Heuristic hFunction) :
	AStar(pGraph, hFunction)
{
}


std::vector<GraphNode*>ThetaStar::FindPath(GraphNode* pStartNode, GraphNode* pGoalNode)
{

	std::vector<GraphNode*> path{};
	std::list<NodeRecord> closedList{};
	NodeRecord currentNodeRecord{};


	//GenerateClosedList
	GenerateClosedList(closedList, currentNodeRecord, pStartNode, pGoalNode);

	//Add nodes to the path
	AddNodesToPath(path, currentNodeRecord, closedList, pStartNode);
	return path;
}

bool ThetaStar::IsLineOfSight(GraphNode* pStartNode, GraphNode* pEndNode) const
{
	if (!pStartNode || !pEndNode) return false;

	//There is a line of sight between two nodes if they are on the same row or column
	if (pStartNode->GetPosition().x == pEndNode->GetPosition().x || pStartNode->GetPosition().y == pEndNode->GetPosition().y)
	{
		return true;
	}
	return false;
}

void ThetaStar::AddNodesToPath(std::vector<GraphNode*>& path, NodeRecord& currentNodeRecord, std::list<NodeRecord>& closedList, GraphNode* pStartNode) const
{
	do
	{

		//If the path contains at least two elements and there is a line of sight between the current element and
		//the grand-parent element
		if (path.size() > 1 && IsLineOfSight(path[path.size() - 2], currentNodeRecord.pNode))
		{
			//we can eliminate the parent element
			path.pop_back();
		}

		//same as A*
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



	if (IsLineOfSight(pStartNode, path[path.size() - 2])) path.pop_back();

	path.push_back(pStartNode);
	std::reverse(path.begin(), path.end());
}