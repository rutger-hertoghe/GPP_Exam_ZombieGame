#pragma once
#include "Exam_HelperStructs.h"

class IExamInterface;

class AgentInventory
{
public:
	AgentInventory() = delete;
	explicit AgentInventory(IExamInterface* pInterface);

	bool PickUpItem(const EntityInfo& itemEntityInfo);
	bool HasItem(eItemType itemType);
	bool UsePistol();	// Deprecated
	bool UseShotgun();	// Deprecated
	bool UseFood();		// Deprecated
	bool UseMedkit();	// Deprecated
	bool UseItem(eItemType itemType);

private:
	UINT m_NrSlots;
	IExamInterface* m_pInterface;
	std::vector<eItemType> m_Items;

	UINT GetFirstEmptySlot();
	UINT FindSlotWithItem(eItemType itemType);
	bool AddItem(UINT itemSlot, const EntityInfo& itemEntityInfo);
	bool RemoveItem(UINT slot);
	eItemType GetItemWithHighestQuantity();
	int GetItemQuality(ItemInfo itemInfo) const; // no const& because copy is required
	std::vector<UINT> GetAllSlotsWithItem(eItemType itemType) const;
	UINT GetLowestQualityDuplicateOfItem(const ItemInfo& grabbedItem);
	UINT GetLowestQualityDuplicateOfItem(eItemType itemType);
};

