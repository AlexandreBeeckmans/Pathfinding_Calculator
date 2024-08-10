#include "stdafx.h"
#include "ENavGraph.h"
#include "../EliteGraph/EGraphNode.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraphNodeFactory/EGraphNodeFactory.h"
//#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

using namespace Elite;

NavGraph::NavGraph(const std::vector<Polygon>& colliderShapes, float widthWorld, float heightWorld, float playerRadius = 1.0f) :
	Graph(false, new GraphNodeFactoryTemplate<NavGraphNode>()),
	m_pNavMeshPolygon(nullptr)
{
	float const halfWidth = widthWorld / 2.0f;
	float const halfHeight = heightWorld / 2.0f;
	std::list<Vector2> baseBox
	{ { -halfWidth, halfHeight },{ -halfWidth, -halfHeight },{ halfWidth, -halfHeight },{ halfWidth, halfHeight } };

	m_pNavMeshPolygon = new Polygon(baseBox); // Create copy on heap

	//Store all children
	for (auto p : colliderShapes)
	{
		p.ExpandShape(playerRadius);
		m_pNavMeshPolygon->AddChild(p);
	}

	//Triangulate
	m_pNavMeshPolygon->Triangulate();

	//Create the actual graph (nodes & connections) from the navigation mesh
	CreateNavigationGraph();
}

NavGraph::NavGraph(const NavGraph& other): Graph(other)
{
}

NavGraph::~NavGraph()
{
	delete m_pNavMeshPolygon;
	m_pNavMeshPolygon = nullptr;
}

std::shared_ptr<NavGraph> NavGraph::Clone()
{
	return std::shared_ptr<NavGraph>(new NavGraph(*this));
}

int NavGraph::GetNodeIdFromLineIndex(int lineIdx) const
{

	for (GraphNode* pNode : m_pNodes)
	{
		if (reinterpret_cast<NavGraphNode*>(pNode)->GetLineIndex() == lineIdx)
		{
			return pNode->GetId();
		}
	}

	return invalid_node_id;
}

Elite::Polygon* NavGraph::GetNavMeshPolygon() const
{
	return m_pNavMeshPolygon;
}

void NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigationmesh and create a node on the center of each edge
	const std::vector<Line*>& pLines{ m_pNavMeshPolygon->GetLines() };
	for (Line* pLine : pLines)
	{
		if (m_pNavMeshPolygon->GetTrianglesFromLineIndex(pLine->index).size() == 2)
		{
			AddNode(new NavGraphNode{ pLine->index, (pLine->p1 + pLine->p2) * 0.5f });
		}
	}


	//2  Now that every node is created, connect the nodes that share the same triangle (for each triangle, ... )
	std::vector<Triangle*> pTriangles{ m_pNavMeshPolygon->GetTriangles() };
	for (Triangle* pTriangle : pTriangles)
	{
		std::vector<int> validId{};
		for (int lineIndex : pTriangle->metaData.IndexLines)
		{
			int nodeId{ GetNodeIdFromLineIndex(lineIndex) };

			//Check for invalid node Id
			if (nodeId != -1)
			{
				validId.push_back(nodeId);
			}
		}

		if (validId.size() >= 2)
		{
			AddConnection(new Elite::GraphConnection{ validId[0], validId[1] });

			if (validId.size() == 3)
			{
				AddConnection(new Elite::GraphConnection{ validId[1], validId[2] });
				AddConnection(new Elite::GraphConnection{ validId[2], validId[0] });
			}
		}
	}


	//3. Set the connections cost to the actual distance
	SetConnectionCostsToDistances();
}

