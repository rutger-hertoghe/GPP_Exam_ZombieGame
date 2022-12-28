#include "stdafx.h"
#include "ExplorationMemory.h"

ExplorationMemory::ExplorationMemory(const Elite::Vector2& worldCenter, const Elite::Vector2& worldSize, int divisionsPerDim)
{
	m_CellDims = {worldSize.x / divisionsPerDim,  worldSize.y / divisionsPerDim };
	Elite::Vector2 worldOrigin{ worldCenter.x - worldSize.x, worldCenter.y - worldSize.y };

	for (int x{ 0 }; x < divisionsPerDim; ++x)
	{
		for (int y{ 0 }; y < divisionsPerDim; ++y)
		{
			Elite::Vector2 cellMid{ worldSize.x / 2 - x * m_CellDims.x + m_CellDims.x / 2, worldSize.y / 2 - y * m_CellDims.y + m_CellDims.y };
			m_VisitingPoints.push_back(cellMid);
		}
	}
}

void ExplorationMemory::Update(float dt, const AgentInfo& agent)
{
	// TODO: Add that when not exploring, passed through cells are also removed from m_VisitingPoints
	// Might be expensive tho? (Loop through all the cells)
	// Maybe different data structure is more opportune here?
	if (ArrivedInCell(agent))
	{
		RemoveOldTargetFromVec();
		FindClosestUnexplored(agent);
	}
}

Elite::Vector2 ExplorationMemory::GetCurrentTarget()
{
	return m_CurrentTarget;
}

// TODO: remove commented console logging code

Elite::Vector2 ExplorationMemory::FindClosestUnexplored(const AgentInfo& agent)
{
	const Elite::Vector2 agentPos{ agent.Position };

	auto locationDist{ [&agentPos](const Elite::Vector2& lhs,const Elite::Vector2& rhs) {
		float dist1 = agentPos.DistanceSquared(lhs);
		float dist2 = agentPos.DistanceSquared(rhs);
		return dist1 < dist2;
	} };

	auto closestUnexploredIt{ std::min_element(begin(m_VisitingPoints), end(m_VisitingPoints), locationDist) };

	m_CurrentTarget = *closestUnexploredIt;
	std::cout << "Closest point: " << m_CurrentTarget.x << ", " << m_CurrentTarget.y << ";\n";
	return *closestUnexploredIt;
}

bool ExplorationMemory::ArrivedInCell(const AgentInfo& agent)
{
	// TODO: Link arrival to FOV cone instead of being inside cell
	Elite::Vector2 agentPos{ agent.Position };
	if (abs(agentPos.x - m_CurrentTarget.x) < m_CellDims.x / 2 && abs(agentPos.y - m_CurrentTarget.y) < m_CellDims.y /2)
	{
		std::cout << "ARRIVED IN CELL!\n";
		return true;
	}
	return false;
}

void ExplorationMemory::RemoveOldTargetFromVec()
{
	m_VisitingPoints.erase(std::remove(begin(m_VisitingPoints), end(m_VisitingPoints), m_CurrentTarget), m_VisitingPoints.end());
	std::cout << "Remaining points to visit: " << m_VisitingPoints.size() << "\n";
}
