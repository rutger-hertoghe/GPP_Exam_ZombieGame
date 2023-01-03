#pragma once
#include "Exam_HelperStructs.h"

class SteeringBehaviour
{
public:
	SteeringBehaviour() = default;
	virtual ~SteeringBehaviour() = default;
	
	void SetTarget(const Elite::Vector2& target);
	virtual SteeringPlugin_Output CalculateSteering(float deltaTime, const AgentInfo& agent) = 0;
	
	const Elite::Vector2& GetTarget();
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

class SeekAndScan : public Seek
{
public:
	SeekAndScan() = default;
	virtual ~SeekAndScan() = default;
	virtual SteeringPlugin_Output CalculateSteering(float deltaTime, const AgentInfo& agent) override;
};

class SprintSeek : public Seek
{
public:
	SprintSeek() = default;
	virtual ~SprintSeek() = default;
	virtual SteeringPlugin_Output CalculateSteering(float deltaTime, const AgentInfo& agent) override;
};

class SprintSeekScan : public SeekAndScan
{
public:
	SprintSeekScan() = default;
	virtual ~SprintSeekScan() = default;
	virtual SteeringPlugin_Output CalculateSteering(float deltaTime, const AgentInfo& agent) override;
};

class Flee : public SteeringBehaviour
{
public:
	Flee() = default;
	virtual ~Flee() = default;
	
	virtual SteeringPlugin_Output CalculateSteering(float deltaTime, const AgentInfo& agent) override;
	void SetFaceTarget(bool faceTarget);

private:
	bool m_FaceTarget{ false };
};

class Wander : public Seek
{
public:
	Wander() = default;
	virtual ~Wander() = default;

	virtual SteeringPlugin_Output CalculateSteering(float deltaTime, const AgentInfo& agent) override;

private:
	float m_WanderAngle{};
	float m_MaxAngleChange{ 0.3f };
	float m_Offset{ 10.f };
	float m_Radius{ 5.f };

	float Wander::GenerateRandomAngle(float precisionScaling) const;
};


