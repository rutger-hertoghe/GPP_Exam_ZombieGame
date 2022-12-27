#pragma once
#include "EBehaviorTree.h"
#include "Exam_HelperStructs.h"
#include "SteeringBehaviours.h"

using namespace Elite;

namespace BT_Actions
{
	inline BehaviorState SetToSeek(Blackboard* pBlackboard)
	{
		Plugin* pPlugin;
		bool isValid{ pBlackboard->GetData("Plugin", pPlugin) };
		if (!isValid)
		{
			std::cout << "pPlugin in " << __func__ << "was invalid!\n";
			return BehaviorState::Failure;
		}

		pPlugin->SetToSeek();
		return BehaviorState::Success;
	}
}

namespace BT_Conditions
{

}

