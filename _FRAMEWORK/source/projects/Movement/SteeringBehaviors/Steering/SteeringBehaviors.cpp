//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "../Obstacle.h"
#include "framework\EliteMath\EMatrix2x3.h"
#include <limits>

using namespace Elite;

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Vector2  currentPos{ pAgent->GetPosition() };
	Vector2 targetPos{ m_Target.Position };

	Vector2 direction{ targetPos - currentPos };
	direction.Normalize();

	steering.LinearVelocity = direction * pAgent->GetMaxLinearSpeed();

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, direction, 5.0f, {0.0f,1.0f,0.0f});
	}

	return steering;
}

//Wander
//****

SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Vector2  currentPos{ pAgent->GetPosition() };
	//Vector2 targetPos{ m_WanderTarget };

	const Vector2 centerWanderCircle{ pAgent->GetDirection() * m_WanderCircleDistance + currentPos };

	const int minAngle{ static_cast<int>(m_WanderAngle - ToDegrees(m_MaxAngleChange)) };
	const int maxAngle{ static_cast<int>(m_WanderAngle + ToDegrees(m_MaxAngleChange)) };

	const int range{ maxAngle - minAngle };
	float randomAngle{ static_cast<float>(rand() % range + minAngle) };
	m_WanderAngle = randomAngle;

	const float sinAngle{ sinf(ToRadians(m_WanderAngle)) };
	const float cosAngle{ cosf(ToRadians(m_WanderAngle)) };


	const Vector2 targetPos{ cosAngle * m_WanderCircleRadius + centerWanderCircle.x , sinAngle * m_WanderCircleRadius + centerWanderCircle.y };
	m_Target = targetPos;

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawCircle(centerWanderCircle, m_WanderCircleRadius, { 0,0,1 }, DEBUGRENDERER2D->NextDepthSlice());
		DEBUGRENDERER2D->DrawPoint(m_Target.Position, 4, { 0,1,0 });
	}

	return Seek::CalculateSteering(deltaT, pAgent);
}


//Flee
//****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Vector2  currentPos{ pAgent->GetPosition() };
	Vector2 targetPos{ m_Target.Position };

	Vector2 direction{ -(targetPos - currentPos) };
	direction.Normalize();



	steering.LinearVelocity = direction * pAgent->GetMaxLinearSpeed();

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, direction, 3.0f, { 0.0f,1.0f,0.0f });
	}

	return steering;
}

//Arrive
//****
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Vector2  currentPos{ pAgent->GetPosition() };
	Vector2 targetPos{ m_Target.Position };

	Vector2 toTarget{ targetPos - currentPos };

	float speed = pAgent->GetMaxLinearSpeed();
	float distance{ toTarget.Magnitude() };
	float a{ distance - m_TargetRadius };
	float b{ m_SlowRadius - m_TargetRadius };

	if (distance < m_SlowRadius)
	{
		speed *= (a / b);
	}
	

	pAgent->SetLinearVelocity(toTarget.GetNormalized() * speed);

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawCircle(currentPos, m_SlowRadius, { 0,0,1 }, DEBUGRENDERER2D->NextDepthSlice());
		DEBUGRENDERER2D->DrawCircle(currentPos, m_TargetRadius, { 1,0,0 }, DEBUGRENDERER2D->NextDepthSlice());
	}

	return Seek::CalculateSteering(deltaT, pAgent);
}

//PURSUIT
//****
SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Vector2  currentPos{ pAgent->GetPosition() };
	Vector2 targetPos{ m_Target.Position };

	Vector2 toTarget{ targetPos - currentPos };
	float timeToTarget{ toTarget.Magnitude() / pAgent->GetMaxLinearSpeed()};

	Vector2 targetNewPos{targetPos + m_Target.LinearVelocity * timeToTarget};

	m_Target.Position = targetNewPos;

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawCircle(targetNewPos, 1, { 1,1,1 }, DEBUGRENDERER2D->NextDepthSlice());
	}


	return Seek::CalculateSteering(deltaT, pAgent);
}

//Evade
//****
SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{

	const Vector2 toTarget{ m_Target.Position - pAgent->GetPosition() };
	const float distance{ toTarget.Magnitude() };

	if (distance > m_EvadeRadius)
	{
		SteeringOutput output{};
		output.IsValid = false;
		

		if (pAgent->GetDebugRenderingEnabled())
		{
			DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), m_EvadeRadius, { 1,0,0 }, DEBUGRENDERER2D->NextDepthSlice());
		}
		return output;
	}
	

	const float time{ distance / pAgent->GetMaxLinearSpeed() };

	const Vector2 predictedPos{ m_Target.Position + m_Target.LinearVelocity * time };
	m_Target.Position = predictedPos;

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawCircle(predictedPos, 1, { 1,1,1 }, DEBUGRENDERER2D->NextDepthSlice());
	}

	return Flee::CalculateSteering(deltaT, pAgent);
}
