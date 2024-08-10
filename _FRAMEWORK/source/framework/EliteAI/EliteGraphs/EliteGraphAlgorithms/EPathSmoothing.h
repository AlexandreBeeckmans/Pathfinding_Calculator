#pragma once

#include <vector>
#include "framework/EliteGeometry/EGeometry2DTypes.h"
#include "framework/EliteAI/EliteGraphs/EliteNavGraph/ENavGraphNode.h"

namespace Elite
{
	//Portal struct (only contains line info atm, you can expand this if needed)
	struct Portal
	{
		Portal() {}
		explicit Portal(const Elite::Line& line) :
			Line(line)
		{
		}
		Elite::Line Line = {};
	};


	class SSFA final
	{
	public:
		//=== SSFA Functions ===
		//--- References ---
		//http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
		//https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work
		static std::vector<Portal> FindPortals(const std::vector<GraphNode*>& nodePath, Polygon* navMeshPolygon)
		{
			//Container
			std::vector<Portal> vPortals{};


			vPortals.push_back(Portal(Line(nodePath[0]->GetPosition(), nodePath[0]->GetPosition())));


			//For each node received, get it's corresponding line
			for (size_t i = 1; i < nodePath.size() - 1; ++i)
			{
				//Local variables
				auto pNode = static_cast<NavGraphNode*>(nodePath[i]); //Store node, except last node, because this is our target node!
				auto pLine = navMeshPolygon->GetLines()[pNode->GetLineIndex()];

				//Redetermine it's "orientation" based on the required path (left-right vs right-left) - p1 should be right point
				auto centerLine = (pLine->p1 + pLine->p2) / 2.0f;
				auto previousPosition = i == 0 ? nodePath[0]->GetPosition() : nodePath[i - 1]->GetPosition();
				auto cp = Cross((centerLine - previousPosition), (pLine->p1 - previousPosition));
				Line portalLine = {};
				if (cp > 0)//Left
					portalLine = Line(pLine->p2, pLine->p1);
				else //Right
					portalLine = Line(pLine->p1, pLine->p2);

				//Store portal
				vPortals.push_back(Portal(portalLine));
			}
			//Add degenerate portal to force end evaluation
			vPortals.push_back(Portal(Line(nodePath[nodePath.size() - 1]->GetPosition(), nodePath[nodePath.size() - 1]->GetPosition())));

			return vPortals;
		}

		static std::vector<Elite::Vector2> OptimizePortals(const std::vector<Portal>& portals)
		{
			//P1 == right point of portal, P2 == left point of portal
			std::vector<Vector2> vPath = {};
			const unsigned int amtPortals{ static_cast<unsigned int>(portals.size()) };

			int apexIdx{ 0 }, leftLegIdx{ 1 }, rightLegIdx{ 1 };

			Vector2 apexPos = portals[apexIdx].Line.p1;
			Vector2 rightLeg = portals[rightLegIdx].Line.p1 - apexPos;
			Vector2 leftLeg = portals[leftLegIdx].Line.p2 - apexPos;

			vPath.push_back(apexPos);

			for (unsigned int portalIdx{ 1 }; portalIdx < amtPortals; ++portalIdx)
			{
				//Local
				Portal currentPortal = portals[portalIdx];

				//--- RIGHT CHECK ---
				//1. See if moving funnel inwards - RIGHT
				const Vector2 newRightLeg{ currentPortal.Line.p1  - apexPos };
				if (rightLeg.Cross(newRightLeg) > 0)
				{
					//2. See if new line degenerates a line segment - RIGHT
					//not cross left leg
					if (newRightLeg.Cross(leftLeg) > 0)
					{
						rightLeg = newRightLeg;
						rightLegIdx = portalIdx;
					}
					//else we cross left leg
					else
					{
						apexPos += leftLeg;
						//apexIdx = leftLegIdx;
						portalIdx = leftLegIdx + 1;

						leftLegIdx = portalIdx;
						rightLegIdx = portalIdx;

						vPath.push_back(apexPos);

						if (portalIdx < amtPortals)
						{
							rightLeg = portals[portalIdx].Line.p1 - apexPos;
							leftLeg = portals[portalIdx].Line.p2 - apexPos;
							continue;
						}
					}
				}

					


				//--- LEFT CHECK ---
				//1. See if moving funnel inwards - LEFT
				const Vector2 newLeftLeg{ currentPortal.Line.p2 - apexPos };
				if (leftLeg.Cross(newLeftLeg) < 0)
				{
					//2. See if new line degenerates a line segment - LEFT
					//not cross right leg
					if (newLeftLeg.Cross(rightLeg) < 0)
					{
						leftLeg = newLeftLeg;
						leftLegIdx = portalIdx;
					}
					//else we cross right leg
					else
					{
						apexPos += rightLeg;
						//apexIdx = rightLegIdx;
						portalIdx = rightLegIdx + 1;

						rightLegIdx = portalIdx;
						leftLegIdx = portalIdx;

						vPath.push_back(apexPos);

						if (portalIdx < amtPortals)
						{
							leftLeg = portals[portalIdx].Line.p2 - apexPos;
							rightLeg = portals[portalIdx].Line.p1 - apexPos;
							continue;
						}
					}
				}
			}

			// Add last path point (You can use the last portal p1 or p2 points as both are equal to the endPoint of the path
			vPath.push_back(portals[amtPortals - 1].Line.p1);
			return vPath;
		}
	private:
		SSFA() {};
		~SSFA() {};
	};
}
