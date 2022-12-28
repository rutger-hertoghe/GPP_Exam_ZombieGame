#pragma once
#include "Exam_HelperStructs.h"

class ExplorationMemory;
class HouseMemory;

class AgentMemory
{
public:
	AgentMemory() = delete;
	explicit AgentMemory(ExplorationMemory* pExplorationMemory, HouseMemory* pHouseMemory);
	~AgentMemory();

	void Update(float dt, const AgentInfo& agent);
	ExplorationMemory* GetExplorationMemory();
	HouseMemory* GetHouseMemory();

private:
	ExplorationMemory* m_pExplorationMemory;
	HouseMemory* m_pHouseMemory;
};

