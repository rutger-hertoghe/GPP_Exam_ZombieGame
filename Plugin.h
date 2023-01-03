#pragma once
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"

class IBaseInterface;
class IExamInterface;

namespace Elite 
{
	class Blackboard;
	//class IDecisionMaking;
}

class AgentMemory;
class AgentFOV;
class AgentMovement;
class AgentInventory;
class DecisionModule;

// using namespace Elite;

class Plugin : public IExamPlugin
{
public:

	Plugin() {};
	virtual ~Plugin();

	void Initialize(IBaseInterface* pInterface, PluginInfo& info) override;
	void DllInit() override;
	void DllShutdown() override;

	void InitGameDebugParams(GameDebugParams& params) override;
	void Update(float dt) override;

	SteeringPlugin_Output UpdateSteering(float dt) override;
	void Render(float dt) const override;

private:
	//Interface, used to request data from/perform actions with the AI Framework
	IExamInterface* m_pInterface = nullptr;

	//bool m_CanRun = false; //Demo purpose
	//bool m_GrabItem = false; //Demo purpose
	//bool m_UseItem = false; //Demo purpose
	//bool m_RemoveItem = false; //Demo purpose
	//float m_AngSpeed = 0.f; //Demo purpose

	// UINT m_InventorySlot = 0;

	// OWN IMPLEMENTATIONS
	// Elite::Blackboard* m_pBlackboard;
	Elite::Vector2 m_Target;

	Elite::Blackboard* CreateBlackboard();
	//-----------------------
	// AGENT MODULES
	AgentMemory* m_pAgentMemory;
	AgentInventory* m_pAgentInventory;
	AgentFOV* m_pAgentFOV;
	AgentMovement* m_pAgentMovement;
	DecisionModule* m_pDecisionModule;
	//-----------------------
};

//ENTRY
//This is the first function that is called by the host program
//The plugin returned by this function is also the plugin used by the host program
extern "C"
{
	__declspec (dllexport) IPluginBase* Register()
	{
		return new Plugin();
	}
}