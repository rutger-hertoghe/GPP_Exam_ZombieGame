#include "stdafx.h"
#include "SteeringBehaviours.h"

// --------------------------------------------
// BASE CLASS IMPLEMENTATION
void SteeringBehaviour::SetTarget(const Elite::Vector2& target)
{
    m_Target = target;
}
const Elite::Vector2& SteeringBehaviour::GetTarget()
{
    return m_Target;
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


// --------------------------------------------
// FLEE
SteeringPlugin_Output Flee::CalculateSteering(float deltaTime, const AgentInfo& agent)
{
    SteeringPlugin_Output steering;
    steering.LinearVelocity = agent.Position - m_Target;
    steering.LinearVelocity.Normalize();
    steering.LinearVelocity *= agent.MaxLinearSpeed;

    if (m_FaceTarget)
    {
        steering.AutoOrient = false;
        float desiredOrientation{ atan2f(-steering.LinearVelocity.y, -steering.LinearVelocity.x) };
        constexpr float toleranceValue{ 0.01f };
        if (abs(agent.Orientation - desiredOrientation) < toleranceValue)
        {
            steering.AngularVelocity = 0.f;
        }
        else if (agent.Orientation < desiredOrientation)
        {
            steering.AngularVelocity = agent.MaxAngularSpeed;
        }
        else if (agent.Orientation > desiredOrientation)
        {
            steering.AngularVelocity = -agent.MaxAngularSpeed;
        }
    }

    return steering;
}

void Flee::SetFaceTarget(bool faceTarget)
{
    m_FaceTarget = faceTarget;
}
// --------------------------------------------


// --------------------------------------------
// WANDER
SteeringPlugin_Output Wander::CalculateSteering(float deltaTime, const AgentInfo& agent)
{
    
    const float agentRotation{ agent.Orientation };
    Elite::Vector2 movementVector{ cosf(agentRotation), sinf(agentRotation) };
    movementVector.Normalize();
    Elite::Vector2 circleCenter{ agent.Position + movementVector * m_Offset };
    const float randomAngle{ GenerateRandomAngle(1000.f) };
    m_WanderAngle += randomAngle;
    Elite::Vector2 chosenTarget{ circleCenter.x + m_Radius * cosf(m_WanderAngle), circleCenter.y + m_Radius * sinf(m_WanderAngle) };

    m_Target = chosenTarget;

    SteeringPlugin_Output steering{ Seek::CalculateSteering(deltaTime, agent) };
    steering.AutoOrient = true;
    return steering;
}

float Wander::GenerateRandomAngle(float precisionScaling) const
{
    const int inputRandomFloat{ static_cast<int>(m_MaxAngleChange * precisionScaling) };
    return static_cast<float>((rand() % inputRandomFloat) - inputRandomFloat / 2) / precisionScaling;
}
// --------------------------------------------


// --------------------------------------------
// SEEK AND SCAN
SteeringPlugin_Output SeekAndScan::CalculateSteering(float deltaTime, const AgentInfo& agent)
{
    SteeringPlugin_Output steering{ Seek::CalculateSteering(deltaTime, agent) };
    steering.AutoOrient = false;
    steering.AngularVelocity = agent.MaxAngularSpeed;
    return steering;
}
// --------------------------------------------

SteeringPlugin_Output SprintSeek::CalculateSteering(float deltaTime, const AgentInfo& agent)
{
    SteeringPlugin_Output steering{ Seek::CalculateSteering(deltaTime, agent) };
    steering.RunMode = true;
    return steering;
}
