#include "GameObject.h"

#include "MotionConfig.h"

GameObject::GameObject()
    : Position(0.0f, 0.0f),
      Size(1.0f, 1.0f),
      Velocity(0.0f),
      Color(1.0f),
      Rotation(0.0f),
      Sprite(),
      is_solid(false),
      Destroyed(false) {}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, bool isLock,
                       glm::vec3 color, glm::vec2 velocity)
    : Position(pos),
      Size(size),
      Velocity(velocity),
      Color(color),
      Rotation(0.0f),
      Sprite(sprite),
      lock(isLock),
      is_solid(false),
      Destroyed(false) {}

GameObject::GameObject() : Radius(12.5f), Owner(nullptr), Sticky(false), PassThrough(false) {}

GameObject::GameObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite,
                       bool isLock)
    : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, isLock, glm::vec3(1.0f),
                 velocity),
      Radius(radius),
      Owner(nullptr),
      Sticky(false),
      PassThrough(false) {}

void GameObject::Draw(SpriteRenderer& renderer) {
  renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

glm::vec2 GameObject::Move(float dt, unsigned int window_width, unsigned int window_length) {
  if (Owner == nullptr) {
    // Apply friction (linear deceleration)
    float speed = glm::length(Velocity);
    if (speed > 0.0f) {
      float drop = MotionConfig::BALL_FRICTION * dt;
      float newSpeed = std::max(speed - drop, 0.0f);
      Velocity = Velocity * (newSpeed / speed);
    }

    Position += Velocity * dt;

    // Wall bounces with restitution
    if (Position.x <= 45.0f) {
      Velocity.x = -Velocity.x * MotionConfig::BALL_RESTITUTION;
      Position.x = 45.0f;
    } else if (Position.x + Size.x >= window_width - 45.0f) {
      Velocity.x = -Velocity.x * MotionConfig::BALL_RESTITUTION;
      Position.x = window_width - 45.0f - Size.x;
    }
    if (Position.y <= 30.0f) {
      Velocity.y = -Velocity.y * MotionConfig::BALL_RESTITUTION;
      Position.y = 30.0f;
    } else if (Position.y + Size.y >= window_length - 30.0f) {
      Velocity.y = -Velocity.y * MotionConfig::BALL_RESTITUTION;
      Position.y = window_length - 30.0f - Size.y;
    }
  }
  return Position;
}

void GameObject::Reset(glm::vec2 position, glm::vec2 velocity) {
  this->Position = position;
  this->Velocity = velocity;
  this->Owner = nullptr;
  this->Sticky = false;
  this->PassThrough = false;
}