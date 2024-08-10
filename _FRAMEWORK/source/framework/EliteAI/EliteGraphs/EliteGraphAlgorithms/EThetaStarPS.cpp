#include "stdafx.h"
#include "EThetaStarPS.h"


using namespace Elite;


ThetaStarPS::ThetaStarPS(Graph* pGraph, Heuristic hFunction) :
	ThetaStar(pGraph, hFunction)
{

}

std::vector<GraphNode*>ThetaStarPS::FindPath(GraphNode* pStartNode, GraphNode* pGoalNode)
{
	//Calculate the path from AStar and smooth it
	std::vector<GraphNode*> finalPath{};
	std::vector<GraphNode*> path{ AStar::FindPath(pStartNode, pGoalNode) };

	//Remove all the redundant nodes from the path
	int previous{ 0 };
	int next{ 2 };
	finalPath.push_back(path[0]);

	for (size_t i{ 1 }; i < path.size() - 1; ++i)
	{
		//We only add noe to the path if there is no line of sight between its previous and its next
		if (!IsLineOfSight(path[previous], path[next]))
		{
			finalPath.push_back(path[i]);
			previous = i;
		}
		++next;
	}

	//add the destination node to the path
	finalPath.push_back(path[path.size() - 1]);
	return finalPath;
}