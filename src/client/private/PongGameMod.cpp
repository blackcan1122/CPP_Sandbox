#include "PongGameMod.h"

#include <set>

#include "TickAll.h"
#include <random>

PongGameMod::PongGameMod()
{
    SetName("PongGame");
    PointsPlayer = 0;
    PointsEnemy = 0;
    AmountOfBalls = 1;

    SetTargetFPS(60);


    PlayerOne = std::make_shared<BaseRectangle>();
    PlayerTwo = std::make_shared<BaseRectangle>();
    Ball = std::make_shared<BaseCircle>();

    Reset();

     TempContainer = {PlayerOne,PlayerTwo};


}

PongGameMod::~PongGameMod()
{
}

void PongGameMod::Update()
{
    ClearBackground(GRAY);
    m_DeltaTime = GetFrameTime();
    DrawFPS(100, 100);

    if (Wait == true)
    {
        waitTimeElapsed += m_DeltaTime;
        if (waitTimeElapsed >= 3.0f) // Pause for 2 seconds
        {
            Wait = false; // Resume game logic
            waitTimeElapsed = 0.0f;
        }
        else
        {

            TickAll(0); // Pass 0 as delta time to stop all game object updates
            int textWidth = MeasureText(TextFormat("%d ", (int)(4.f - waitTimeElapsed)), 64);
            DrawText(TextFormat("%d ", (int)(4.f - waitTimeElapsed)), (GetScreenWidth() - textWidth) / 2, GetScreenHeight() / 2, 64, GREEN);
            return;
        }
    }

    waitTimeElapsed += m_DeltaTime;
    TickAll(m_DeltaTime);

    //Vector2 PlayerTwoPos = { GetScreenWidth() - 100, Ball->GetPosition().y - (PlayerTwo->GetDimensions().y/2)};
    //
    //Vector2 NewPlayerPos = Vector2Lerp(PlayerTwo->GetPosition(), PlayerTwoPos, RandomDifficulty + m_DeltaTime * 0.1f);

    //PlayerTwo->SetPosition(NewPlayerPos);

    float predictedBallY = PredictBallY(Ball->GetPosition(), Ball->GetVelocity(), PlayerTwo->GetPosition().x);

    if (PlayerTwo->GetPosition().y > predictedBallY) {
        PlayerTwo->FakeInput(KEY_UP, m_DeltaTime);
    }
    else {
        PlayerTwo->FakeInput(KEY_DOWN, m_DeltaTime);
    }




    for (const auto& Rec : TempContainer)
    {
        if (CheckCollisionRecs(Rec->GetBBox(), Ball->GetBBox()))
        {
            Ball->CalculateCollision(Rec);
        }
    }

    float TargetSpeed = 500.f + (waitTimeElapsed * 10);

    if (Ball->GetMagnitudeVelocity() < TargetSpeed)
    {
        Vector2 NewVelocity = Vector2Scale(Ball->GetNormalizedVelocity(), TargetSpeed);
        Ball->SetStartVelocity(NewVelocity);
    }

    if (abs(Ball->GetNormalizedVelocity().x) < 0.25)
    {
        Vector2 BiasX;

        if (Ball->GetPosition().x > GetScreenWidth())
        {
            BiasX = { 0.55,0 };
        }
        else
        {
            BiasX = { -0.55,0 };
        }
        Vector2 Vector2Bias = Vector2Add(Ball->GetNormalizedVelocity(), BiasX);
        Vector2 Velocity = Vector2Scale(Vector2Bias, Ball->GetMagnitudeVelocity());

        Ball->SetStartVelocity(Velocity);
      
    }

    if (Ball->GetPosition().x == (GetScreenWidth() - Ball->GetRadius()) || Ball->GetPosition().x == (0 + Ball->GetRadius()))
    {
        Reset();
    }

    if (Ball->GetPosition().x > GetScreenWidth() || Ball->GetPosition().x < 0)
    {
        Reset();
    }



    
}

void PongGameMod::SetName(std::string NewName)
{
    GameMode::SetName(NewName);
}

void PongGameMod::Reset()
{

    std::random_device rd;
    std::mt19937 RandomGenerator(rd()); // Seed with a random device
    std::uniform_real_distribution<float> FloatValueForDifficulty(0.6f, 0.90f);
    std::uniform_real_distribution<float> FloatValueX(-100.f, 100.f);
    std::uniform_real_distribution<float> FloatValueY(-100.f, 100.f);

    //PlayerOne
    PlayerOne->RestrictAxis('Y');
    PlayerOne->SetUseGravity(false);
    PlayerOne->SetIsControllable(true);
    PlayerOne->SetDimensions(Vector2{ 50,150 });
    PlayerOne->SetColor(GREEN);
    PlayerOne->SetPosition(Vector2{ 50, (float)(GetScreenHeight() / 2 - (PlayerOne->GetDimensions().y / 2)) });
    PlayerOne->SetIsBoundByScreen(true);

    // Player Two 
    PlayerTwo->RestrictAxis('Y');
    PlayerTwo->SetUseGravity(false);
    PlayerTwo->SetIsControllable(false);
    PlayerTwo->SetDimensions(Vector2{ 50, 150 });
    PlayerTwo->SetColor(GREEN);
    float StartPosPT = GetScreenWidth() - 100;
    PlayerTwo->SetPosition(Vector2{ (float)GetScreenWidth() - 100, (float)(GetScreenHeight() / 2 - (PlayerTwo->GetDimensions().y / 2)) });
    PlayerTwo->SetIsBoundByScreen(true);
    Ball->SetUseGravity(false);

    // Ball
    Ball->SetUseGravity(false);
    Ball->SetIsBoundByScreen(true);
    Ball->UseDrag(false);
    Ball->UseDampening(false);
    Vector2 BallPos;
    BallPos.x = GetScreenWidth() / 2 + 10;
    BallPos.y = GetScreenHeight() / 2;
    Ball->SetPosition(BallPos);

    Vector2 InitialVelocity = { FloatValueX(RandomGenerator), FloatValueY(RandomGenerator) };
    if (Vector2Length(InitialVelocity) < 25.f) 
    {
        InitialVelocity = Vector2Scale(Vector2Normalize(InitialVelocity), 25.f);
    }
    Ball->SetStartVelocity(InitialVelocity);

    Wait = true;
    waitTimeElapsed = 0;



    RandomDifficulty = FloatValueForDifficulty(RandomGenerator);
}

std::string PongGameMod::GetName()
{
    return GameMode::GetName();
}

float PongGameMod::PredictBallY(Vector2 ballPosition, Vector2 ballVelocity, float paddleX)
{
    // Ensure the ball is moving towards the AI paddle
    if (ballVelocity.x <= 0) {
        return ballPosition.y; // Ball is moving away; return current position
    }

    float predictedY = ballPosition.y;

    // Simulate the ball's movement
    while (ballPosition.x < paddleX) {
        // Update ball position
        ballPosition.x += ballVelocity.x * GetFrameTime();
        ballPosition.y += ballVelocity.y * GetFrameTime();


        if (ballPosition.y <= 0) 
        {
            ballPosition.y = -ballPosition.y;
            ballVelocity.y *= -1;            
        }

        else if (ballPosition.y >= GetScreenHeight())  
        {
            ballPosition.y = 2 * GetScreenHeight() - ballPosition.y;
            ballVelocity.y *= -1;
        }

        // Stop if the ball goes off-screen (failsafe)
        if (ballPosition.x < 0 || ballPosition.y < 0 || ballPosition.y > GetScreenHeight()) {
            break;
        }
    }

    return ballPosition.y;
}
