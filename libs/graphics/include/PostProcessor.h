#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glm/glm.hpp>

#include "Shaders.h"
#include "SpriteRenderer.h"
#include "Texture.h"

class PostProcessor {
public:
  PostProcessor(Shader shader, unsigned int width, unsigned int height);
  void BeginRender();
  void EndRender();
  void Render(float time);

private:
  unsigned int MSFBO, FBO;
  unsigned int RBO;
  unsigned int VAO;
  void initRenderData();
  Shader PostProcessingShader;
  Texture2D Texture;
  unsigned int Width, Height;
  bool Confuse, Chaos, Shake;
};

#endif