#pragma once
#include "Exam_HelperStructs.h"

using namespace Elite;

class ExplorationMemory
{
public:
	ExplorationMemory() = delete;
	ExplorationMemory(const Elite::Vector2& worldCenter, const Elite::Vector2& worldSize, int divisionsPerDim );

	void Update(float dt, const AgentInfo& agent);
	Elite::Vector2 GetCurrentTarget();

private:
	Elite::Vector2 m_CurrentTarget;
	Elite::Vector2 m_CellDims;
	std::vector<Elite::Vector2> m_VisitingPoints;

	Elite::Vector2 FindClosestUnexplored(const AgentInfo& agent);
	bool ArrivedInCell(const AgentInfo& agent);
	void RemoveOldTargetFromVec();
};

