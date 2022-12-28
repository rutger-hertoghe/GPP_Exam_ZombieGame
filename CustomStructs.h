#pragma once
#include "stdafx.h"
#include "Exam_HelperStructs.h"

using namespace Elite;

struct RememberedHouse
{
	Vector2 center;
	Vector2 size;
	float timeSinceLastVisit;
	bool perimeterScouted;
};