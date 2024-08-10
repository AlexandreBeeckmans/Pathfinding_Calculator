#include "stdafx.h"

#include "ENavGraphPathfinding.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EAStar.h"
#include "framework/EliteAI/EliteGraphs/EliteNavGraph/ENavGraph.h"

using namespace Elite;

std::vector<Vector2>Elite::NavMeshPathfinding::FindPath(Vector2 startPos, Vector2 endPos, NavGraph* pNavGraph)
{
	std::vector<Vector2> debugNodePositions{};
	std::vector<Portal> debugPortals{};
	return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}

std::vector<Vector2> NavMeshPathfinding::FindPath(Vector2 startPos, Vector2 endPos, NavGraph* pNavGraph, std::vector<Vector2>& debugNodePositions, std::vector<Portal>& debugPortals)
{
	//Create the path to return
	std::vector<Vector2> finalPath{};

	//Get the startTriangle and endTriangle
	const Triangle* pStartTriangle = pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(startPos);
	const Triangle* pEndTriangle = pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(endPos);


	//If we don't have a valid startTriangle or endTriangle -> return empty path
	if (pStartTriangle == nullptr || pEndTriangle == nullptr) return finalPath;
	
	//If the startTriangle and endTriangle are the same -> return straight line path
	if (pStartTriangle == pEndTriangle)
	{
		finalPath.push_back(startPos);
		finalPath.push_back(endPos);
		return finalPath;
	}
	
	//=> Start looking for a path
	//Clone the graph (returns shared_ptr!)
	const auto& cloneGraph{ pNavGraph->Clone() };

	//Create extra node for the Start Node (Agent's position) and add it to the graph.
	
	GraphNode* pStartNode{ new GraphNode(startPos) };
	cloneGraph->AddNode(pStartNode);

	//Make connections between the Start Node and the startTriangle nodes.
	for (const int indexLine : pStartTriangle->metaData.IndexLines)
	{
		const int nodeId{ cloneGraph->GetNodeIdFromLineIndex(indexLine) };

		if (nodeId == invalid_node_id)
			continue;

		cloneGraph->AddConnection(new GraphConnection(nodeId, pStartNode->GetId()));
	}

	//Create extra node for the End Node (endpos) and add it to the graph. 
	GraphNode* pEndNode{ new GraphNode( endPos) };
	cloneGraph->AddNode(pEndNode);

	//Make connections between the End Node and the endTriangle nodes.
	for (const int indexLine : pEndTriangle->metaData.IndexLines)
	{
		const int nodeId{ cloneGraph->GetNodeIdFromLineIndex(indexLine) };

		if (nodeId == invalid_node_id)
			continue;

		cloneGraph->AddConnection(new GraphConnection(nodeId, pEndNode->GetId()));
	}
	
	//Run AStar on the new graph
	AStar aStarGraph{ cloneGraph.get(), HeuristicFunctions::Manhattan};
	std::vector<GraphNode*> pathNodes = aStarGraph.FindPath(pStartNode, pEndNode);

	
	for (GraphNode* node : pathNodes)
	{
		finalPath.push_back(node->GetPosition());
	}

	//Store the path for debug rendering
	debugNodePositions.clear();
	debugNodePositions = finalPath;

	//Run optimiser on new graph, MAKE SURE the AStar path is working properly before starting the following section:
	debugPortals.clear();
	debugPortals = SSFA::FindPortals(pathNodes, pNavGraph->GetNavMeshPolygon());
	finalPath = SSFA::OptimizePortals(debugPortals);

	finalPath.push_back(endPos);
	return finalPath;
}
