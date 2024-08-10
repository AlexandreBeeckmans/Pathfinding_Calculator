/*=============================================================================*/
// Copyright 2023-2024 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringBehaviors.h: SteeringBehaviors interface and different implementations
/*=============================================================================*/
#pragma once

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "../SteeringHelpers.h"
class SteeringAgent;
class Obstacle;

#pragma region **ISTEERINGBEHAVIOR** (BASE)
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) = 0;

	//Seek Functions
	void SetTarget(const TargetData& target) { m_Target = target; }

	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	TargetData m_Target;
};
#pragma endregion

///////////////////////////////////////
//SEEK
//****
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

///////////////////////////////////////
//WANDER
//****
class Wander : public Seek
{
public:
	Wander() = default;;
	virtual ~Wander() = default;

	//Wander Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

protected:
	const float m_WanderCircleDistance = 7.5f;
	const float m_WanderCircleRadius = 5.0f;

	float m_WanderAngle = 90.0f;
	float m_MaxAngleChange{ Elite::ToRadians(45.0f) };
};

///////////////////////////////////////
//FLEE
//****
class Flee : public ISteeringBehavior
{
public:
	Flee() = default;
	virtual ~Flee() = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

///////////////////////////////////////
//ARRIVE
//****
class Arrive : public Seek
{
public:
	Arrive() = default;
	virtual ~Arrive() = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	void SetTargetRadius(const float newRadius) { m_TargetRadius = newRadius; };
	void SetSlowRadius(const float nesRadius) { m_SlowRadius = nesRadius; };

private:
	float m_SlowRadius = 15.0f;
	float m_TargetRadius = 3.0f;
};

///////////////////////////////////////
//Pursuit
//****
class Pursuit : public Seek
{
public:
	Pursuit() = default;
	virtual ~Pursuit() = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

///////////////////////////////////////
//EVADE
//****
class Evade : public Flee
{
public:
	Evade(float evadeRadius = 15.0f) : m_EvadeRadius{ evadeRadius } {};
	virtual ~Evade() = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private:
	float m_EvadeRadius;
};



