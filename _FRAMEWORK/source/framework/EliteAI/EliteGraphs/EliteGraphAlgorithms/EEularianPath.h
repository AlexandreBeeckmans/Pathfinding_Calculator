#pragma once
#include <stack>
#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraph/EGraphNode.h"
namespace Elite
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	class EulerianPath
	{
	public:

		EulerianPath(Graph* pGraph);

		Eulerianity IsEulerian() const;
		std::vector<GraphNode*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(const std::vector<GraphNode*>& pNodes, std::vector<bool>& visited, int startIndex) const;
		bool IsConnected() const;
		int ConvertIdToIndex(const std::vector<GraphNode*>& pNodes, GraphConnection* connection, int startIndex) const;

		Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* pGraph)
		: m_pGraph(pGraph)
	{
	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{

		// If the graph is not connected, there can be no Eulerian Trail
		if (!IsConnected()) return Eulerianity::notEulerian;

		// Count nodes with odd degree 
		std::vector<Elite::GraphNode*> nodes{ m_pGraph->GetAllNodes() };
		int oddCount{ 0 };

		for (Elite::GraphNode* node : nodes)
		{
			std::vector<Elite::GraphConnection*> connections = m_pGraph->GetConnectionsFromNode(node);
			int amountConnections{ int(connections.size()) };


			//Use of binary operator to find if a number is odd
			//if (amountConnections % 2 == 1)
			if (amountConnections & 1)
			{
				++oddCount;
			}
		}

		// A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		if (oddCount > 2 || oddCount == 1) return Eulerianity::notEulerian;


		// A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		// An Euler trail can be made, but only starting and ending in these 2 nodes
		if (oddCount == 2) return Eulerianity::semiEulerian;

		// A connected graph with no odd nodes is Eulerian
		return Eulerianity::eulerian;

	}

	inline std::vector<GraphNode*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		std::shared_ptr<Elite::Graph> graphCopy = m_pGraph->Clone();
		std::vector<GraphNode*> path = std::vector<GraphNode*>{};
		int nrOfNodes = graphCopy->GetAmountOfNodes();

		// Check if there can be an Euler path
		// If this graph is not eulerian, return the empty path
		if (eulerianity == Eulerianity::notEulerian) return path;

		// Find a valid starting index for the algorithm
		GraphNode* startingNode{ };
		if (eulerianity == Eulerianity::semiEulerian)
		{
			//If there're two nodes with odd degree --> take one
			for (GraphNode* node : graphCopy->GetAllNodes())
			{
				if (node != nullptr)
				{
					std::vector<Elite::GraphConnection*> connections{ graphCopy->GetConnectionsFromNode(node) };
					if (connections.size() & 1)
					{
						startingNode = node;
						break;
					}
				}
			}
		}
		else
		{
			//else choose any node
			for (GraphNode* node : graphCopy->GetAllNodes())
			{
				if (node != nullptr)
				{
					startingNode = node;
					break;
				}
			}
		}




		// Start algorithm loop
		std::stack<int> nodeStack;
		std::vector<Elite::GraphConnection*> startingNodeConnections{ graphCopy->GetConnectionsFromNode(startingNode) };
		Elite::GraphNode* currentNode{ startingNode };
		std::vector<Elite::GraphConnection*> currentConections{ graphCopy->GetConnectionsFromNode(currentNode) };
		while (graphCopy->GetConnectionsFromNode(currentNode).size() > 0 || nodeStack.size() > 0)
		{
			if (currentConections.size() > 0)
			{
				nodeStack.push(currentNode->GetId());

				const int indexToCheck{ ConvertIdToIndex(graphCopy->GetAllNodes(), currentConections[0], 0)};
				GraphNode* neighbour{ graphCopy->GetAllNodes()[currentConections[0]->GetToNodeId()] };

				graphCopy->RemoveConnection(currentConections[0]);
				currentNode = neighbour;

			}
			else
			{
				if (currentNode)
				{
					path.push_back(m_pGraph->GetAllNodes()[currentNode->GetId()]);
					currentNode = graphCopy->GetAllNodes()[nodeStack.top()];
					nodeStack.pop();
				}
			}
			currentConections = graphCopy->GetConnectionsFromNode(currentNode);
		}

		if (startingNode)
		{
			path.push_back(m_pGraph->GetAllNodes()[startingNode->GetId()]);
		}
		
		std::reverse(path.begin(), path.end()); // reverses order of the path
		return path;
	}


	inline void EulerianPath::VisitAllNodesDFS(const std::vector<GraphNode*>& pNodes, std::vector<bool>& visited, int startIndex) const
	{
		// mark the visited node
		visited[startIndex] = true;

		// recursively visit any valid connected nodes that were not visited before
		for (GraphConnection* connection : m_pGraph->GetConnectionsFromNode(pNodes[startIndex]))
		{
			//Convert the node we found to its index in the visited array
			int indiceToCheck{ ConvertIdToIndex(pNodes, connection, startIndex) };
			

			if (!visited[indiceToCheck])
			{
				VisitAllNodesDFS(pNodes, visited, indiceToCheck);
			}
		}
		
	}

	inline bool EulerianPath::IsConnected() const
	{
		//Return all valid nodes
		std::vector<Elite::GraphNode*> nodes = m_pGraph->GetAllNodes();
		

		if (nodes.size() == 0)
			return false;

		int startingNodeIndex{ 0 };
		Elite::GraphNode* pStartingNode{ nodes[startingNodeIndex]};

		// start a depth-first-search traversal from the node that has at least one connection
		std::vector<bool> visitedNodes;
		visitedNodes.resize(nodes.size());

		VisitAllNodesDFS(nodes, visitedNodes, startingNodeIndex);

		// if a node was never visited, this graph is not connected
		for (const bool isNodeVisited : visitedNodes)
		{
			if (!isNodeVisited) return false;
		}
		return true;
	}

	int EulerianPath::ConvertIdToIndex(const std::vector<GraphNode*>& pNodes, GraphConnection* connection, int startIndex) const
	{
		for (size_t i{0}; i < pNodes.size(); ++i)
		{
			if (connection->GetToNodeId() == pNodes[i]->GetId())
			{
				return i;
			}
		}

		return -1;
	}

}