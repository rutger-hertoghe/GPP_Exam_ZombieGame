#include "stdafx.h"
#include "CustomStructs.h"

RememberedHouse::RememberedHouse(const HouseInfo& houseInfo)
	: center{houseInfo.Center}
	, size{houseInfo.Size}
	, timeSinceLastVisit{ 0.f }
	, visited{false}
{
	GenerateWayPoints();
}

void RememberedHouse::GenerateWayPoints()
{
	constexpr float waypointOffset{ 10.f };
	const float halfX{ size.x / 2 - waypointOffset};
	const float halfY{ size.y / 2 - waypointOffset};
	Elite::Vector2 topRight{ center.x + halfX, center.y + halfY };
	Elite::Vector2 topLeft{ center.x - halfX, center.y + halfY };
	Elite::Vector2 botRight{ center.x + halfX, center.y - halfY };
	Elite::Vector2 botLeft{ center.x - halfX, center.y - halfY };
	m_WayPoints.emplace_back(topRight);
	m_WayPoints.emplace_back(botLeft);
	m_WayPoints.emplace_back(botRight);
	m_WayPoints.emplace_back(topLeft);
}

//std::vector<Elite::Vector2> RememberedHouse::CalculateVertices() const
//{
//	std::vector<Elite::Vector2> vertices;
//	const float halfX{ size.x / 2 };
//	const float halfY{ size.y / 2 };
//	Elite::Vector2 topRight{center.x + halfX, center.y + halfY};
//	Elite::Vector2 topLeft{center.x - halfX, center.y + halfY};
//	Elite::Vector2 botRight{center.x + halfX, center.y - halfY};
//	Elite::Vector2 botLeft{ center.x - halfX, center.y - halfY };
//	vertices.emplace_back(topRight);
//	vertices.emplace_back(botLeft);
//	vertices.emplace_back(botRight);
//	vertices.emplace_back(topLeft);
//	return vertices;
//}
