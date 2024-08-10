#include "stdafx.h"

#include "EInfluenceMap.h"

using namespace Elite;

InfluenceMap::InfluenceMap(int columns, int rows, float cellSize) :
	m_numColumns{ columns },
	m_numRows{ rows },
	m_cellSize{ cellSize }
{
	m_InfluenceBackBuffer.resize(columns * rows, 0.f);
	m_InfluenceFrontBuffer.resize(columns * rows, 0.f);
}

InfluenceMap::~InfluenceMap()
{
}

void InfluenceMap::SetInfluenceAtPosition(Elite::Vector2& pos, float influence, bool additive)
{
	auto idx = GetIndexAtPosition(pos);
	if (additive)
	{
		m_InfluenceFrontBuffer[idx] += influence;
	}
	else
	{
		m_InfluenceFrontBuffer[idx] = influence;
	}
}

float InfluenceMap::GetInfluenceAtPosition(Elite::Vector2& pos) const
{
	if (!IsInBounds(pos))return -1.f;

	auto idx = GetIndexAtPosition(pos);
	return m_InfluenceFrontBuffer[idx];
}


void InfluenceMap::Update(float deltaTime)
{
	//Propagation interval
	m_TimeSinceLastPropagation += deltaTime;
	if (m_TimeSinceLastPropagation < m_PropagationInterval)
		return;
	m_TimeSinceLastPropagation = 0.f;

	//Frontbuffer and Backbuffer have same size
	//Always read from frontbuffer and write to backbuffer!!

	//--> m_InfluenceBackBuffer;
	//--> m_InfluenceFrontBuffer

	//Index based for-loop over all cells
	//for each idx, calculate the maxInfluence of the neighbors (3x3 grid around cell)
	for (int index{ 0 }; index < m_numColumns * m_numRows; ++index)
	{
		float maxInfluence{};
		//	- Get neighboring indices (TIP: use GetNeighboringIndices method)
		//  - For each neighbor idx
		for (const int neighborIdx : GetNeighboringIndices(index))
		{
			//- if index == -1 => invalid index (edge of map)
			if (neighborIdx == -1) return;
				
			//- sample the influence on that index (frontbuffer)
			const float sampledInfluence{ m_InfluenceFrontBuffer[neighborIdx] };


			//- calculate the cost to that neighbor (m_influenceCosts array)
			const float influenceCost{ m_influenceCosts[neighborIdx]};
			
			//- calculate the influence of that neighbor:
			float neighborInfluence = sampledInfluence * expf(-influenceCost * m_Decay);

			//- keep the maxInfluence (based on highest absolute value!!) 
			if (neighborInfluence > maxInfluence)
			{
				maxInfluence = neighborInfluence;
			}

		}
		//- maxInfluence is interpolated with current influence based on momentum:
		float newInfluence = Lerp(maxInfluence, m_InfluenceFrontBuffer[index], m_Momentum);

		// - write newInfluence to the BackBuffer
		m_InfluenceBackBuffer[index] = newInfluence;
	}
	SwapBuffers();
}

Color InfluenceMap::GetNodeColorFromInfluence(float influence) const
{
	const float half = .5f;

	Color nodeColor{};
	float relativeInfluence = abs(influence) / m_MaxAbsInfluence;

	if (influence < 0)
	{
		nodeColor = Elite::Color{
		Lerp(m_NeutralColor.r, m_NegativeColor.r, relativeInfluence),
		Lerp(m_NeutralColor.g, m_NegativeColor.g, relativeInfluence),
		Lerp(m_NeutralColor.b, m_NegativeColor.b, relativeInfluence)
		};
	}
	else
	{
		nodeColor = Elite::Color{
		Lerp(m_NeutralColor.r, m_PositiveColor.r, relativeInfluence),
		Lerp(m_NeutralColor.g, m_PositiveColor.g, relativeInfluence),
		Lerp(m_NeutralColor.b, m_PositiveColor.b, relativeInfluence)
		};
	}
	return nodeColor;
}
bool InfluenceMap::IsInBounds(Vector2& position) const
{
	int r, c;

	c = int(floorf(position.x / m_cellSize));
	r = int(floorf(position.y / m_cellSize));
	return c >= 0 && c < m_numColumns && r >= 0 && r < m_numRows;
}
int InfluenceMap::GetIndexAtPosition(Vector2& position) const
{
	int r, c;

	c = int(floorf(position.x / m_cellSize));
	r = int(floorf(position.y / m_cellSize));

	c = Elite::Clamp(c, 0, m_numColumns - 1);
	r = Elite::Clamp(r, 0, m_numRows - 1);

	return r * m_numColumns + c;
}

std::vector<int> Elite::InfluenceMap::GetNeighboringIndices(int nodeIdx) const
{
	std::vector<int> indices{};
	indices.resize(9);

	int row = nodeIdx / m_numColumns;
	int col = nodeIdx % m_numColumns;

	int idx = 0;
	for (int r = -1; r < 2; ++r)
	{
		int currRow = row + r;
		if (currRow < 0 || currRow >= m_numRows)
		{
			indices[idx++] = -1;
			indices[idx++] = -1;
			indices[idx++] = -1;
			continue;
		}

		for (int c = -1; c < 2; ++c)
		{
			int currCol = col + c;

			if (c == 0 && r == 0)
			{
				indices[idx++] = -1;
				continue;
			}

			if (currCol < 0 || currCol >= m_numColumns)
				indices[idx++] = -1;
			else
				indices[idx++] = currRow * m_numColumns + currCol;
		}
	}
	return indices;
}
void InfluenceMap::Render() const
{
	float depth = DEBUGRENDERER2D->NextDepthSlice();

	float halfSize = m_cellSize * .5f;
	int idx;
	Color col;
	Vector2 pos;
	Vector2 verts[4];
	for (int r = 0; r < m_numRows; ++r)
	{
		pos.y = r * m_cellSize;
		for (int c = 0; c < m_numColumns; ++c)
		{
			idx = r * m_numColumns + c;
			col = GetNodeColorFromInfluence(m_InfluenceFrontBuffer[idx]);
			pos.x = c * m_cellSize;

			verts[0] = Vector2(pos.x, pos.y);
			verts[1] = Vector2(pos.x, pos.y + m_cellSize);
			verts[2] = Vector2(pos.x + m_cellSize, pos.y + m_cellSize);
			verts[3] = Vector2(pos.x + m_cellSize, pos.y);
			DEBUGRENDERER2D->DrawSolidPolygon(&verts[0], 4, col, depth);
			//DEBUGRENDERER2D->DrawPolygon(&verts[0], 4, { .2f,.2f,.2f }, depth);

		};

	}
}

void InfluenceMap::SwapBuffers()
{
	m_InfluenceBackBuffer.swap(m_InfluenceFrontBuffer);
}