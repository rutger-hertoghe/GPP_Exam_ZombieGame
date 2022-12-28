#pragma once
#include "Exam_HelperStructs.h"

class IExamInterface;

class AgentFOV
{
public:
	AgentFOV() = delete;
	explicit AgentFOV(IExamInterface* pInterface);

	std::vector<EntityInfo> AgentFOV::GetEntitiesInFOV() const;
	std::vector<HouseInfo> GetHousesInFOV() const;

private:
	IExamInterface* m_pInterface; // Owned by Plugin so no deletion in destructor
};

