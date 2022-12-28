#include "stdafx.h"
#include "HouseMemory.h"

void HouseMemory::Update(float dt)
{
	for (auto& house : m_Houses)
	{
		house.second.timeSinceLastVisit += dt;
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
	RememberedHouse newHouse{};
	newHouse.center = house.Center;
	newHouse.size = house.Size;
	newHouse.timeSinceLastVisit = 0.f; // Variable needs to be higher than revisit triggering float, otherwise house will be remembered but not visited
	
	m_Houses.emplace(ConvertHousePosToKey(house.Center), newHouse);
}

void HouseMemory::SetHouseToVisited(const HouseInfo& house)
{
	m_Houses.at(ConvertHousePosToKey(house.Center)).timeSinceLastVisit = 0.f;
}

RememberedHouse HouseMemory::GetRememberedHouse(const HouseInfo& house)
{
	RememberedHouse houseMem{ m_Houses.at(ConvertHousePosToKey(house.Center)) };
	return houseMem;
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
