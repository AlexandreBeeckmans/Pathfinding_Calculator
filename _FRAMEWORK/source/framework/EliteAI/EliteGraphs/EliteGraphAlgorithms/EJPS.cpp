#include "stdafx.h"
#include "EJPS.h"

#include <algorithm>

using namespace Elite;

JPS::JPS(TerrainGridGraph* pGraph, Heuristic hFunction) :
	AStar(pGraph, hFunction)
{
}

std::vector<GraphNode*> JPS::FindPath(GraphNode* pStartNode, GraphNode* pDestinationNode)
{
	//The path of the agent
	std::vector<GraphNode*> finalPath{};

	//All the nodes that will be considered as current node -> the open list
	std::deque<GraphNode*> pNodesToEvaluate{};

	finalPath.push_back(pStartNode);
	GraphNode* pCurrentNode{ pStartNode };

	//We loop until we find the destination
	while (pCurrentNode != pDestinationNode)
	{

		//if we can reach the final node
		if (!IsObstacleOnTrack(pCurrentNode, pDestinationNode))
		{
			//we add the current and the final node to the path and we leave the loop
			finalPath.push_back(pCurrentNode);
			finalPath.push_back(pDestinationNode);
			break;
		}

		//The size of the final path is used to know if nodes where added to it
		const unsigned int startFinalPathSize{ finalPath.size() };

		//Check all the cases reacheble in horizontal and vertical direction from the current node
		CheckHorizontal(pCurrentNode, pDestinationNode, finalPath, 1);

		//If no node were added, we check the vertical-top line
		if (startFinalPathSize == finalPath.size())
		{
			CheckVertical(pCurrentNode, pDestinationNode, finalPath, 1);

			//If no node were added we check the horizontal-left line
			if (startFinalPathSize == finalPath.size())
			{
				CheckHorizontal(pCurrentNode, pDestinationNode, finalPath, -1);

				//If no node were added we check the vertical-bottom line
				if (startFinalPathSize == finalPath.size())
				{
					CheckVertical(pCurrentNode, pDestinationNode, finalPath, -1);
				}
			}
		}
		


		//If no jumping point was added to the path, we add the diagonal neigbours in the open list.
		//They will be the new nodes to be evaluated
		if (startFinalPathSize == finalPath.size())
		{
			
			GraphNode* pTopRight{ GetNodeFromBase(pCurrentNode, 1,1) };
			if (pTopRight) pNodesToEvaluate.push_back(pTopRight);
			
			GraphNode* pTopLeft{ GetNodeFromBase(pCurrentNode, -1,1) };
			if (pTopLeft) pNodesToEvaluate.push_back(pTopLeft);

			GraphNode* pBottomRight{ GetNodeFromBase(pCurrentNode, 1,-1) };
			if (pBottomRight) pNodesToEvaluate.push_back(pBottomRight);

			GraphNode* pBottomLeft{ GetNodeFromBase(pCurrentNode, -1, -1) };
			if (pBottomLeft) pNodesToEvaluate.push_back(pBottomLeft);


			if (!pNodesToEvaluate.empty())
			{
				pCurrentNode = pNodesToEvaluate.front();
				pNodesToEvaluate.pop_front();
			}
		}
		else
		{
			//If there were a jumping point
			//--> it becomes new the new current point
			//we clear the diagonal neighbours node list, it will be refilled from that new node
			pCurrentNode = finalPath[finalPath.size() - 1];
			pNodesToEvaluate.clear();
		}
	}
	



	ConnectJumpingPoints(finalPath);
	return finalPath;
}



void JPS::CheckHorizontal(GraphNode* pBaseNode, GraphNode* pEndNode, std::vector<GraphNode*>& finalPath, const int direction) const
{
	bool hasReachedEndOfLine{ false };
	
	GraphNode* pCurrentNode{ pBaseNode };

	while (!hasReachedEndOfLine)
	{
		auto connections = m_pGraph->GetConnectionsFromNode(pCurrentNode);
		bool hasFoundNeighbour{ false };

		//We look for the direct neighbour in the correct direction
		for (GraphConnection* pConnection : connections)
		{
			GraphNode* pEvaluatedNode{ m_pGraph->GetNode(pConnection->GetToNodeId()) };
			if (pEvaluatedNode->GetPosition().x * direction > pCurrentNode->GetPosition().x * direction)
			{
				pCurrentNode = pEvaluatedNode;
				hasFoundNeighbour = true;
				break;
			}
		}

		

		if (hasFoundNeighbour)
		{
			//if we reached the end --> leave the function
			if (pCurrentNode == pEndNode)
			{
				finalPath.push_back(pEndNode);
				return;
			}

			//if there is a forced neighbour, the current node and its base parent node are added to the path
			if (HasForcedNeighboursHorizontal(pCurrentNode))
			{
				if (std::find(finalPath.cbegin(), finalPath.cend(), pCurrentNode) == finalPath.cend())
				{
					finalPath.push_back(pBaseNode);
					finalPath.push_back(pCurrentNode);
				}
				hasReachedEndOfLine = true;
			}
		}
		else
		{
			//if the current node does not have any neighbour in the correct direction
			//--> we reached the end of the line
			hasReachedEndOfLine = true;
		}
	}
}

void JPS::CheckVertical(GraphNode* pBaseNode, GraphNode* pEndNode, std::vector<GraphNode*>& finalPath, const int direction) const
{
	bool hasReachedEndOfLine{ false };
	GraphNode* pCurrentNode{ pBaseNode };

	while (!hasReachedEndOfLine)
	{
		auto connections = m_pGraph->GetConnectionsFromNode(pCurrentNode);
		bool hasFoundNeighbour{ false };

		//We search if the current node has a neighbour in the correct direction
		for (GraphConnection* pConnection : connections)
		{
			GraphNode* pEvaluatedNode{ m_pGraph->GetNode(pConnection->GetToNodeId()) };
			if (pEvaluatedNode->GetPosition().y * direction > pCurrentNode->GetPosition().y * direction)
			{
				pCurrentNode = pEvaluatedNode;
				hasFoundNeighbour = true;
				break;
			}
		}



		if (hasFoundNeighbour)
		{
			//if we reached the end --> leave the function
			if (pCurrentNode == pEndNode)
			{
				finalPath.push_back(pEndNode);
				return;
			}

			//if there is a forced neighbour, the current node and its base parent node are added to the path
			if (HasForcedNeighboursVertical(pCurrentNode))
			{
				if (std::find(finalPath.cbegin(), finalPath.cend(), pCurrentNode) == finalPath.cend())
				{
					finalPath.push_back(pCurrentNode);
				}
				hasReachedEndOfLine = true;
			}
		}
		else
		{
			hasReachedEndOfLine = true;
		}
	}
}

GraphNode* JPS::GetNodeFromBase(GraphNode* pBaseNode, const int horizontalDecal, const int verticalDecal) const
{

	TerrainGridGraph* pTerrainGraph = static_cast<TerrainGridGraph*>(m_pGraph);
	const int decaledRow{ pTerrainGraph->GetRowAndColumn(pBaseNode->GetId()).first + verticalDecal };
	const int decaledColumn{ pTerrainGraph->GetRowAndColumn(pBaseNode->GetId()).second + horizontalDecal };

	if (decaledRow < 0) return nullptr;
	if (decaledColumn < 0) return nullptr;
	if (decaledRow >= pTerrainGraph->GetRows()) return nullptr;
	if (decaledColumn >= pTerrainGraph->GetColumns()) return nullptr;

	int nextIndex = decaledColumn + ((decaledRow)*pTerrainGraph->GetColumns());

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//WARNING Have to check if this node is not a water node
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	GraphNode* decaledNode = m_pGraph->GetNode(nextIndex);

	//cast to terrain
	TerrainGraphNode* pTerrainDecaledNode{ static_cast<TerrainGraphNode*>(decaledNode) };

	if (pTerrainDecaledNode->GetTerrainType() == TerrainType::Water) return nullptr;

	return decaledNode;
	
}


bool JPS::HasForcedNeighboursHorizontal(GraphNode* pBaseNode) const
{


	if(HasObstacleOnLeft(pBaseNode)) return false;
	if (HasObstacleOnRight(pBaseNode)) return false;
	if (HasObstacleOnTop(pBaseNode)) return true;
	if (HasObstacleOnBottom(pBaseNode)) return true;

	return false;
}

bool JPS::HasForcedNeighboursVertical(GraphNode* pBaseNode) const
{

	if (HasObstacleOnTop(pBaseNode)) return false;
	if (HasObstacleOnBottom(pBaseNode)) return false;
	if (HasObstacleOnLeft(pBaseNode)) return true;
	if (HasObstacleOnRight(pBaseNode)) return true;
	

	return false;
}

bool JPS::HasObstacleOnTop(GraphNode* pBaseNode) const
{
	TerrainGridGraph* pTerrainGraph = static_cast<TerrainGridGraph*>(m_pGraph);

	const int currentRow{ pTerrainGraph->GetRowAndColumn(pBaseNode->GetId()).first };
	const int currentColumn{ pTerrainGraph->GetRowAndColumn(pBaseNode->GetId()).second };

	//if on top of the grid -->no top neighbor
	if ( currentRow >= pTerrainGraph->GetRows() - 1) return false;

	const int topNeighbourIndex = currentColumn + ((currentRow + 1) * pTerrainGraph->GetColumns());
	GraphNode* topNeighbour{ pTerrainGraph->GetNode(topNeighbourIndex) };
	TerrainGraphNode* pTerrainTopNeighbour = static_cast<TerrainGraphNode*>(topNeighbour);

	if (pTerrainTopNeighbour->GetTerrainType() == TerrainType::Water) return true;

	return false;
}

bool JPS::HasObstacleOnBottom(GraphNode* pBaseNode) const
{
	TerrainGridGraph* pTerrainGraph = static_cast<TerrainGridGraph*>(m_pGraph);

	const int currentRow{ pTerrainGraph->GetRowAndColumn(pBaseNode->GetId()).first };
	const int currentColumn{ pTerrainGraph->GetRowAndColumn(pBaseNode->GetId()).second };

	//if on bottom of the grid -->no bottom neighbor
	if (currentRow <= 0) return false;

	const int bottomNeighbourIndex = currentColumn + ((currentRow - 1) * pTerrainGraph->GetColumns());
	GraphNode* bottomNeighbour{ pTerrainGraph->GetNode(bottomNeighbourIndex) };
	TerrainGraphNode* pTerrainBottomNeighbour = static_cast<TerrainGraphNode*>(bottomNeighbour);

	//If obstacle on bottom return true 
	if (pTerrainBottomNeighbour->GetTerrainType() == TerrainType::Water) return true;

	return false;
}

bool JPS::HasObstacleOnLeft(GraphNode* pBaseNode) const
{
	TerrainGridGraph* pTerrainGraph = static_cast<TerrainGridGraph*>(m_pGraph);

	const int currentRow{ pTerrainGraph->GetRowAndColumn(pBaseNode->GetId()).first };
	const int currentColumn{ pTerrainGraph->GetRowAndColumn(pBaseNode->GetId()).second };

	//if on left of the grid -->no bottom neighbor
	if (currentColumn <= 0) return false;

	const int bottomNeighbourIndex = (currentColumn - 1) + (currentRow * pTerrainGraph->GetColumns());
	GraphNode* bottomNeighbour{ pTerrainGraph->GetNode(bottomNeighbourIndex) };
	TerrainGraphNode* pTerrainBottomNeighbour = static_cast<TerrainGraphNode*>(bottomNeighbour);

	//If obstacle on left return true 
	if (pTerrainBottomNeighbour->GetTerrainType() == TerrainType::Water) return true;

	return false;
}

bool JPS::HasObstacleOnRight(GraphNode* pBaseNode) const
{
	TerrainGridGraph* pTerrainGraph = static_cast<TerrainGridGraph*>(m_pGraph);

	const int currentRow{ pTerrainGraph->GetRowAndColumn(pBaseNode->GetId()).first };
	const int currentColumn{ pTerrainGraph->GetRowAndColumn(pBaseNode->GetId()).second };

	//if on right of the grid -->no bottom neighbor
	if (currentColumn >= pTerrainGraph->GetColumns() - 1) return false;

	const int bottomNeighbourIndex = (currentColumn + 1) + (currentRow * pTerrainGraph->GetColumns());
	GraphNode* bottomNeighbour{ pTerrainGraph->GetNode(bottomNeighbourIndex) };
	TerrainGraphNode* pTerrainBottomNeighbour = static_cast<TerrainGraphNode*>(bottomNeighbour);

	//If obstacle on right return true 
	if (pTerrainBottomNeighbour->GetTerrainType() == TerrainType::Water) return true;

	return false;
}


void JPS::ConnectJumpingPoints(std::vector<GraphNode*>& finalPath) const
{
	std::vector<GraphNode*> correctedPath{};


	for (size_t i{ 0 }; i < finalPath.size() - 1; ++i)
	{
		correctedPath.push_back(finalPath[i]);

		//If there is no obstacle between the current node and the end node
		if (!IsObstacleOnTrack(finalPath[i], finalPath[finalPath.size() - 1]))
		{
			//we can just add the end node to the path, all the other nodes are redundant.
			correctedPath.push_back(finalPath[finalPath.size() - 1]);
			finalPath.clear();
			finalPath = correctedPath;
			return;
		}
	}
}






