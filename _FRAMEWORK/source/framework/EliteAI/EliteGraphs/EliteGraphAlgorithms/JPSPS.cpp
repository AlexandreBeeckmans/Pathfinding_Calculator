#include "stdafx.h"
#include "JPSPS.h"

using namespace Elite;

JPSPS::JPSPS(TerrainGridGraph* pGraph, Heuristic hFunction) :
	JPS(pGraph, hFunction)
{

}

std::vector<GraphNode*> JPSPS::FindPath(GraphNode* pStartNode, GraphNode* pDestinationNode)
{
	//We calculate the JPS Path and then smooth it
	std::vector<GraphNode*> finalPath{};
	std::vector<GraphNode*> path{ JPS::FindPath(pStartNode, pDestinationNode) };


	int previous{ 0 };
	int next{ 2 };
	finalPath.push_back(path[0]);

	for (size_t i{ 1 }; i < path.size() - 1; ++i)
	{
		//If there is an obstacle between the previous and the next node,
		//We add the current node to the path
		if (IsObstacleOnTrack(path[previous], path[next]))
		{
			finalPath.push_back(path[i]);
			previous = i;
		}
		++next;
	}

	finalPath.push_back(path[path.size() - 1]);
	return finalPath;

	//*****************************************************************
}


