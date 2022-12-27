#pragma once
#include "Exam_HelperStructs.h"

class SteeringBehaviour
{
public:
	SteeringBehaviour() = default;
	virtual ~SteeringBehaviour() = default;
	
	void SetTarget(const Elite::Vector2& target);
	virtual SteeringPlugin_Output CalculateSteering(float deltaTime, const AgentInfo& agent) = 0;
	
protected:
	Elite::Vector2 m_Target;
};

class Seek : public SteeringBehaviour
{
public:
	Seek() = default;
	virtual ~Seek() = default;
	virtual SteeringPlugin_Output CalculateSteering(float deltaTime, const AgentInfo& agent) override;
};

class Flee : public SteeringBehaviour
{
public:
	Flee() = default;
	virtual ~Flee() = default;
	
	virtual SteeringPlugin_Output CalculateSteering(float deltaTime, const AgentInfo& agent) override;
};

//class Explore : public SteeringBehaviour
//{
//public:
//	Explore() = default;
//	virtual ~Explore() = default;
//
//	virtual SteeringPlugin_Output CalculateSteering(float deltaTime, const AgentInfo& agent) override;
//
//private:
//	std::vector<Elite::Vector2> exploredLocations;
//	float explorationGridSize{ 50.f };
//
//
//};

