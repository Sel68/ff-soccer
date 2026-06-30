
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>

#include "GameConfig.h"
#include "SpriteRenderer.h"
#include "Texture.h"
#include "motion.h"

class GameObject {
public:
  glm::vec2 position, size, velocity;
  float rotation;
  bool lock;
  float radius;
  GameObject *owner;

  // Motion tracking
  Motion motion_library;
  Motion::GeneratedProfile currentProfile;
  double current_segment_time = 0.0;
  glm::vec2 current_target = glm::vec2(0.0f, 0.0f);
  Motion::VelocityState current_velocities{0.0, 0.0, 0.0};

  GameObject();
  GameObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite,
             bool isLock);
  GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, bool isLock,
             glm::vec3 color = glm::vec3(1.0f),
             glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

  virtual void Draw(SpriteRenderer &renderer);

  glm::vec2 Move(float dt, unsigned int window_width,
                 unsigned int window_length);
  void Reset(glm::vec2 position, glm::vec2 velocity);

private:
  glm::vec3 color;
  bool is_solid;
  Texture2D sprite;
  bool sticky, pass_through;
  bool Destroyed;
};

#endif