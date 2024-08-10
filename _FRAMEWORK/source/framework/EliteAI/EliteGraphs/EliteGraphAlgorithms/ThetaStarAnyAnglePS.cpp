#include "stdafx.h"
#include "ThetaStarAnyAnglePS.h"

using namespace Elite;

ThetaStarAnyAnglePS::ThetaStarAnyAnglePS(Graph* pGraph, Heuristic hFunction) :
	ThetaStarPS(pGraph, hFunction)
{

}

std::vector<GraphNode*>ThetaStarAnyAnglePS::FindPath(GraphNode* pStartNode, GraphNode* pGoalNode)
{
	//We calculate Theta* path, and then smooth it to make it look like JPS
	std::vector<GraphNode*> finalPath{};
	std::vector<GraphNode*> path{ ThetaStarPS::FindPath(pStartNode, pGoalNode) };


	int previous{ 0 };
	int next{ 2 };
	finalPath.push_back(path[0]);


	for (size_t i{ 1 }; i < path.size() - 1; ++i)
	{
		//Same Logic as JPSPS
		if (IsObstacleOnTrack(path[previous], path[next]))
		{
			finalPath.push_back(path[i]);
			previous = i;
		}
		++next;
	}

	finalPath.push_back(path[path.size() - 1]);
	return finalPath;
}