#pragma once

namespace Elite
{
	class IDecisionMaking;
	class Blackboard;
}



class DecisionModule
{
public:
	DecisionModule(Elite::Blackboard* pBlackboard);
	~DecisionModule();

	void Initialize(Elite::Blackboard* pBlackboard);
	void Update(float dt);

private:
	Elite::IDecisionMaking* m_pDecisionMaking;

};

