#include "stdafx.h"
#include "DecisionModule.h"
#include "EBehaviorTree.h"
#include "BT_Behaviors.h"

using namespace Elite;

DecisionModule::DecisionModule(Elite::Blackboard* pBlackboard)
{
	Initialize(pBlackboard);
}

DecisionModule::~DecisionModule()
{
	SAFE_DELETE(m_pDecisionMaking);
}

void DecisionModule::Initialize(Blackboard* pBlackboard)
{
	BehaviorSequence* zombieInFovRoutine
	{
		new BehaviorSequence{std::vector<IBehavior*>
		{
			new BehaviorConditional{BT_Conditions::EnemyInFOV},
			new BehaviorConditional{BT_Conditions::HasGun},
			new BehaviorAction{BT_Actions::FleeFacingTarget},
			new BehaviorSelector{std::vector<IBehavior*>{
				new BehaviorConditional{BT_Conditions::NotAlignedWithEnemy},
				new BehaviorAction{BT_Actions::UseShotgun},
				new BehaviorAction{BT_Actions::UsePistol},
			}}
		}}
	};

	BehaviorSequence* lowEnergyRoutine
	{
		new BehaviorSequence{std::vector<IBehavior*>
		{
			new BehaviorConditional{BT_Conditions::LowOnEnergy},
			new BehaviorAction{BT_Actions::EatFood}
		}}
	};

	BehaviorSequence* itemGrabRoutine
	{
		new BehaviorSequence{std::vector<IBehavior*>{
			new BehaviorAction{BT_Actions::GetClosestItemInFOV},
			new BehaviorSelector{std::vector<IBehavior*>{
				new BehaviorSequence{std::vector<IBehavior*>{
					new BehaviorConditional{BT_Conditions::ItemInGrabbingRange},
					new BehaviorAction{BT_Actions::GrabItem}
				}},
				new BehaviorAction{BT_Actions::ApproachItem}
			}},
		}}
	};

	BehaviorSequence* houseRoutine
	{
		new BehaviorSequence{std::vector<IBehavior*>
		{
			new BehaviorAction{BT_Actions::GetUnvisitedHouseInFOV}, // ALWAYS RETURNING TRUE
			new BehaviorConditional{BT_Conditions::FoundHouseUnvisited},
			new BehaviorAction{BT_Actions::EnterHouse}
		}}
	};

	BehaviorSelector* exploringRoutine
	{
		new BehaviorSelector{std::vector<IBehavior*>{
			new BehaviorSequence{std::vector<IBehavior*>{
				new BehaviorSelector{std::vector<IBehavior*>{
					new BehaviorConditional{BT_Conditions::IsDoneExploring},
					new BehaviorConditional{BT_Conditions::IfNoWeaponsOrFood}
				}},
				new BehaviorAction{BT_Actions::GetClosestUnvisitedHouseInMemory}
			}},
			new BehaviorAction{BT_Actions::Explore}
		}}
	};

	BehaviorSequence* scanRoutine
	{
		new BehaviorSequence{std::vector<IBehavior*>{
			new BehaviorConditional{BT_Conditions::ShouldScanSurroundings},
			new BehaviorAction{BT_Actions::ScanArea}
		}}
	};

	BehaviorSequence* bittenRoutine
	{
		new BehaviorSequence{std::vector<IBehavior*>{
			new BehaviorConditional{BT_Conditions::WasBitten},
			new BehaviorAction{BT_Actions::SprintAway}
		}}
	};

	BehaviorSequence* purgeZoneRoutine
	{
		new BehaviorSequence{std::vector<IBehavior*>{
			new BehaviorConditional{BT_Conditions::IsInPurgeZone},
			new BehaviorAction{BT_Actions::EscapePurgeZone}
		}}
	};

	BehaviorSequence* medkitRoutine
	{
		new BehaviorSequence{std::vector<IBehavior*>{
			new BehaviorConditional{BT_Conditions::CouldUseMedkit},
			new BehaviorAction{BT_Actions::UseMedkit}
		}}
	};

	Elite::IBehavior* pRootNode{
		new BehaviorSelector{std::vector<IBehavior*>
		{
			zombieInFovRoutine,
			purgeZoneRoutine,
			lowEnergyRoutine,
			medkitRoutine,
			bittenRoutine,
			itemGrabRoutine,
			houseRoutine,
			scanRoutine,
			exploringRoutine
		}}
	};

	m_pDecisionMaking = new BehaviorTree{ pBlackboard, pRootNode };
}

void DecisionModule::Update(float dt)
{
	m_pDecisionMaking->Update(dt);
}
