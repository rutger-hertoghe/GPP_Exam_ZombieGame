#include "stdafx.h"
#include "AgentMemory.h"
#include "ExplorationMemory.h"
#include "HouseMemory.h"

AgentMemory::AgentMemory(ExplorationMemory* pExplorationMemory, HouseMemory* pHouseMemory)
	: m_pExplorationMemory{pExplorationMemory}
	, m_pHouseMemory{pHouseMemory}
{
}

AgentMemory::~AgentMemory()
{
	SAFE_DELETE(m_pExplorationMemory);
}

void AgentMemory::Update(float dt, const AgentInfo& agent)
{
	m_pExplorationMemory->Update(dt, agent);
	m_pHouseMemory->Update(dt);
}

ExplorationMemory* AgentMemory::GetExplorationMemory()
{
	return m_pExplorationMemory;
}

HouseMemory* AgentMemory::GetHouseMemory()
{
	return m_pHouseMemory;
}
