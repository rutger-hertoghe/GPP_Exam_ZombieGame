#pragma once
#include "EBehaviorTree.h"
#include "Exam_HelperStructs.h"
#include "SteeringBehaviours.h"
#include "ExplorationMemory.h"
#include "AgentMemory.h"
#include "AgentFOV.h"
#include "AgentMovement.h"
#include "HouseMemory.h"

using namespace Elite;

#define BS_RETRIEVE_AND_VALIDATE(name, data) isValid = pBlackboard->GetData(name, data); if(!isValid){std::cout << VAR_NAME(data) << " in " << __func__ << " was invalid!\n"; return BehaviorState::Failure;}
#define BC_RETRIEVE_AND_VALIDATE(name, data) isValid = pBlackboard->GetData(name, data); if(!isValid){std::cout << VAR_NAME(data) << " in " << __func__ << " was invalid!\n"; return false;}

namespace BT_Actions
{
	//inline BehaviorState SetToSeek(Blackboard* pBlackboard)
	//{
	//	bool isValid{};

	//	AgentMovement* pMovement;
	//	BS_RETRIEVE_AND_VALIDATE("AgentMovement", pMovement);

	//	pMovement->SetToSeek();
	//	return BehaviorState::Success;
	//}

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

	inline BehaviorState MemoriseHouse(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentMemory* pAgentMemory;
		BS_RETRIEVE_AND_VALIDATE("AgentMemory", pAgentMemory);

		HouseInfo* pHouseInfo;
		BS_RETRIEVE_AND_VALIDATE("House", pHouseInfo);

		pAgentMemory->GetHouseMemory()->AddHouse(*pHouseInfo);
		return BehaviorState::Success;
	}

	inline BehaviorState EnterHouse(Blackboard* pBlackboard)
	{
		bool isValid{};
		
		HouseInfo* pHouseInfo;
		BS_RETRIEVE_AND_VALIDATE("House", pHouseInfo);

		AgentMovement* pMovement;
		BS_RETRIEVE_AND_VALIDATE("AgentMovement", pMovement);

		IExamInterface* pInterface;
		BS_RETRIEVE_AND_VALIDATE("Interface", pInterface);

		pMovement->SetToSeek(pHouseInfo->Center);

		if (pInterface->Agent_GetInfo().IsInHouse)
		{
			return BehaviorState::Success;
		}
		return BehaviorState::Running;
	}

	inline BehaviorState UseGun(Blackboard* pBlackboard)
	{
		bool isValid{};
		UINT shotGunSlot;
		BS_RETRIEVE_AND_VALIDATE("ShotgunSlot", shotGunSlot);

		UINT pistolSlot;
		BS_RETRIEVE_AND_VALIDATE("PistolSlot", pistolSlot);

		IExamInterface* pInterface;
		BS_RETRIEVE_AND_VALIDATE("Interface", pInterface);

		UINT inventorySlots{ pInterface->Inventory_GetCapacity() };

		if (shotGunSlot < inventorySlots)
		{
			pInterface->Inventory_UseItem(shotGunSlot);
			return BehaviorState::Success;
		}
		
		if (pistolSlot < inventorySlots)
		{
			pInterface->Inventory_UseItem(pistolSlot);
			return BehaviorState::Success;
		}

		return BehaviorState::Failure;
	}

	inline BehaviorState SetToWander(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentMovement* pMovement;
		BS_RETRIEVE_AND_VALIDATE("AgentMovement", pMovement);

		pMovement->SetToWander();

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

		if (pAgentFOV->GetHousesInFOV().size() > 0)
		{
			return false;
		}

		if (pAgentFOV->GetEntitiesInFOV().size() > 0)
		{
			return false;
		}
		
		return true;
	}

	inline bool HouseInFOV(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentFOV* pAgentFOV;
		BC_RETRIEVE_AND_VALIDATE("AgentFOV", pAgentFOV);

		/*IExamInterface* pInterface;
		BC_RETRIEVE_AND_VALIDATE("Interface", pInterface)*/

		std::vector<HouseInfo> houseVec{ pAgentFOV->GetHousesInFOV() };

		if (houseVec.size() > 0)
		{
			//TODO: somehow pick most relevant house??
			HouseInfo* pHouseInfo{};
			BC_RETRIEVE_AND_VALIDATE("House", pHouseInfo);
			if (pHouseInfo != nullptr)
			{
				delete pHouseInfo;
			}
			pHouseInfo = new HouseInfo{ houseVec[0] };

			pBlackboard->ChangeData("House", pHouseInfo);
			return true;
		}

		return false;
	}

	inline bool HouseInMemory(Blackboard* pBlackboard)
	{
		bool isValid{};

		AgentMemory* pAgentMemory;
		BC_RETRIEVE_AND_VALIDATE("AgentMemory", pAgentMemory);

		HouseInfo* pHouseInfo;
		BC_RETRIEVE_AND_VALIDATE("House", pHouseInfo);

		return pAgentMemory->GetHouseMemory()->IsHouseInMemory(*pHouseInfo);
	}

	inline bool HouseNeedsRevisit(Blackboard* pBlackboard)
	{
		bool isValid{};
		HouseInfo* pHouseInfo;
		BC_RETRIEVE_AND_VALIDATE("House", pHouseInfo);

		AgentMemory* pAgentMemory;
		BC_RETRIEVE_AND_VALIDATE("AgentMemory", pAgentMemory);

		const float timeSinceVisit{ pAgentMemory->GetHouseMemory()->GetRememberedHouse(*pHouseInfo).timeSinceLastVisit};
		const float revisitTime{ 100.f };
		if (timeSinceVisit > revisitTime)
		{
			return true;
		}

		return false;
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
			EntityInfo* pEnemyInfo{ new EntityInfo{} };
			pEnemyInfo->Location = foundEnemy->Location;
			pEnemyInfo->EntityHash = foundEnemy->EntityHash;
			pEnemyInfo->Type = foundEnemy->Type;
			pBlackboard->ChangeData("Enemy", pEnemyInfo);
			return true;
		}

		return false;
	}

	inline bool IsGunInInventory(Blackboard* pBlackboard)
	{
		bool isValid{};
		IExamInterface* pInterface;
		BC_RETRIEVE_AND_VALIDATE("Interface", pInterface);

		const UINT inventorySlots{ pInterface->Inventory_GetCapacity() };
		UINT shotgunSlot{ inventorySlots };
		UINT pistolSlot{ inventorySlots };
		for (UINT i{ 0 }; i < inventorySlots; ++i)
		{
			ItemInfo itemInfo;
			pInterface->Inventory_GetItem(i, itemInfo);
			if (itemInfo.Type == eItemType::PISTOL)
			{
				pistolSlot = i;
				pBlackboard->ChangeData("PistolSlot", pistolSlot);
			}
			else if (itemInfo.Type == eItemType::SHOTGUN)
			{
				shotgunSlot = i;
				pBlackboard->ChangeData("ShotgunSlot", shotgunSlot);
			}
		}

		if (shotgunSlot < inventorySlots || pistolSlot < inventorySlots)
		{
			return true;
		}

		return false;
	}
}

