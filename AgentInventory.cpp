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

	ItemInfo itemInfo;
	m_pInterface->Item_Grab(itemEntityInfo, itemInfo);

	if (itemInfo.Type == eItemType::GARBAGE)
	{
		m_pInterface->Item_Destroy(itemEntityInfo);
	}

	UINT emptySlot{ GetFirstEmptySlot()};
	if (emptySlot == m_NrSlots)
	{
		std::cout << "Inventory is full!\n";
		return false;
	}

	if (m_pInterface->Inventory_AddItem(emptySlot, itemInfo))
	{
		m_Items[emptySlot] = itemInfo.Type;
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
		// TODO: discard pistol here?
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
		// TODO: discard pistol here?
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
