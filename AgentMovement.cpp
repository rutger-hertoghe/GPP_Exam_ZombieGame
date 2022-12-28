#include "stdafx.h"
#include "AgentMovement.h"
#include "SteeringBehaviours.h"
#include "IExamInterface.h"

AgentMovement::AgentMovement(IExamInterface* pInterface)
	: m_pInterface{ pInterface }
	, m_pSeek{new Seek{}}
	, m_pFlee{new Flee{}}
	, m_pWander{new Wander{}}
	, m_Target{ 0.f, 0.f }
	, m_useNavMesh{ true }
{
	m_pCurrentSteeringBehaviour = nullptr;
}

AgentMovement::~AgentMovement()
{
	SAFE_DELETE(m_pSeek);
	SAFE_DELETE(m_pFlee);
	SAFE_DELETE(m_pWander);
}

SteeringPlugin_Output AgentMovement::CalculateSteering(float dt, const AgentInfo& agent)
{
	auto steering = SteeringPlugin_Output();
	if (!m_pCurrentSteeringBehaviour) return steering;

	m_pInterface->Draw_Circle(m_Target, 2.f, { 0.5f, 0.5f, 0.f });

	// Fleeing from zombie doesn't properly work due to the nav mesh stuff
	if (m_useNavMesh)
	{
		Elite::Vector2 navMeshTarget{ m_pInterface->NavMesh_GetClosestPathPoint(m_Target) };
		m_pInterface->Draw_SolidCircle(navMeshTarget, 2.f, { 0.f, 0.f }, { 1.f, 1.f, 0.f });
		m_pCurrentSteeringBehaviour->SetTarget(navMeshTarget);
	}
	else
	{
		m_pCurrentSteeringBehaviour->SetTarget(m_Target);
	}
	steering = m_pCurrentSteeringBehaviour->CalculateSteering(dt, agent);

	return steering;
}

void AgentMovement::SetToSeek(const Elite::Vector2& target, bool useNavMesh)
{
	SetUseNavMesh(useNavMesh);
	m_Target = target;
	m_pCurrentSteeringBehaviour = m_pSeek;
}

void AgentMovement::SetToFlee(const Elite::Vector2& target, bool useNavMesh, bool faceTarget)
{
	SetUseNavMesh(useNavMesh);
	m_Target = target;
	m_pCurrentSteeringBehaviour = m_pFlee;
	m_pFlee->SetFaceTarget(faceTarget);
}

void AgentMovement::SetToWander()
{
	SetUseNavMesh(false);
	m_pCurrentSteeringBehaviour = m_pWander;
}

//void AgentMovement::SetTarget(const Elite::Vector2& target)
//{
//	m_Target = target;
//}

void AgentMovement::SetUseNavMesh(bool useNavMesh)
{
	m_useNavMesh = useNavMesh;
}
