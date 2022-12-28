#pragma once
#include "Exam_HelperStructs.h"
// TODO: implement this class!

class Seek;
class Flee;
class Wander;
class SteeringBehaviour;
class IExamInterface;

class AgentMovement
{
public:
	explicit AgentMovement(IExamInterface* pInterface);
	~AgentMovement();
	
	//void DebugRender(); // TODO: implement!
	SteeringPlugin_Output CalculateSteering(float dt, const AgentInfo& agent);

	void SetToSeek(const Elite::Vector2& target, bool useNavMesh = true);
	void SetToFlee(const Elite::Vector2& target, bool useNavMesh = true, bool faceTarget = false);
	void SetToWander();

	//void SetTarget(const Elite::Vector2& target);
	void SetUseNavMesh(bool useNavMesh);

	//TODO: maybe use combined steering (seek + wander) while exploring map

private:
	bool m_useNavMesh;

	Elite::Vector2 m_Target;

	SteeringBehaviour* m_pCurrentSteeringBehaviour;
	Seek* m_pSeek;
	Flee* m_pFlee;
	Wander* m_pWander;
	IExamInterface* m_pInterface; // Owned by Plugin so no deletion in destructor
};

