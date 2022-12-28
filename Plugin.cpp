#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"
#include "EBehaviorTree.h"
#include "EBlackboard.h"
#include "SteeringBehaviours.h"
#include "BT_Behaviors.h"
#include "ExplorationMemory.h"
#include "AgentMemory.h"
#include "AgentFOV.h"
#include "AgentMovement.h"
#include "HouseMemory.h"

using namespace std;
using namespace Elite;

Plugin::~Plugin()
{
	SAFE_DELETE(m_pAgentMemory);
	SAFE_DELETE(m_pDecisionMaking);
}

//Called only once, during initialization
void Plugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);

	HouseInfo testHouse{};
	testHouse.Center = { 100.f, 100.f };

	HouseMemory testHouseMemory{};
	testHouseMemory.IsHouseInMemory(testHouse);

	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "Sloeber";
	info.Student_FirstName = "Rutger";
	info.Student_LastName = "Hertoghe";
	info.Student_Class = "2DAE07";

	m_pAgentFOV = new AgentFOV{ m_pInterface };
	m_pAgentMovement = new AgentMovement{m_pInterface};

	ExplorationMemory* explorationMem{new ExplorationMemory(m_pInterface->World_GetInfo().Center, m_pInterface->World_GetInfo().Dimensions, 10)};
	HouseMemory* houseMemory{ new HouseMemory{} };
	m_pAgentMemory = new AgentMemory{ explorationMem, houseMemory };

	Blackboard* pBlackboard = CreateBlackboard();

	BehaviorSequence* exploringRoutine
	{
		new BehaviorSequence{std::vector<IBehavior*>
		{
			new BehaviorConditional{BT_Conditions::NothingInFOV},
			new BehaviorAction{BT_Actions::Explore}
		}}
	};

	BehaviorSequence* zombieInFovRoutine
	{
		new BehaviorSequence{std::vector<IBehavior*>
		{
			new BehaviorConditional{BT_Conditions::EnemyInFOV},
			new BehaviorAction{BT_Actions::FleeFacingTarget}
		}}
	};

	BehaviorSequence* houseInFovRoutine
	{
		new BehaviorSequence{std::vector<IBehavior*>
		{
			new BehaviorConditional{BT_Conditions::HouseInFOV},
			new BehaviorSelector{std::vector<IBehavior*>
			{
				new BehaviorSequence{std::vector<IBehavior*>{
					new BehaviorConditional{BT_Conditions::HouseInMemory},
					new BehaviorConditional{BT_Conditions::HouseNeedsRevisit},
					new BehaviorAction{BT_Actions::EnterHouse}
				}},
				new BehaviorAction{BT_Actions::MemoriseHouse}
			}}
		}}
	};

	Elite::IBehavior* pRootNode{
		new BehaviorSelector{std::vector<IBehavior*>
		{
			houseInFovRoutine,
			//zombieInFovRoutine,
			exploringRoutine
		}}
	};

	m_pDecisionMaking = new BehaviorTree{ pBlackboard, pRootNode };
}

//Called only once
void Plugin::DllInit()
{
	//Called when the plugin is loaded
}

//Called only once
void Plugin::DllShutdown()
{
	//Called when the plugin gets unloaded
}

//Called only once, during initialization
void Plugin::InitGameDebugParams(GameDebugParams& params)
{
	params.AutoFollowCam = true; //Automatically follow the AI? (Default = true)
	params.RenderUI = true; //Render the IMGUI Panel? (Default = true)
	params.SpawnEnemies = true; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = false; //GodMode > You can't die, can be useful to inspect certain behaviors (Default = false)
	params.LevelFile = "GameLevel.gppl";
	params.AutoGrabClosestItem = true; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
	params.StartingDifficultyStage = 1;
	params.InfiniteStamina = false;
	params.SpawnDebugPistol = true;
	params.SpawnDebugShotgun = true;
	params.SpawnPurgeZonesOnMiddleClick = true;
	params.PrintDebugMessages = true;
	params.ShowDebugItemNames = true;
	params.Seed = 36;
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
void Plugin::Update(float dt)
{
	//Demo Event Code
	//In the end your AI should be able to walk around without external input
	if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		//Update target based on input
		Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		//m_Target = m_pInterface->Debug_ConvertScreenToWorld(pos);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
	{
		m_CanRun = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
	{
		m_AngSpeed -= Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
	{
		m_AngSpeed += Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
	{
		m_GrabItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
	{
		m_UseItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
	{
		m_RemoveItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	{
		m_CanRun = false;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Delete))
	{
		m_pInterface->RequestShutdown();
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_M))
	{
		if (m_InventorySlot > 0)
			--m_InventorySlot;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_P))
	{
		if (m_InventorySlot < 4)
			++m_InventorySlot;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Q))
	{
		ItemInfo info = {};
		m_pInterface->Inventory_GetItem(m_InventorySlot, info);
		std::cout << (int)info.Type << std::endl;
	}
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{
	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	auto agentInfo = m_pInterface->Agent_GetInfo();

	m_pAgentMemory->Update(dt, agentInfo);
	m_pDecisionMaking->Update(dt);
	

	//Use the navmesh to calculate the next navmesh point
	//Elite::Vector2 checkpointLocation{ 1000.f, 1000.f };
	//auto nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(checkpointLocation);

	//OR, Use the mouse target
	//auto nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(m_Target); //Uncomment this to use mouse position as guidance

	auto steering{ m_pAgentMovement->CalculateSteering(dt, agentInfo) };
	
	/*if (Distance(nextTargetPos, agentInfo.Position) < 2.f)
	{
		steering.LinearVelocity = Elite::ZeroVector2;
	}*/

	steering.RunMode = m_CanRun; //If RunMode is True > MaxLinSpd is increased for a limited time (till your stamina runs out)
	
	//SteeringPlugin_Output is works the exact same way a SteeringBehaviour output

	//for (auto& e : vEntitiesInFOV)
	//{
	//	if (e.Type == eEntityType::PURGEZONE)
	//	{
	//		PurgeZoneInfo zoneInfo;
	//		m_pInterface->PurgeZone_GetInfo(e, zoneInfo);
	//		//std::cout << "Purge Zone in FOV:" << e.Location.x << ", "<< e.Location.y << "---Radius: "<< zoneInfo.Radius << std::endl;
	//	}
	//}

	//INVENTORY USAGE DEMO
	//********************

	if (m_GrabItem)
	{
		ItemInfo item;
		//Item_Grab > When DebugParams.AutoGrabClosestItem is TRUE, the Item_Grab function returns the closest item in range
		//Keep in mind that DebugParams are only used for debugging purposes, by default this flag is FALSE
		//Otherwise, use GetEntitiesInFOV() to retrieve a vector of all entities in the FOV (EntityInfo)
		//Item_Grab gives you the ItemInfo back, based on the passed EntityHash (retrieved by GetEntitiesInFOV)
		if (m_pInterface->Item_Grab({}, item))
		{
			//Once grabbed, you can add it to a specific inventory slot
			//Slot must be empty
			m_pInterface->Inventory_AddItem(m_InventorySlot, item);
		}
	}

	if (m_UseItem)
	{
		//Use an item (make sure there is an item at the given inventory slot)
		m_pInterface->Inventory_UseItem(m_InventorySlot);
	}

	if (m_RemoveItem)
	{
		//Remove an item from a inventory slot
		m_pInterface->Inventory_RemoveItem(m_InventorySlot);
	}

//@End (Demo Purposes)
	m_GrabItem = false; //Reset State
	m_UseItem = false;
	m_RemoveItem = false;

	return steering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	/*if (m_pInterface)					return;
	if (m_pCurrentSteeringBehaviour)	return;

	m_pInterface->Draw_SolidCircle(m_pCurrentSteeringBehaviour->GetTarget(), .7f, {0,0}, {1, 0, 0});*/
}

Elite::Blackboard* Plugin::CreateBlackboard()
{
	Elite::Blackboard* pBlackboard{ new Elite::Blackboard{} };
	pBlackboard->AddData("Interface", m_pInterface);
	pBlackboard->AddData("Plugin", this);
	pBlackboard->AddData("Enemy", static_cast<EntityInfo*>(nullptr));
	pBlackboard->AddData("ShotgunSlot", static_cast<UINT>(10));
	pBlackboard->AddData("PistolSlot", static_cast<UINT>(10));
	pBlackboard->AddData("AgentMemory", m_pAgentMemory);
	pBlackboard->AddData("AgentFOV", m_pAgentFOV);
	pBlackboard->AddData("AgentMovement", m_pAgentMovement);
	pBlackboard->AddData("House", static_cast<HouseInfo*>(nullptr));
	return pBlackboard;
}