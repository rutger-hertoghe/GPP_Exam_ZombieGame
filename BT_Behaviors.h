#pragma once
#include "EBehaviorTree.h"
#include "Exam_HelperStructs.h"
#include "SteeringBehaviours.h"
#include "ExplorationMemory.h"
#include "AgentMemory.h"
#include "AgentFOV.h"
#include "AgentMovement.h"
#include "HouseMemory.h"
#include "AgentInventory.h"

using namespace Elite;

#define BS_RETRIEVE_AND_VALIDATE(name, data) isValid = pBlackboard->GetData(name, data); if(!isValid){std::cout << VAR_NAME(data) << " in " << __func__ << " was invalid!\n"; return BehaviorState::Failure;}
#define BC_RETRIEVE_AND_VALIDATE(name, data) isValid = pBlackboard->GetData(name, data); if(!isValid){std::cout << VAR_NAME(data) << " in " << __func__ << " was invalid!\n"; return false;}

// TODO: Make house memory functions accept HouseInfo pointers instead of dereferencing pHouse(Info)??

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

		EntityInfo* pEnemyInfo;
		BS_RETRIEVE_AND_VALIDATE("Enemy", pEnemyInfo);

		AgentMovement* pMovement;
		BS_RETRIEVE_AND_VALIDATE("AgentMovement", pMovement);

		pMovement->SetToFlee(pEnemyInfo->Location, false, true);
		return BehaviorState::Success;
	}

	inline BehaviorState GetClosestUnvisitedHouseInFOV(Blackboard* pBlackboard)
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
			return BehaviorState::Failure;
		}

		std::cout << "Houses in view: " << houseVec.size() << "\n";

		// Get the agent position
		const auto agentPos{ pInterface->Agent_GetInfo().Position };
		// Set the minDist to a larger float to start comparing with the highest value possible
		float minDist{ FLT_MAX };
		// HouseInfo to store closest house in
		HouseInfo closestHouse;

		//Start looping through all found houses
		for (auto& house : houseVec)
		{
			// Get the distance to the center of the house
			const float distToHouse{ DistanceSquared(agentPos, house.Center) };
			// If the distance to the current house is smaller than the stored smallest distance, continue with next house in loop
			if (distToHouse > minDist)
			{
				continue;
			}

			if(pHouseMemory->IsHouseInMemory(house))
			{
				closestHouse = house;
				minDist = distToHouse;
				continue;
			}

			pHouseMemory->AddHouse(house);
			closestHouse = house;
			minDist = distToHouse;
		}

		if (minDist < FLT_MAX)
		{
			//return BehaviorState::Failure;
			pBlackboard->ChangeData("House", closestHouse);
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

		pMovement->SetToSeek(houseInfo.Center);

		if (pInterface->Agent_GetInfo().IsInHouse)
		{
			pHouseMemory->SetHouseToVisited(houseInfo);
		}

		return BehaviorState::Success;
	}

	/*inline BehaviorState SetHouseToVisited(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentMemory* pAgentMemory;
		BS_RETRIEVE_AND_VALIDATE("AgentMemory", pAgentMemory);
		
		HouseInfo* pHouse;
		BS_RETRIEVE_AND_VALIDATE("House", pHouse);

		pAgentMemory->GetHouseMemory()->SetHouseToVisited(*pHouse);
		return BehaviorState::Success;
	}*/

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

	inline BehaviorState SetToWander(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentMovement* pMovement;
		BS_RETRIEVE_AND_VALIDATE("AgentMovement", pMovement);

		pMovement->SetToWander();

		return BehaviorState::Success;
	}

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
			std::string itemName{};
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
			std::cout << "Closest item is: " << itemName << "\n";
			pBlackboard->ChangeData("Item", closestItem);
			return BehaviorState::Success;
		}

		return BehaviorState::Failure;
	}

	inline BehaviorState GrabItem(Blackboard* pBlackboard)
	{
		std::cout << "Entered " << __func__ << "\n";
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
			EntityInfo* pEnemyInfo{};
			BC_RETRIEVE_AND_VALIDATE("Enemy", pEnemyInfo);
			if (pEnemyInfo != nullptr)
			{
				delete pEnemyInfo;
			}

			pEnemyInfo = new EntityInfo{};
			pEnemyInfo->Location = foundEnemy->Location;
			pEnemyInfo->EntityHash = foundEnemy->EntityHash;
			pEnemyInfo->Type = foundEnemy->Type;

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

		if(pHouseMemory->GetRememberedHouse(houseInfo).visited)
		{
			return false;
		}
		return true;
	}

	/*inline bool FoundHouseUnvisited(Blackboard* pBlackboard)
	{
		bool isValid{};

		HouseInfo houseInfo;
		BC_RETRIEVE_AND_VALIDATE("House", houseInfo);

		AgentMemory* pAgentMemory;
		BC_RETRIEVE_AND_VALIDATE("AgentMemory", pAgentMemory);

		HouseMemory* pHouseMemory{ pAgentMemory->GetHouseMemory() };

		if (pHouseMemory->IsHouseInMemory(houseInfo))
		{
			auto remHouse{ pHouseMemory->GetRememberedHouse(houseInfo) };
			if (remHouse.visited == false)
			{
				return true;
			}
		}
		return false;
	}*/

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
			std::cout << "Item in range\n";
			return true;
		}
		return false;
	}
}

