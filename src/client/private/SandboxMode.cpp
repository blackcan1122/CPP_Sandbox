#include "SandboxMode.h"

#if defined(_WIN32)           
#define NOGDI             // All GDI defines and routines
#define NOUSER            // All USER defines and routines
#endif
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")


#if defined(_WIN32)           // raylib uses these names as function parameters
#undef near
#undef far
#endif


#include "EventDispatcher.hpp"
#include "Physic.hpp"
#include "TickAll.h"



SandboxMode::SandboxMode()
{
	SetName("Sandbox");
    SetTargetFPS(60);
    MyRectangle = std::make_shared<BaseRectangle>();
    SetWindowSize(900, 900);
    SetWindowState(FLAG_WINDOW_RESIZABLE);


        //Dispatcher.AddListener("CollisionEvent", [MyRectangle](std::shared_ptr<Event> event)
        //{
        //    auto Test = std::dynamic_pointer_cast<CollisionEvent>(event);
        //    if (Test)
        //    {
        //        MyRectangle->OnCollision(Test);
        //    }
        //});

    // Initializing Start Position
    Vector2 Pos1 = { (GetScreenWidth()/2) - MyRectangle->GetDimensions().x, (GetScreenHeight()/2) - MyRectangle->GetDimensions().x };
    // Init Start Parameter of Objects
    MyRectangle->SetPosition(Pos1);
    MyRectangle->SetUseGravity(false);
    MyRectangle->SetIsControllable(true);


    PhysicEngineObj.CollectAllObjectsForGravity();

    EventDispatcher GlobalDispatcher;

    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    WSAStartup(MAKEWORD(2, 2), &wsa);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost
    server.sin_family = AF_INET;
    server.sin_port = htons(12345); // Port number

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == 0) {
        send(sock, "Hello, Server!", 14, 0);
        char buffer[256];
        recv(sock, buffer, sizeof(buffer), 0);
        std::cout << "Server response: " << buffer << std::endl;
    }

    closesocket(sock);
    WSACleanup();

}

SandboxMode::~SandboxMode()
{
    RuntimeCircles.clear();
    RuntimeRectangles.clear();
    PhysicEngineObj.CleanUp();
}

void SandboxMode::Update()
{
    ClearBackground(LIGHTGRAY);

    // Start Parameter for the Frame:
    float DeltaTime = GetFrameTime();
    TickAll(DeltaTime);

    DrawText(TextFormat("%f", DeltaTime), 200, 200, 16, BLUE);
    if (IsKeyPressed(KEY_A))
    {
        for (auto& Objects : RuntimeCircles)
        {
            if (helper == false)
            {
                Objects->Debugmode = true;
                std::cout << "DebugMode On" << std::endl;
            }
            else
            {
                Objects->Debugmode = false;
                std::cout << "DebugMode Off" << std::endl;
            }

        }
        if (helper == false)
        {
            helper = true;
        }
        else
        {
            helper = false;
        }
    }

    AmountOfBalls += GetMouseWheelMove();
    DrawText(TextFormat("Amount of Spawned Balls Will be: %d", AmountOfBalls), 300, 200, 16, GREEN);

    // Collision Detection for All Circles
    for (auto& Object1 : RuntimeCircles)
    {
        for (auto& Object2 : RuntimeCircles)
        {
            if (Object1 == Object2) { break; }
            if (CheckCollisionCircles(Object1->GetCenter(), Object1->GetRadius(), Object2->GetCenter(), Object2->GetRadius()))
            {
                Object1->CalculateCollision(Object2);
            }
        }
    }

    // Collision Detection for Rectangle to Circle

    for (const auto& Circle : RuntimeCircles)
    {
        if (CheckCollisionCircleRec(Circle->GetCenter(), Circle->GetRadius(), MyRectangle->GetBBox()))
        {
            Circle->CalculateCollision(MyRectangle);
        }
    }
    if (IsKeyPressed(KEY_TAB))
    {
        for (int i = 0; i < AmountOfBalls; i++)
        {
            Vector2 RandomOffset;
            RandomOffset.x = GetRandomValue(-5, 5);
             RandomOffset.y = GetRandomValue(-5, 5);
            
            RuntimeCircles.push_back(std::make_shared<BaseCircle>());
            RuntimeCircles[RuntimeCircles.size() - 1]->SetIsControllable(false);
            RuntimeCircles[RuntimeCircles.size() - 1]->SetUseGravity(true);
            RuntimeCircles[RuntimeCircles.size() - 1]->SetStartVelocity(Vector2() = {50,50});
            RuntimeCircles[RuntimeCircles.size() - 1]->SetIsBoundByScreen(true);
            RuntimeCircles[RuntimeCircles.size() - 1]->SetPosition(GetMousePosition()+ RandomOffset);
        }
        PhysicEngineObj.CollectAllObjectsForGravity();
    }
    if (IsKeyPressed(KEY_D))
    {
        for (int i = 0; i < AmountOfBalls; i++)
        {
            if (RuntimeCircles.size() > 1)
            {
                RuntimeCircles.pop_back();
            }
            PhysicEngineObj.CollectAllObjectsForGravity();
        }
    }

    DrawText(TextFormat("There are Currently %d", TickableFactory::GetTickables().size()), 500, 500, 18, RED);

    // Debug Velocity Vector
    Vector2 DebugVelVector[2];
    DebugVelVector[0] = MyRectangle->GetCenter();
    DebugVelVector[1] = MyRectangle->CalculateForwardVector();
    DrawSplineLinear(DebugVelVector, 2, 2, RED);
}

void SandboxMode::SetName(std::string NewName)
{
	Name = NewName;
}

std::string SandboxMode::GetName()
{
	return Name;
}
