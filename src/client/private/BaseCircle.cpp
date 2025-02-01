#include "BaseCircle.h"
#include "iostream"

void BaseCircle::Tick(float Deltatime)
{

	DrawCircleShape();
	if (IsControllable == true)
	{
		UseControllTransform(Deltatime);
	}
	else
	{
		CalculateNewPos(Deltatime);
	}
	if (Debugmode == true)
	{
		DrawDebugVelocity();
	}

	BoundingBox = { Position.x - Radius, Position.y - Radius, Radius * 2, Radius * 2 };
}

void BaseCircle::SetIsControllable(bool Status)
{
	this->IsControllable = Status;
}

void BaseCircle::SetIsBoundByScreen(bool Status)
{
	this->IsBoundByScreen = Status;
}

void BaseCircle::SetUseGravity(bool Status)
{
	this->GravityAffects = Status;
}

void BaseCircle::SetPosition(Vector2 NewPos)
{
	Position = NewPos;
}

Vector2 BaseCircle::CalculatePosition(Vector2& CurrentPos, Vector2& Velocity, float& Dampening, Vector2& Accel, const float Deltatime)
{
	Vector2 ReturnArray;
	ReturnArray = Vector2Add(Velocity, Accel);

	ReturnArray.x *= Dampening;
	ReturnArray.y *= Dampening;

	ReturnArray = Vector2Add(Position, Vector2Scale(Velocity,Deltatime));

	
	return ReturnArray;

}

bool BaseCircle::GetGravityAffected()
{
	return GravityAffects;
}

Vector2 BaseCircle::GetVelocity()
{
	return Velocity;
}

void BaseCircle::CalculateGravity(float Gravity, float Deltatime)
{
	if ((this->GetPosition().y + Radius) > static_cast<int>(GetScreenHeight()) && IsBoundByScreen)
	{
		Vector2 Pos = this->GetPosition();
		Pos.y -= (Pos.y - static_cast<int>(GetScreenHeight()) + Radius);
		SetPosition(Pos);
		Vector2 Helper = { 0,-1 };
		Vector2 ReflectVector;
		ReflectVector = (Vector2Reflect(Velocity, Helper));
		ReflectVector = Vector2Scale(ReflectVector, 1);
		Velocity = ReflectVector;
	}
	if ((this->GetPosition().x + Radius) > static_cast<int>(GetScreenWidth()) && IsBoundByScreen)
	{
		Vector2 Pos = this->GetPosition();
		Pos.x -= (Pos.x - static_cast<int>(GetScreenWidth()) + Radius);
		SetPosition(Pos);
		Vector2 Helper = { -1,0 };
		Vector2 ReflectVector;
		ReflectVector = (Vector2Reflect(Velocity, Helper));
		ReflectVector = Vector2Scale(ReflectVector, 1);
		Velocity = ReflectVector;
	}
	if ((this->GetPosition().x - Radius) < 0 && IsBoundByScreen)
	{
		Vector2 Pos = this->GetPosition();
		Pos.x -= (Pos.x - 0 - Radius);
		SetPosition(Pos);
		Vector2 Helper = { 1,0 };
		Vector2 ReflectVector;
		ReflectVector = (Vector2Reflect(Velocity, Helper));
		ReflectVector = Vector2Scale(ReflectVector, 1);
		Velocity = ReflectVector;
	}

	Velocity.y += Gravity;
	Velocity.y *= (m_Drag * Dampening);


	CalculateNewPos(Deltatime);
}


void BaseCircle::CalculateNewPos(float Deltatime)
{
	// Update velocity first
	Velocity.x *= Dampening;
	Velocity.y *= Dampening;

	// Velocity threshold
	const float velocityThreshold = 7.0f;
	if (std::abs(Velocity.x) < velocityThreshold) { Velocity.x = 0; }
	if (std::abs(Velocity.y) < velocityThreshold) { Velocity.y = 0; }

	// Update position using new velocity
	Vector2 NewPos = GetPosition();
	NewPos.x += Velocity.x * Deltatime;
	NewPos.y += Velocity.y * Deltatime;

	// Screen boundary collision
	if (IsBoundByScreen)
	{
		const float bounce = 1.f; // Coefficient of restitution for walls

		if (NewPos.y - Radius < 0)
		{
			NewPos.y = Radius;
			Velocity.y = -Velocity.y * bounce;
		}
		else if (NewPos.y + Radius > static_cast<float>(GetScreenHeight()))
		{
			NewPos.y = static_cast<float>(GetScreenHeight()) - Radius;
			Velocity.y = -Velocity.y * bounce;
		}

		if (NewPos.x - Radius < 0)
		{
			NewPos.x = Radius;
			Velocity.x = -Velocity.x * bounce;
		}
		else if (NewPos.x + Radius > static_cast<float>(GetScreenWidth()))
		{
			NewPos.x = static_cast<float>(GetScreenWidth()) - Radius;
			Velocity.x = -Velocity.x * bounce;
		}
	}
	SetPosition(NewPos);
}

void BaseCircle::UseControllTransform(float Deltatime)
{
		// Accelarting
		Accel = 30.f; // Magic Accel Number
		if (IsKeyDown(KEY_UP))
		{
			Velocity.y -= Accel;
		}
		if (IsKeyDown(KEY_DOWN))
		{
			Velocity.y += Accel;
		}
		if (IsKeyDown(KEY_RIGHT))
		{
			Velocity.x += Accel;
		}
		if (IsKeyDown(KEY_LEFT))
		{
			Velocity.x -= Accel;
		}

		// Apply Dampening
		Velocity.x *= Dampening;
		Velocity.y *= Dampening;

		// Update position with velocity

		Position.x += Velocity.x * Deltatime;
		Position.y += Velocity.y * Deltatime;

		// Check if velocity is close to zero and set it to zero if so
		if (std::abs(Velocity.x) < 0.02)
		{
			Velocity.x = 0;
		}
		if (std::abs(Velocity.y) < 0.02)
		{
			Velocity.y = 0;
		}

		DrawText(TextFormat("Velocity X: %f, Velocity Y:%f", Velocity.x, Velocity.y), 50, 50, 12, BLACK);
	
}

void BaseCircle::DrawCircleShape()
{
	DrawCircle(Position.x, Position.y, Radius, FColor);
}

void BaseCircle::SetRadius(float NewRadius)
{
	Radius = NewRadius;
}


float BaseCircle::GetRadius()
{
	return Radius;
}


void BaseCircle::OnCollision(std::shared_ptr<CollisionEvent> event)
{
	if (event)
	{
		// Need to be implement for circles
	}
}

void BaseCircle::DrawDebugVelocity()
{
	Vector2 DebugVelVector[2];
	DebugVelVector[0] = this->GetCenter();
	DebugVelVector[1] = this->CalculateForwardVector();
	DrawSplineLinear(DebugVelVector, 2, 2, RED);
}

Vector2 BaseCircle::GetNormalizedVelocity()
{
	Vector2 NormalizedVector = Vector2Normalize(Velocity);
	return NormalizedVector;
}

float BaseCircle::GetMagnitudeVelocity()
{
	float Magnitude = std::sqrt((Velocity.x * Velocity.x) + (Velocity.y * Velocity.y));
	return Magnitude;
}

Vector2 BaseCircle::CalculateForwardVector()
{
	Vector2 NormalizedVelocity = GetNormalizedVelocity();
	Vector2 ForwardVector = Vector2Scale(NormalizedVelocity, this->GetMagnitudeVelocity() * 0.25f);


	ForwardVector = Vector2Add(ForwardVector, this->GetCenter());

	return ForwardVector;
}

void BaseCircle::CalculateCollision(std::shared_ptr<Tickable> CollisionObject)
{
	if (!CollisionObject)
	{
		std::cerr << "Collision Object is Nullptr\n";
		return;
	}

	// Case when our Collision Object is derived from BaseRectangle
	if (std::dynamic_pointer_cast<BaseRectangle>(CollisionObject))
	{
		std::shared_ptr<BaseRectangle> CollObject = std::dynamic_pointer_cast<BaseRectangle>(CollisionObject);

		Vector2 circleCenter = this->GetCenter();
		Vector2 rectCenter = CollObject->GetCenter();
		Rectangle rect = CollObject->GetBBox();

		// Find closest point on rectangle to circle
		Vector2 closestPoint = {
			std::clamp(circleCenter.x, rect.x, rect.x + rect.width),
			std::clamp(circleCenter.y, rect.y, rect.y + rect.height)
		};

		// Calculate collision normal and distance
		Vector2 normal = Vector2Subtract(circleCenter, closestPoint);
		float distance = Vector2Length(normal);

		if (distance > this->GetRadius()) {
			return;
		}

		// Normalize the collision normal
		if (distance > 0) {
			normal = Vector2Scale(normal, 1.0f / distance);
		}
		else {
			normal = { 0, -1 }; // Default push direction if no velocity
		}

		// Calculate relative velocity
		Vector2 relativeVel = Vector2Subtract(this->Velocity, CollObject->GetVelocity());
		float velAlongNormal = Vector2DotProduct(relativeVel, normal);

		// Determine if the circle is resting on the rectangle
		const float restingThreshold = 0.1f;  // Adjust this value as needed
		bool isResting = std::abs(velAlongNormal) < restingThreshold;

		if (isResting) {
			// When resting, apply a portion of the rectangle's velocity
			float platformInfluence = 0.8f;  // How much the platform movement affects the circle
			Vector2 platformVel = CollObject->GetVelocity();

			// Only apply horizontal movement from platform
			this->Velocity.x = platformVel.x * platformInfluence;

			// Apply minimal vertical velocity to keep contact
			if (normal.y < 0) {  // Only if circle is on top
				this->Velocity.y = std::min(this->Velocity.y, platformVel.y);
			}
		}
		else {
			// Normal collision response for non-resting case
			float e = 0.7f;  // Restitution coefficient
			float j = -(1.0f + e) * velAlongNormal;

			Vector2 impulse = Vector2Scale(normal, j);
			this->Velocity = Vector2Add(this->Velocity, impulse);

			// Reduced friction for non-resting case
			Vector2 tangent = { -normal.y, normal.x };
			float velAlongTangent = Vector2DotProduct(relativeVel, tangent);
			float frictionCoef = 0.1f;  // Lower friction when not resting
			Vector2 frictionImpulse = Vector2Scale(tangent, -velAlongTangent * frictionCoef);
			this->Velocity = Vector2Add(this->Velocity, frictionImpulse);
		}

		// Position correction
		float penetration = this->GetRadius() - distance;
		const float percent = 0.8f;
		const float slop = 0.01f;

		if (penetration > slop)
		{
			Vector2 correction = Vector2Scale(normal, penetration * percent);
			this->SetPosition(Vector2Add(this->GetPosition(), correction));
		}
		
	}

	
	// Case when our Collision Object is derived from BaseCircle
	if (std::dynamic_pointer_cast<BaseCircle>(CollisionObject))
	{
		std::shared_ptr<BaseCircle> CollObject = std::dynamic_pointer_cast<BaseCircle>(CollisionObject);

		Vector2 DirectionToOtherObject = Vector2Subtract(this->GetCenter(), CollObject->GetCenter());
		float Distance = Vector2Length(DirectionToOtherObject);
		float RadiusSum = this->GetRadius() + CollObject->GetRadius();

		//// Normalize the collision normal
		Vector2 Normal = Vector2Scale(DirectionToOtherObject, 1.0f / Distance);

		// Early exit if not colliding
		// Should never Happen
		if (Distance >= RadiusSum) {
			return;
		}

		// Position correction to prevent sinking
		float penetration = RadiusSum - Distance;
		const float percent = 0.8f;  // Correction percentage
		const float slop = 0.01f;    // Penetration allowance
		float e = 0.7f;

		
		if (penetration > slop) {
			Vector2 correction = Vector2Scale(Normal, penetration * percent);
			// Distribute correction based on combined mass
			Vector2 thisCorrection = Vector2Scale(correction, this->Mass / (this->Mass + CollObject->Mass));
			Vector2 otherCorrection = Vector2Scale(correction, CollObject->Mass / (this->Mass + CollObject->Mass));

			this->SetPosition(Vector2Add(this->GetPosition(), thisCorrection));
			CollObject->SetPosition(Vector2Subtract(CollObject->GetPosition(), otherCorrection));
		}

		// Calculate relative velocity
		Vector2 RelativeVelocity = Vector2Subtract(this->Velocity, CollObject->Velocity);

		// Calculate velocity along the normal
		float VelAlongNormal = Vector2DotProduct(RelativeVelocity, Normal);

		// Don't resolve if objects are moving apart
		if (VelAlongNormal < 0) {
			float j = -(1 + e) * VelAlongNormal / (this->Mass + CollObject->Mass);

			Vector2 impulseThis = Vector2Scale(Normal, j * this->Mass);
			Vector2 impulseOther = Vector2Scale(Normal, j * CollObject->Mass);

			this->Velocity = Vector2Add(this->Velocity, Vector2Scale(impulseThis, Dampening / this->Mass));
			CollObject->Velocity = Vector2Subtract(CollObject->Velocity, Vector2Scale(impulseOther, Dampening / CollObject->Mass));
		}

	}

}

void BaseCircle::UseDampening(bool UseDampening)
{
	if (UseDampening)
	{
		Dampening = 0.9f;
		return;
	}
	
	Dampening = 1.f;
}

void BaseCircle::UseDrag(bool Drag)
{
	m_Drag = Drag;
}

void BaseCircle::SetStartVelocity(Vector2 StartVelocity)
{
	Velocity = StartVelocity;
}

Rectangle BaseCircle::GetBBox()
{
	return BoundingBox;
}

Vector2 BaseCircle::GetCenter()
{
	float Radius = this->GetRadius();
	Vector2 OffsettedPos = { GetPosition().x , GetPosition().y  };
	return OffsettedPos;
	
}


