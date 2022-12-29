#include "stdafx.h"
#include "AgentInventory.h"
#include "IExamInterface.h"

AgentInventory::AgentInventory(IExamInterface* pInterface)
	: m_pInterface{pInterface}
{
	m_NrSlots = m_pInterface->Inventory_GetCapacity();
	for (UINT i{ 0 }; i < m_NrSlots; ++i)
	{
		m_Items.push_back(eItemType::RANDOM_DROP);
	}
}

bool AgentInventory::PickUpItem(const EntityInfo& itemEntityInfo)
{
	if (itemEntityInfo.Type != eEntityType::ITEM)
	{
		std::cout << "Tried to pick up non-item!!\n";
		return false;
	}

	ItemInfo grabbedItem;
	m_pInterface->Item_Grab(itemEntityInfo, grabbedItem);

	/*switch(grabbedItem.Type)
	{
	case eItemType::GARBAGE:
		m_pInterface->Item_Destroy(itemEntityInfo);
		break;
	case eItemType::PISTOL:
		const UINT pistolSlot{ FindSlotWithItem(eItemType::PISTOL) };
		if(pistolSlot != m_NrSlots)
		{
			ItemInfo currentPistol;
			m_pInterface->Inventory_GetItem(pistolSlot, currentPistol);
			if (m_pInterface->Weapon_GetAmmo(grabbedItem) > m_pInterface->Weapon_GetAmmo(currentPistol))
			{
				m_pInterface->Inventory_RemoveItem(pistolSlot);
				m_pInterface->Inventory_AddItem(pistolSlot, grabbedItem);
			}
			break;
		}
		
	}*/

	UINT emptySlot{ GetFirstEmptySlot()};
	if (emptySlot == m_NrSlots)
	{
		std::cout << "Inventory is full!\n";
		return false;
	}

	if (m_pInterface->Inventory_AddItem(emptySlot, grabbedItem))
	{
		m_Items[emptySlot] = grabbedItem.Type;
		if (grabbedItem.Type == eItemType::GARBAGE)
		{
			RemoveItem(emptySlot);
		}
		return true;
	}

	
	std::cout << "Item not added to inventory\n";
	return false;
}

bool AgentInventory::UsePistol()
{
	UINT pistolSlot{ FindSlotWithItem(eItemType::PISTOL) };
	if (pistolSlot == m_NrSlots)
	{
		std::cout << "No pistol equipped!\n";
		return false;
	}

	bool hasShot{ m_pInterface->Inventory_UseItem(pistolSlot) };
	if (!hasShot)
	{
		std::cout << "Out of ammo!\n";
		RemoveItem(pistolSlot);
	}
	return hasShot;
}

bool AgentInventory::UseShotgun()
{
	UINT shotgunSlot{ FindSlotWithItem(eItemType::SHOTGUN) };
	if (shotgunSlot == m_NrSlots)
	{
		std::cout << "No shotgun equipped!\n";
		return false;
	}

	bool hasShot{ m_pInterface->Inventory_UseItem(shotgunSlot) };
	if (!hasShot)
	{
		std::cout << "Out of ammo!\n";
		RemoveItem(shotgunSlot);
	}
	return hasShot;
}

bool AgentInventory::HasItem(eItemType itemType)
{
	if (FindSlotWithItem(itemType) == m_NrSlots)
	{
		return false;
	}
	return true;
}

bool AgentInventory::UseFood()
{
	UINT foodSlot{ FindSlotWithItem(eItemType::FOOD) };
	if(foodSlot == m_NrSlots)
	{
		return false;
	}

	ItemInfo itemInfo;
	m_pInterface->Inventory_GetItem(foodSlot, itemInfo);

	std::cout << "Using food \n";
	bool usedFood{ m_pInterface->Inventory_UseItem(foodSlot) };
	RemoveItem(foodSlot);
	
	return usedFood;
}

UINT AgentInventory::GetFirstEmptySlot()
{
	for (UINT slot{ 0 }; slot < m_NrSlots; ++slot)
	{
		if (m_Items[slot] == eItemType::RANDOM_DROP)
		{
			return slot;
		}
	}
	return m_NrSlots;
}

UINT AgentInventory::FindSlotWithItem(eItemType itemType)
{
	for (UINT slot{ 0 }; slot < m_NrSlots; ++slot)
	{
		if (m_Items[slot] == itemType)
		{
			return slot;
		}
	}
	return m_NrSlots;
}

void AgentInventory::RemoveItem(UINT slot)
{
	m_pInterface->Inventory_RemoveItem(slot);
	m_Items[slot] = eItemType::RANDOM_DROP;
}
