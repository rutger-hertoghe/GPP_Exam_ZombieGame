#pragma once
#include <unordered_map>
#include "CustomStructs.h"
#include "Exam_HelperStructs.h"

class HouseMemory
{
public:
	HouseMemory() = default;
	
	void Update(float dt);
	bool IsHouseInMemory(const HouseInfo& house);
	void AddHouse(const HouseInfo& house);
	void SetHouseToVisited(const HouseInfo& house);
	RememberedHouse GetRememberedHouse(const HouseInfo& house);

private:
	const float m_TimeForRevisit{ 100.f };
	std::unordered_map<long int, RememberedHouse> m_Houses;
	static long int ConvertHousePosToKey(const Elite::Vector2& houseCenter);
};

