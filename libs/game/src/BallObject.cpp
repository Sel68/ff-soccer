#include "BallObject.h"

BallObject::BallObject() 
    : GameObject(), Radius(12.5f), Stuck(true), Sticky(false), PassThrough(false)  { }

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite, bool isLock)
    : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, isLock, glm::vec3(1.0f), velocity), Radius(radius), Stuck(true), Sticky(false), PassThrough(false) { }

glm::vec2 BallObject::Move(float dt, unsigned int window_width, unsigned int window_length)
{
    if (!Stuck)
    {
        Position += Velocity * dt;
        if (Position.x <= 45.0f)
        {
            Velocity.x = -Velocity.x;
            Position.x = 45.0f;
        }
        else if (Position.x + Size.x >= window_width - 45.0f)
        {
            Velocity.x = -Velocity.x;
            Position.x = window_width - 45.0f - Size.x;
        }
        if (Position.y <= 30.0f)
        {
            Velocity.y = -Velocity.y;
            Position.y = 30.0f;
        }
        else if (Position.y + Size.y >= window_length - 30.0f)
        {
            Velocity.y = -Velocity.y;
            Position.y = window_length - 30.0f - Size.y;
        }
    }
    return Position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
    this->Position = position;
    this->Velocity = velocity;
    this->Stuck = true;
    this->Sticky = false;
    this->PassThrough = false;
}