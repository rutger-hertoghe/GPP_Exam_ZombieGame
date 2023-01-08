#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"
#include "EBehaviorTree.h"
#include "EBlackboard.h"
#include "SteeringBehaviours.h"
#include "ExplorationMemory.h"
#include "AgentMemory.h"
#include "AgentFOV.h"
#include "AgentMovement.h"
#include "HouseMemory.h"
#include "AgentInventory.h"
#include "DecisionModule.h"

using namespace std;
using namespace Elite;

Plugin::~Plugin()
{
	SAFE_DELETE(m_pAgentMemory);
	SAFE_DELETE(m_pDecisionModule);
	SAFE_DELETE(m_pAgentFOV);
	SAFE_DELETE(m_pAgentInventory);
	SAFE_DELETE(m_pAgentMovement);
}

//Called only once, during initialization
void Plugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);

	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "Sloeber";
	info.Student_FirstName = "Rutger";
	info.Student_LastName = "Hertoghe";
	info.Student_Class = "2DAE07";

	m_pAgentFOV = new AgentFOV{ m_pInterface };
	m_pAgentMovement = new AgentMovement{ m_pInterface };
	m_pAgentInventory = new AgentInventory{ m_pInterface };

	ExplorationMemory* explorationMem
	{
		new ExplorationMemory(
		m_pInterface->World_GetInfo().Center, 
		m_pInterface->World_GetInfo().Dimensions, 
		m_pInterface->Agent_GetInfo().FOV_Range,
		10)
	};
	HouseMemory* houseMemory{ new HouseMemory{} };
	m_pAgentMemory = new AgentMemory{ explorationMem, houseMemory };

	Blackboard* pBlackboard = CreateBlackboard();

	m_pDecisionModule = new DecisionModule{ pBlackboard };
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
	params.SpawnDebugPistol = false;
	params.SpawnDebugShotgun = false;
	params.SpawnPurgeZonesOnMiddleClick = true;
	params.PrintDebugMessages = true;
	params.ShowDebugItemNames = true;
	params.Seed = 17;
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
void Plugin::Update(float dt)
{
	////Demo Event Code
	////In the end your AI should be able to walk around without external input
	//if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	//{
	//	//Update target based on input
	//	Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
	//	const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
	//	//m_Target = m_pInterface->Debug_ConvertScreenToWorld(pos);
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
	//{
	//	m_CanRun = true;
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
	//{
	//	m_AngSpeed -= Elite::ToRadians(10);
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
	//{
	//	m_AngSpeed += Elite::ToRadians(10);
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
	//{
	//	m_GrabItem = true;
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
	//{
	//	m_UseItem = true;
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
	//{
	//	m_RemoveItem = true;
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	//{
	//	m_CanRun = false;
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Delete))
	//{
	//	m_pInterface->RequestShutdown();
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_M))
	//{
	//	if (m_InventorySlot > 0)
	//		--m_InventorySlot;
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_P))
	//{
	//	if (m_InventorySlot < 4)
	//		++m_InventorySlot;
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Q))
	//{
	//	ItemInfo info = {};
	//	m_pInterface->Inventory_GetItem(m_InventorySlot, info);
	//	std::cout << (int)info.Type << std::endl;
	//}
	//else if(m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_F))
	//{
	//	m_pAgentInventory->UseFood();
	//}
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{
	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	auto agentInfo = m_pInterface->Agent_GetInfo();

	m_pAgentMemory->Update(dt, agentInfo);
	m_pDecisionModule->Update(dt);
	m_pAgentMovement->Update(dt);

	auto steering{ m_pAgentMovement->CalculateSteering(dt, agentInfo) };

//@End (Demo Purposes)
	//m_GrabItem = false; //Reset State
	//m_UseItem = false;
	//m_RemoveItem = false;

	return steering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	m_pInterface->Draw_Circle({ 0.f, 0.f }, 200.f, { 1.f, 0.f, 1.f });
	m_pInterface->Draw_Circle({ 0.f, 0.f }, 250.f, { 1.f, 0.f, 1.f });
	for(const auto& point : m_pAgentMemory->GetExplorationMemory()->GetExplorationPoints())
	{
		m_pInterface->Draw_Circle(point, 1.f, { 1.f, 0.f, 1.f });
	}
}

Elite::Blackboard* Plugin::CreateBlackboard()
{
	Elite::Blackboard* pBlackboard{ new Elite::Blackboard{} };
	pBlackboard->AddData("Interface", m_pInterface);
	pBlackboard->AddData("Plugin", this);
	pBlackboard->AddData("Enemy", EntityInfo{});
	pBlackboard->AddData("AgentMemory", m_pAgentMemory);
	pBlackboard->AddData("AgentFOV", m_pAgentFOV);
	pBlackboard->AddData("AgentMovement", m_pAgentMovement);
	pBlackboard->AddData("House", HouseInfo{});
	pBlackboard->AddData("Item", EntityInfo{});
	pBlackboard->AddData("Inventory", m_pAgentInventory);
	pBlackboard->AddData("PurgeZone", PurgeZoneInfo{});
	return pBlackboard;
}
