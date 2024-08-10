#include "stdafx.h"
#include "ERandomWalk.h"

using namespace Elite;

RandomWalk::RandomWalk(Graph* pGraph, Heuristic hFunction) :
	AStar(pGraph, hFunction)
{
}


std::vector<GraphNode*> RandomWalk::FindPath(GraphNode* pStartNode, GraphNode* pDestinationNode)
{
	std::vector<GraphNode*> path{};
	GraphNode* pCurrentNode{ pStartNode };


	//We repeat this loop until we reach the destination node
	while (pCurrentNode != pDestinationNode)
	{
		//Add the current node to the path
		path.push_back(pCurrentNode);


		std::vector<GraphConnection*> currentNodeConnections{m_pGraph->GetConnectionsFromNode(pCurrentNode)};

		//Select a random neighbour of the current node
		unsigned int randomIndex{ rand() % currentNodeConnections.size() };
		GraphNode* pNextNode = { m_pGraph->GetNode(currentNodeConnections[randomIndex]->GetToNodeId())};


		//That random neighbours becomes the current node
		pCurrentNode = pNextNode;
	}
	
	path.push_back(pDestinationNode);

	return path;

}