#include "stdafx.h"
#include "ExplorationMemory.h"

ExplorationMemory::ExplorationMemory(const Elite::Vector2& worldCenter, const Elite::Vector2& worldSize, float arrivalRange, int divisionsPerDim)
	: m_ArrivalRange(arrivalRange)
{
	GenerateExplorationPoints(worldCenter, worldSize, divisionsPerDim);
}

void ExplorationMemory::Update(float dt, const AgentInfo& agent)
{
	if (m_IsDoneExploring) return;

	if (ArrivedInCell(agent))
	{
		RemoveOldTargetFromVec();
		if(m_VisitingPoints.empty())
		{
			m_IsDoneExploring = true;
			return;
		}
		FindClosestUnexplored(agent);
	}
}

Elite::Vector2 ExplorationMemory::GetCurrentTarget()
{
	return m_CurrentTarget;
}

bool ExplorationMemory::IsDoneExploring()
{
	return m_IsDoneExploring;
}

const std::vector<Elite::Vector2>& ExplorationMemory::GetExplorationPoints() const
{
	return m_VisitingPoints;
}

Elite::Vector2 ExplorationMemory::FindClosestUnexplored(const AgentInfo& agent)
{
	const Elite::Vector2 tetherPoint{ agent.Position };

	auto locationDist{ [&tetherPoint](const Elite::Vector2& lhs,const Elite::Vector2& rhs) {
		const float dist1 = tetherPoint.DistanceSquared(lhs);
		const float dist2 = tetherPoint.DistanceSquared(rhs);
		return dist1 < dist2;
	} };

	const auto closestUnexploredIt{ std::min_element(begin(m_VisitingPoints), end(m_VisitingPoints), locationDist) };

	m_CurrentTarget = *closestUnexploredIt;
	return *closestUnexploredIt;
}

bool ExplorationMemory::ArrivedInCell(const AgentInfo& agent)
{
	Elite::Vector2 agentPos{ agent.Position };
	if (DistanceSquared(agentPos, m_CurrentTarget) < m_ArrivalRange * m_ArrivalRange )
	{
		return true;
	}
	return false;
}

void ExplorationMemory::RemoveOldTargetFromVec()
{
	m_VisitingPoints.erase(std::remove(begin(m_VisitingPoints), end(m_VisitingPoints), m_CurrentTarget), m_VisitingPoints.end());
	// std::cout << "Remaining points to visit: " << m_VisitingPoints.size() << "\n";
}

void ExplorationMemory::GenerateExplorationPoints(const Elite::Vector2& worldCenter, const Elite::Vector2& worldSize, int divisionsPerDim)
{
	constexpr float innerCircleRange{ 40.f };
	constexpr float middleCircleRange{ 180.f };
	constexpr float outerCircleRange{ 250.f };
	const float degreeIncrements{ (static_cast<float>(M_PI) * 2) / divisionsPerDim };
	for(int i{0}; i < divisionsPerDim; ++i)
	{
		Elite::Vector2 angleVec{cosf(degreeIncrements * i), sinf(degreeIncrements * i)};
		Elite::Vector2 phasedAngleVec{cosf(degreeIncrements * i + degreeIncrements/2), sinf(degreeIncrements * i + degreeIncrements / 2)};

		m_VisitingPoints.push_back(angleVec * innerCircleRange);
		m_VisitingPoints.push_back(phasedAngleVec * middleCircleRange);
		m_VisitingPoints.push_back(angleVec * outerCircleRange);
	}
}
