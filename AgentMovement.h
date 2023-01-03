#pragma once
#include "Exam_HelperStructs.h"

class Seek;
class Flee;
class Wander;
class SeekAndScan;
class SprintSeek;
class SprintSeekScan;
class SteeringBehaviour;
class IExamInterface;

class AgentMovement
{
public:
	explicit AgentMovement(IExamInterface* pInterface);
	~AgentMovement();

	void Update(float dt);
	//void DebugRender(); // TODO: implement!
	SteeringPlugin_Output CalculateSteering(float dt, const AgentInfo& agent);

	void SetToSeek(const Elite::Vector2& target, bool useNavMesh = true);
	//void SetToSeekAndScan(const Elite::Vector2& target, bool useNavMesh = true);
	void SetToSeekAndScan(bool useNavMesh = true);
	void SetToSSS(bool useNavMesh = true);
	void SetToSprintSeek(bool useNavMesh = true);
	void SetToSprintSeek(const Elite::Vector2& target, bool useNavMesh = true);
	void SetToFlee(const Elite::Vector2& target, bool useNavMesh = true, bool faceTarget = false);
	void SetToWander();

	//void SetTarget(const Elite::Vector2& target);
	void SetUseNavMesh(bool useNavMesh);

	bool GetShouldScan();

private:
	bool m_useNavMesh;
	float m_Timer;
	const float m_TimeToScan;
	bool m_ShouldScan;
	const float m_ScanTime;

	Elite::Vector2 m_Target;

	SteeringBehaviour* m_pCurrentSteeringBehaviour;
	Seek* m_pSeek;
	Flee* m_pFlee;
	Wander* m_pWander;
	SeekAndScan* m_pSeekAndScan;
	SprintSeek* m_pSprintSeek;
	SprintSeekScan* m_pSSS;

	IExamInterface* m_pInterface; // Owned by Plugin so no deletion in destructor
};

