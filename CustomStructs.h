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
	bool perimeterScouted;

private:
	std::vector<Elite::Vector2> CalculateVertices() const;
	void CalculateRummagingPoints(std::vector<Elite::Vector2> vertices);
	void CalculatePerimeterPoints(std::vector<Elite::Vector2> vertices);
};