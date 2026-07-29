#include "GameObject.h"

#include "GameConfig.h"

GameObject::GameObject()
    : position(0.0f, 0.0f),
      size(1.0f, 1.0f),
      velocity(0.0f),
      color(1.0f),
      charge(0.0f),
      rotation(0.0f),
      angular_velocity(0.0f),
      is_solid(false),
      Destroyed(false),
      lock(false),
      radius(12.5f),
      owner(nullptr),
      sticky(false),
      pass_through(false),
      sprite() {}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, bool isLock,
                       glm::vec3 color, glm::vec2 velocity)
    : position(pos),
      size(size),
      velocity(velocity),
      color(color),
      rotation(0.0f),
      angular_velocity(0.0f),
      sprite(sprite),
      lock(isLock),
      charge(0.0f),
      is_solid(false),
      Destroyed(false) {}

GameObject::GameObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite,
                       bool isLock)
    : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, isLock, glm::vec3(1.0f),
                 velocity) {
  this->radius = radius;
  this->owner = nullptr;
  this->sticky = false;
  this->pass_through = false;
}

void GameObject::Draw(SpriteRenderer& renderer) {
  renderer.DrawSprite(this->sprite, this->position, this->size, this->rotation, this->color);
}

glm::vec2 GameObject::Move(float dt, unsigned int window_width, unsigned int window_length) {
  if (owner == nullptr) {
    // Apply friction (linear deceleration)
    float speed = glm::length(velocity);
    if (speed > 0.0f) {
      float drop = GameConfig::ball_friction * dt;
      float newSpeed = std::max(speed - drop, 0.0f);
      velocity = velocity * (newSpeed / speed);
    }

    position += velocity * dt;

    // Wall bounces with restitution
    if (position.x <= 45.0f) {
      velocity.x = -velocity.x * GameConfig::ball_restitution;
      position.x = 45.0f;
    } else if (position.x + size.x >= window_width - 45.0f) {
      velocity.x = -velocity.x * GameConfig::ball_restitution;
      position.x = window_width - 45.0f - size.x;
    }
    if (position.y <= 30.0f) {
      velocity.y = -velocity.y * GameConfig::ball_restitution;
      position.y = 30.0f;
    } else if (position.y + size.y >= window_length - 30.0f) {
      velocity.y = -velocity.y * GameConfig::ball_restitution;
      position.y = window_length - 30.0f - size.y;
    }
  }
  return position;
}

void GameObject::Reset(glm::vec2 position, glm::vec2 velocity) {
  this->position = position;
  this->velocity = velocity;
  this->owner = nullptr;
  this->sticky = false;
  this->pass_through = false;
}