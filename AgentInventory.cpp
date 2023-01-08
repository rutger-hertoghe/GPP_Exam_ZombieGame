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
	// Not an item
	if (itemEntityInfo.Type != eEntityType::ITEM)
	{
		std::cout << "Tried to pick up non-item!!\n";
		return false;
	}

	// Item is garbage
	ItemInfo itemInfo;
	m_pInterface->Item_GetInfo(itemEntityInfo, itemInfo);
	if(itemInfo.Type == eItemType::GARBAGE)
	{
		m_pInterface->Item_Destroy(itemEntityInfo);
		return false;
	}

	// Inventory is full
	UINT emptySlot{ GetFirstEmptySlot()};
	if (emptySlot == m_NrSlots) // No empty slots
	{
		if(HasItem(itemInfo.Type))
		{
			emptySlot = GetLowestQualityDuplicateOfItem(itemInfo);
		}
		else
		{
			emptySlot = GetLowestQualityDuplicateOfItem(GetItemWithHighestQuantity());
		}

		if(emptySlot == m_NrSlots) // If lowest quality item is the one in hand, return
		{
			return false;
		}
		ItemInfo itemToDelete;
		m_pInterface->Inventory_GetItem(emptySlot, itemToDelete);
		if (itemToDelete.Type == eItemType::FOOD || itemToDelete.Type == eItemType::MEDKIT)
		{
			m_pInterface->Inventory_UseItem(emptySlot);
		}
		RemoveItem(emptySlot);
	}

	// Add item to inventory
	return AddItem(emptySlot, itemEntityInfo);
}

bool AgentInventory::UsePistol()
{
	return UseItem(eItemType::PISTOL);
}

bool AgentInventory::UseShotgun()
{
	return UseItem(eItemType::SHOTGUN);
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
	return UseItem(eItemType::FOOD);
}

bool AgentInventory::UseMedkit()
{
	return UseItem(eItemType::MEDKIT);
}

bool AgentInventory::UseItem(eItemType itemType)
{
	const UINT itemSlot{ FindSlotWithItem(itemType) };
	if (itemSlot == m_NrSlots)
	{
		return false;
	}

	const bool usedItem{ m_pInterface->Inventory_UseItem(itemSlot) };

	switch(itemType)
	{
	case eItemType::MEDKIT:
	case eItemType::FOOD:
		RemoveItem(itemSlot);
		return usedItem;

	case eItemType::SHOTGUN:
	case eItemType::PISTOL:
		if (!usedItem)
		{
			std::cout << "Out of ammo!\n";
			RemoveItem(itemSlot);
		}
		return usedItem;
	}
	return false;
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

bool AgentInventory::AddItem(UINT itemSlot, const EntityInfo& itemEntityInfo)
{
	// Grab for further interaction
	ItemInfo grabbedItem;
	m_pInterface->Item_Grab(itemEntityInfo, grabbedItem);
	if (m_pInterface->Inventory_AddItem(itemSlot, grabbedItem))
	{
		m_Items[itemSlot] = grabbedItem.Type;
		return true;
	}

	std::cout << "Item not added to inventory\n";
	return false;
}

bool AgentInventory::RemoveItem(UINT slot)
{
	if(slot == m_NrSlots)
	{
		return false;
	}

	m_pInterface->Inventory_RemoveItem(slot);
	m_Items[slot] = eItemType::RANDOM_DROP;
	return true;
}

eItemType AgentInventory::GetItemWithHighestQuantity()
{
	std::vector<int> quantities;
	quantities.resize(4);
	for(auto& itemType : m_Items)
	{
		switch(itemType)
		{
		case eItemType::PISTOL:
		case eItemType::SHOTGUN:
		case eItemType::MEDKIT:
		case eItemType::FOOD:
			++quantities[static_cast<int>(itemType)];
			break;
		default:
			break;
		}
	}
	return static_cast<eItemType>(std::distance(begin(quantities), std::max_element(begin(quantities), end(quantities))));
}

int AgentInventory::GetItemQuality(ItemInfo itemInfo) const
{
	switch(itemInfo.Type)
	{
	case eItemType::FOOD:
		return m_pInterface->Food_GetEnergy(itemInfo);
	case eItemType::SHOTGUN:
	case eItemType::PISTOL:
		return m_pInterface->Weapon_GetAmmo(itemInfo);
	case eItemType::MEDKIT:
		return m_pInterface->Medkit_GetHealth(itemInfo);
	default:
		return 0;
	}
}

std::vector<UINT> AgentInventory::GetAllSlotsWithItem(eItemType itemType) const
{
	std::vector<UINT> slotsWithItem{};
	for(UINT slot{0}; slot < m_NrSlots; ++slot)
	{
		if(m_Items[slot] == itemType)
		{
			slotsWithItem.push_back(slot);
		}
	}
	return slotsWithItem;
}

UINT AgentInventory::GetLowestQualityDuplicateOfItem(const ItemInfo& grabbedItem)
{
	const auto itemSlots{ GetAllSlotsWithItem(grabbedItem.Type) };
	ItemInfo lowestQualityItem{ grabbedItem };
	UINT lowQslot{ m_NrSlots };
	for(const auto& slot : itemSlots)
	{
		ItemInfo itemToCheck;
		m_pInterface->Inventory_GetItem(slot, itemToCheck);
		if(GetItemQuality(itemToCheck) < GetItemQuality(lowestQualityItem))
		{
			lowestQualityItem = itemToCheck;
			lowQslot = slot;
		}
	}

	return lowQslot;
}

UINT AgentInventory::GetLowestQualityDuplicateOfItem(eItemType itemType)
{
	const auto itemSlots{ GetAllSlotsWithItem(itemType) };
	ItemInfo lowestQualityItem{};
	m_pInterface->Inventory_GetItem(itemSlots[0], lowestQualityItem);
	UINT lowQslot{ 0 };
	for (const auto& slot : itemSlots)
	{
		ItemInfo itemToCheck;
		m_pInterface->Inventory_GetItem(slot, itemToCheck);
		if (GetItemQuality(itemToCheck) < GetItemQuality(lowestQualityItem))
		{
			lowestQualityItem = itemToCheck;
			lowQslot = slot;
		}
	}

	return lowQslot;
}
