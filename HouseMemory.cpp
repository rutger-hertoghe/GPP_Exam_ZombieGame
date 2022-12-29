#include "stdafx.h"
#include "HouseMemory.h"

void HouseMemory::Update(float dt)
{
	for (auto& house : m_Houses)
	{
		if (house.second.visited)
		{
			house.second.timeSinceLastVisit += dt;
		}
		if (house.second.timeSinceLastVisit > m_TimeForRevisit)
		{
			house.second.visited = false;
		}
	}
}

bool HouseMemory::IsHouseInMemory(const HouseInfo& house)
{
	try
	{
		auto throwAwayVal{ m_Houses.at(ConvertHousePosToKey(house.Center)) };
	}
	catch (...)
	{
		return false;
	}
	return true;
}

void HouseMemory::AddHouse(const HouseInfo& house)
{
	RememberedHouse newHouse{house};
	
	m_Houses.emplace(ConvertHousePosToKey(house.Center), newHouse);
}

void HouseMemory::SetHouseToVisited(const HouseInfo& house)
{
	auto& rememberedHouse{ m_Houses.at(ConvertHousePosToKey(house.Center)) };
	rememberedHouse.visited = true;
	rememberedHouse.timeSinceLastVisit = 0.f;
}

RememberedHouse HouseMemory::GetRememberedHouse(const HouseInfo& house)
{
	RememberedHouse houseMem{ m_Houses.at(ConvertHousePosToKey(house.Center)) };
	return houseMem;
}

RememberedHouse& HouseMemory::GetRememberedHouseREF(const HouseInfo& house)
{
	return m_Houses.at(ConvertHousePosToKey(house.Center));
}

long int HouseMemory::ConvertHousePosToKey(const Elite::Vector2& houseCenter)
{
	long int key{};
	long int keyP1{ static_cast<long int>(houseCenter.x) };
	long int keyP2{ static_cast<long int>(houseCenter.y) };
	keyP2 <<= sizeof(int);
	key = keyP1 & keyP2;
	return key;
}
