#pragma once
#include "EBehaviorTree.h"
#include "Exam_HelperStructs.h"
#include "IExamInterface.h"
#include "ExplorationMemory.h"
#include "AgentMemory.h"
#include "AgentFOV.h"
#include "AgentMovement.h"
#include "HouseMemory.h"
#include "AgentInventory.h"

using namespace Elite;

#define BS_RETRIEVE_AND_VALIDATE(name, data) isValid = pBlackboard->GetData(name, data); if(!isValid){std::cout << VAR_NAME(data) << " in " << __func__ << " was invalid!\n"; return BehaviorState::Failure;}
#define BC_RETRIEVE_AND_VALIDATE(name, data) isValid = pBlackboard->GetData(name, data); if(!isValid){std::cout << VAR_NAME(data) << " in " << __func__ << " was invalid!\n"; return false;}

namespace BT_Actions
{
	inline BehaviorState Explore(Blackboard* pBlackboard)
	{
		bool isValid{};
		
		AgentMovement* pMovement;
		BS_RETRIEVE_AND_VALIDATE("AgentMovement", pMovement);

		AgentMemory* pAgentMemory;
		BS_RETRIEVE_AND_VALIDATE("AgentMemory", pAgentMemory);

		pMovement->SetToSeek(pAgentMemory->GetExplorationMemory()->GetCurrentTarget(), true);

		return BehaviorState::Success;
	}

	inline BehaviorState FleeFacingTarget(Blackboard* pBlackboard)
	{
		bool isValid{};

		EntityInfo EnemyInfo;
		BS_RETRIEVE_AND_VALIDATE("Enemy", EnemyInfo);

		AgentMovement* pMovement;
		BS_RETRIEVE_AND_VALIDATE("AgentMovement", pMovement);

		pMovement->SetToFlee(EnemyInfo.Location, false, true);
		return BehaviorState::Success;
	}

	inline BehaviorState GetUnvisitedHouseInFOV(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentFOV* pAgentFOV;
		BS_RETRIEVE_AND_VALIDATE("AgentFOV", pAgentFOV);

		IExamInterface* pInterface;
		BS_RETRIEVE_AND_VALIDATE("Interface", pInterface);

		AgentMemory* pAgentMemory;
		BS_RETRIEVE_AND_VALIDATE("AgentMemory", pAgentMemory);

		HouseMemory* pHouseMemory{ pAgentMemory->GetHouseMemory()};
		// Get all houses in FOV
		const std::vector<HouseInfo> houseVec{ pAgentFOV->GetHousesInFOV() };

		// If no houses in FOV, return failure state
		if (houseVec.empty())
		{
			return BehaviorState::Success;
		}

		HouseInfo foundHouse;
		bool foundCandidate{ false };

		//Start looping through all found houses
		for (auto& house : houseVec)
		{
			if(pHouseMemory->IsHouseInMemory(house))
			{
				if(pHouseMemory->GetRememberedHouse(house).visited == false)
				{
					foundHouse = house;
					foundCandidate = true;
				}
				continue;
			}

			pHouseMemory->AddHouse(house);
			foundHouse = house;
			foundCandidate = true;
			break;
		}

		if(foundCandidate)
		{
			pBlackboard->ChangeData("House", foundHouse);
		}
		return BehaviorState::Success;
	}

	inline BehaviorState EnterHouse(Blackboard* pBlackboard)
	{
		bool isValid{};
		
		HouseInfo houseInfo;
		BS_RETRIEVE_AND_VALIDATE("House", houseInfo);

		AgentMovement* pMovement;
		BS_RETRIEVE_AND_VALIDATE("AgentMovement", pMovement);

		IExamInterface* pInterface;
		BS_RETRIEVE_AND_VALIDATE("Interface", pInterface);

		AgentMemory* pAgentMemory;
		BS_RETRIEVE_AND_VALIDATE("AgentMemory", pAgentMemory);

		HouseMemory* pHouseMemory{ pAgentMemory->GetHouseMemory() };

		RememberedHouse& remHouse{ pHouseMemory->GetRememberedHouseREF(houseInfo) };
		auto agentPos{ pInterface->Agent_GetInfo().Position };

		if(remHouse.m_WayPoints.empty())
		{
			pHouseMemory->SetHouseToVisited(houseInfo);
		}
		else
		{
			pMovement->SetToSeek(remHouse.m_WayPoints.back());
			if (DistanceSquared(agentPos, remHouse.m_WayPoints.back()) < 4.f)
			{
				remHouse.m_WayPoints.pop_back();
			}
		}

		return BehaviorState::Success;
	}

	inline BehaviorState UsePistol(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentInventory* pInventory;
		BS_RETRIEVE_AND_VALIDATE("Inventory", pInventory);

		if (!pInventory->HasItem(eItemType::PISTOL))
		{
			return BehaviorState::Failure;
		}

		if (!pInventory->UsePistol())
		{
			return BehaviorState::Failure;
		}
		return BehaviorState::Success;
	}

	inline BehaviorState UseShotgun(Blackboard* pBlackboard)
	{
		bool isValid{};
		AgentInventory* pInventory;
		BS_RETRIEVE_AND_VALIDATE("Inventory", pInventory);

		if (!pInventory->HasItem(eItemType::SHOTGUN))
		{
			return BehaviorState::Failure;
		}

		if (!pInventory->UseShotgun())
		{
			return BehaviorState::Failure;
		}
		return BehaviorState::Success;
	}

	/*inline BehaviorState SetToWander(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentMovement* pMovement;
		BS_RETRIEVE_AND_VALIDATE("AgentMovement", pMovement);

		pMovement->SetToWander();

		return BehaviorState::Success;
	}*/

	// ITEM GRABBING
	inline BehaviorState GetClosestItemInFOV(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentFOV* pAgentFOV;
		BS_RETRIEVE_AND_VALIDATE("AgentFOV", pAgentFOV);

		IExamInterface* pInterface;
		BS_RETRIEVE_AND_VALIDATE("Interface", pInterface);

		const auto agentPos{ pInterface->Agent_GetInfo().Position };

		auto entitiesInFov{ pAgentFOV->GetEntitiesInFOV() };

		float minDistance{ FLT_MAX };
		EntityInfo closestItem{};

		for (auto& entity : entitiesInFov)
		{
			auto dist{ DistanceSquared(agentPos, entity.Location) };
			if (entity.Type == eEntityType::ITEM && dist < minDistance)
			{
				minDistance = dist;
				closestItem = entity;
			}
		}

		if (minDistance < FLT_MAX)
		{
			ItemInfo itemInfo{};
			pInterface->Item_GetInfo(closestItem, itemInfo);
			/*std::string itemName{};
			switch (itemInfo.Type)
			{
			case eItemType::PISTOL:
				itemName = "pistol";
				break;
			case eItemType::SHOTGUN:
				itemName = "shotgun";
				break;
			case eItemType::MEDKIT:
				itemName = "medkit";
				break;
			case eItemType::FOOD:
				itemName = "food";
				break;
			case eItemType::GARBAGE:
				itemName = "garbage";
				break;
			}
			std::cout << "Closest item is: " << itemName << "\n";*/
			pBlackboard->ChangeData("Item", closestItem);
			return BehaviorState::Success;
		}

		return BehaviorState::Failure;
	}

	inline BehaviorState GrabItem(Blackboard* pBlackboard)
	{
		bool isValid{};
		
		AgentInventory* pInventory;
		BS_RETRIEVE_AND_VALIDATE("Inventory", pInventory);

		EntityInfo itemEntityInfo;
		BS_RETRIEVE_AND_VALIDATE("Item", itemEntityInfo);

		pInventory->PickUpItem(itemEntityInfo);
		
		return BehaviorState::Success;
	}

	inline BehaviorState ApproachItem(Blackboard* pBlackboard)
	{
		bool isValid{};

		EntityInfo itemEntityInfo;
		BS_RETRIEVE_AND_VALIDATE("Item", itemEntityInfo);

		AgentMovement* pAgentMovement;
		BS_RETRIEVE_AND_VALIDATE("AgentMovement", pAgentMovement);

		pAgentMovement->SetToSeek(itemEntityInfo.Location);
		return BehaviorState::Success;
	}

	inline BehaviorState ScanArea(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentMovement* pAgentMovement;
		BS_RETRIEVE_AND_VALIDATE("AgentMovement", pAgentMovement);

		pAgentMovement->SetToSeekAndScan();
		return BehaviorState::Success;
	}

	inline BehaviorState EatFood(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentInventory* pInventory;
		BS_RETRIEVE_AND_VALIDATE("Inventory", pInventory);

		pInventory->UseFood();

		return BehaviorState::Failure;
	}

	inline BehaviorState EscapePurgeZone(Blackboard* pBlackboard)
	{
		bool isValid{};
		PurgeZoneInfo purgeZone;
		BS_RETRIEVE_AND_VALIDATE("PurgeZone", purgeZone);

		IExamInterface* pInterface;
		BS_RETRIEVE_AND_VALIDATE("Interface", pInterface);

		AgentMovement* pMovement;
		BS_RETRIEVE_AND_VALIDATE("AgentMovement", pMovement);

		auto agentPos{ pInterface->Agent_GetInfo().Position };
		auto quickestEscapePoint{ agentPos - purgeZone.Center };
		//quickestEscapePoint.Normalize();
		quickestEscapePoint *= purgeZone.Radius;
		pMovement->SetToSprintSeek(quickestEscapePoint);
		return BehaviorState::Success;
		
	}

	inline BehaviorState SprintAway(Blackboard* pBlackboard)
	{
		bool isValid{};
		AgentMovement* pMovement;
		BS_RETRIEVE_AND_VALIDATE("AgentMovement", pMovement);

		pMovement->SetToSprintSeek();

		return BehaviorState::Success;
	}

	inline BehaviorState UseMedkit(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentInventory* pInventory;
		BS_RETRIEVE_AND_VALIDATE("Inventory", pInventory);

		pInventory->UseMedkit();

		return BehaviorState::Failure;
	}

	inline BehaviorState GetClosestUnvisitedHouseInMemory(Blackboard* pBlackboard)
	{
		bool isValid{};
		AgentMemory* pMemory;
		BS_RETRIEVE_AND_VALIDATE("AgentMemory", pMemory);

		IExamInterface* pInterface;
		BS_RETRIEVE_AND_VALIDATE("Interface", pInterface);

		if(!pMemory->GetHouseMemory()->DoesAnyHouseNeedRevisit())
		{
			return BehaviorState::Failure;
		}

		const auto houseInfo{ pMemory->GetHouseMemory()->GetClosestUnvisitedRememberedHouse(pInterface->Agent_GetInfo().Position) };

		pBlackboard->ChangeData("House", houseInfo);
		return BehaviorState::Success;
	}
}

namespace BT_Conditions
{
	inline bool NothingInFOV(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentFOV* pAgentFOV;
		BC_RETRIEVE_AND_VALIDATE("AgentFOV", pAgentFOV);

		if (!pAgentFOV->GetHousesInFOV().empty())
		{
			return false;
		}

		if (!pAgentFOV->GetEntitiesInFOV().empty())
		{
			return false;
		}
		
		return true;
	}

	inline bool EnemyInFOV(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentFOV* pAgentFOV;
		BC_RETRIEVE_AND_VALIDATE("AgentFOV", pAgentFOV);

		std::vector<EntityInfo> entityVec{ pAgentFOV->GetEntitiesInFOV() };
		auto isEnemy{ [&](const EntityInfo& entity) {return entity.Type == eEntityType::ENEMY; } };
		auto foundEnemy{ std::find_if(begin(entityVec), end(entityVec), isEnemy) };
		if (foundEnemy != entityVec.end())
		{
			const EntityInfo pEnemyInfo{*foundEnemy};
			pBlackboard->ChangeData("Enemy", pEnemyInfo);
			return true;
		}

		return false;
	}

	inline bool FoundHouseUnvisited(Blackboard* pBlackboard)
	{
		bool isValid{};

		HouseInfo houseInfo;
		BC_RETRIEVE_AND_VALIDATE("House", houseInfo);

		AgentMemory* pAgentMemory;
		BC_RETRIEVE_AND_VALIDATE("AgentMemory", pAgentMemory);
		HouseMemory* pHouseMemory{ pAgentMemory->GetHouseMemory() };

		if(!pHouseMemory->IsHouseInMemory(houseInfo))
		{
			return false;
		}

		if(pHouseMemory->GetRememberedHouse(houseInfo).visited)
		{
			return false;
		}
		return true;
	}

	inline bool ItemInGrabbingRange(Blackboard* pBlackboard)
	{
		bool isValid{};

		IExamInterface* pInterface;
		BC_RETRIEVE_AND_VALIDATE("Interface", pInterface);

		EntityInfo itemEntityInfo;
		BC_RETRIEVE_AND_VALIDATE("Item", itemEntityInfo);

		auto agentPos{ pInterface->Agent_GetInfo().Position };
		const auto grabbingRange{ pInterface->Agent_GetInfo().GrabRange };
		
		if (DistanceSquared(itemEntityInfo.Location, agentPos) < grabbingRange * grabbingRange)
		{
			return true;
		}
		return false;
	}

	inline bool NotAlignedWithEnemy(Blackboard* pBlackboard)
	{
		bool isValid{};

		EntityInfo enemyEntityInfo;
		BC_RETRIEVE_AND_VALIDATE("Enemy", enemyEntityInfo);

		IExamInterface* pInterface;
		BC_RETRIEVE_AND_VALIDATE("Interface", pInterface);

		const auto agentPos{ pInterface->Agent_GetInfo().Position };
		const auto enemyPos{ enemyEntityInfo.Location };
		const auto vecToEnemy{ enemyPos - agentPos };
		const float angleToEnemy{ atan2f(vecToEnemy.y, vecToEnemy.x) };
		const float angleOfAgent{ pInterface->Agent_GetInfo().Orientation };

		if(abs(angleToEnemy - angleOfAgent) > 0.015f)
		{
			return true;
		}

		return false;
	}

	inline bool LowOnEnergy(Blackboard *pBlackboard)
	{
		bool isValid{};
		IExamInterface* pInterface;
		BC_RETRIEVE_AND_VALIDATE("Interface", pInterface);

		/*AgentInventory* pInventory;
		BC_RETRIEVE_AND_VALIDATE("AgentInventory", pInventory);*/

		if(pInterface->Agent_GetInfo().Energy < 1.f /*&& pInventory->HasItem(eItemType::FOOD)*/)
		{
			return true;
		}
		return false;
	}

	inline bool CouldUseMedkit(Blackboard* pBlackboard)
	{
		bool isValid{};

		IExamInterface* pInterface;
		BC_RETRIEVE_AND_VALIDATE("Interface", pInterface);

		if(pInterface->Agent_GetInfo().Health < 8.f)
		{
			return true;
		}
		return false;
	}

	inline bool ShouldScanSurroundings(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentMovement* pMovement;
		BC_RETRIEVE_AND_VALIDATE("AgentMovement", pMovement);

		if(pMovement->GetShouldScan())
		{
			return true;
		}
		return false;
	}

	inline bool WasBitten(Blackboard* pBlackboard)
	{
		bool isValid{};

		IExamInterface* pInterface;
		BC_RETRIEVE_AND_VALIDATE("Interface", pInterface);

		if(pInterface->Agent_GetInfo().WasBitten)
		{
			return true;
		}
		return false;
	}

	inline bool IsInPurgeZone(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentFOV* pAgentFov;
		BC_RETRIEVE_AND_VALIDATE("AgentFOV", pAgentFov);

		IExamInterface* pInterface;
		BC_RETRIEVE_AND_VALIDATE("Interface", pInterface);

		const auto entitiesInFov{ pAgentFov->GetEntitiesInFOV() };
		for(auto entity : entitiesInFov)
		{
			if(entity.Type == eEntityType::PURGEZONE)
			{
				PurgeZoneInfo purgeInfo;
				pInterface->PurgeZone_GetInfo(entity, purgeInfo);
				pBlackboard->ChangeData("PurgeZone", purgeInfo);
				return true;
			}
		}
		return false;
	}

	inline bool HasGun(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentInventory* pInventory;
		BC_RETRIEVE_AND_VALIDATE("Inventory", pInventory);

		if(pInventory->HasItem(eItemType::PISTOL) || pInventory->HasItem(eItemType::SHOTGUN))
		{
			return true;
		}
		return false;
	}

	inline bool IsDoneExploring(Blackboard* pBlackboard)
	{
		bool isValid{};
		AgentMemory* pMemory;
		BC_RETRIEVE_AND_VALIDATE("AgentMemory", pMemory);

		return pMemory->GetExplorationMemory()->IsDoneExploring();
	}

	inline bool IfNoWeaponsOrFood(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentInventory* pInventory;
		BC_RETRIEVE_AND_VALIDATE("Inventory", pInventory);

		if (pInventory->HasItem(eItemType::FOOD) && (pInventory->HasItem(eItemType::PISTOL) || pInventory->HasItem(eItemType::SHOTGUN)))
		{
			return false;
		}
		return true;
	}
}

