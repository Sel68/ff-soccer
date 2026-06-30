#ifndef TEXTURE_H
#define TEXTURE_H

// IMPORTANT: glad should come first. Then GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture2D {
  friend class ResourceManager;
  friend class PostProcessor;

public:
  Texture2D();
  void Generate(unsigned int width, unsigned int height, unsigned char *data);
  void Bind() const;

private:
  unsigned int ID;
  unsigned int Width, Height;
  unsigned int Internal_Format;
  unsigned int Image_Format;
  unsigned int Wrap_S;
  unsigned int Wrap_T;
  unsigned int Filter_Min;
  unsigned int Filter_Max;

};

#endif