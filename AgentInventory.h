#pragma once
#include "Exam_HelperStructs.h"

class IExamInterface;

class AgentInventory
{
public:
	AgentInventory() = delete;
	explicit AgentInventory(IExamInterface* pInterface);

	bool PickUpItem(const EntityInfo& itemEntityInfo);
	bool UsePistol();
	bool UseShotgun();
	bool HasItem(eItemType itemType);
	bool UseFood();

private:
	UINT m_NrSlots;
	IExamInterface* m_pInterface;
	std::vector<eItemType> m_Items;

	UINT GetFirstEmptySlot();
	UINT FindSlotWithItem(eItemType itemType);
	void RemoveItem(UINT slot);
};

