#include "stdafx.h"
#include "SteeringBehaviours.h"

// --------------------------------------------
// BASE CLASS IMPLEMENTATION
void SteeringBehaviour::SetTarget(const Elite::Vector2& target)
{
    m_Target = target;
}
// --------------------------------------------


// --------------------------------------------
// SEEK
SteeringPlugin_Output Seek::CalculateSteering(float deltaTime, const AgentInfo& agent)
{
    SteeringPlugin_Output steering{};
    steering.LinearVelocity = m_Target - agent.Position;
    steering.LinearVelocity.Normalize();
    steering.LinearVelocity *= agent.MaxLinearSpeed;
    steering.AutoOrient = true;

    return steering;
}
// --------------------------------------------

SteeringPlugin_Output Flee::CalculateSteering(float deltaTime, const AgentInfo& agent)
{
    SteeringPlugin_Output steering;
    steering.LinearVelocity = agent.Position - m_Target;
    steering.LinearVelocity.Normalize();
    steering.LinearVelocity *= agent.MaxLinearSpeed;
    return steering;
}

