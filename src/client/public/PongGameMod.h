#pragma once
#include "Core.h"
#include "GameMode.h"
#include "BaseRectangle.h"
#include "BaseCircle.h"
#include "Tickable.h"
#include "Physic.hpp"
#include "EventDispatcher.hpp"

class PongGameMod : public  GameMode
{
public:
    PongGameMod();
    ~PongGameMod() override;

    void Update() override;
    void SetName(std::string NewName) override;
    void Reset();
    std::string GetName() override;

protected:
    float m_DeltaTime;

private:

    float PredictBallY(Vector2 BallPosition, Vector2 BallVelocity, float PaddleX);

    std::string Name;
    std::shared_ptr<BaseRectangle> PlayerOne;
    std::shared_ptr<BaseRectangle> PlayerTwo;
    std::shared_ptr<BaseCircle> Ball;
    int AmountOfBalls;

    int PointsPlayer;
    int PointsEnemy;

    bool Wait = true;
    float waitTimeElapsed = 0.0f;

    float RandomDifficulty;
    // Creating PhysicEngineObject
    PhysicEngine PhysicEngineObj;

    std::vector<std::shared_ptr<BaseRectangle>> TempContainer;

    EventDispatcher Dispatcher;
};
