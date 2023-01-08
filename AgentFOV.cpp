#include "stdafx.h"
#include "AgentFOV.h"
#include "IExamInterface.h"

AgentFOV::AgentFOV(IExamInterface* pInterface)
	: m_pInterface{pInterface}
{
}

std::vector<EntityInfo> AgentFOV::GetEntitiesInFOV() const
{
	std::vector<EntityInfo> vEntitiesInFOV = {};

	EntityInfo entityInfo = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, entityInfo))
		{
			vEntitiesInFOV.push_back(entityInfo);
			continue;
		}

		break;
	}

	return vEntitiesInFOV;
}

std::vector<HouseInfo> AgentFOV::GetHousesInFOV() const
{
	std::vector<HouseInfo> vHousesInFOV = {};

	HouseInfo houseInfo = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetHouseByIndex(i, houseInfo))
		{
			vHousesInFOV.push_back(houseInfo);
			continue;
		}

		break;
	}

	return vHousesInFOV;
}