#pragma once
#include "stdafx.h"
#include "Exam_HelperStructs.h"

using namespace Elite;

struct RememberedHouse
{
	RememberedHouse(const HouseInfo& houseInfo);
	Vector2 center;
	Vector2 size;
	float timeSinceLastVisit;
	bool visited;
	std::vector<Elite::Vector2> m_WayPoints;

private:
	void GenerateWayPoints();
};