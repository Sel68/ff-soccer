
#ifndef ResourceManager_H
#define ResourceManager_H

#include <stb_image.h>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "Shaders.h"
#include "Texture.h"

class ResourceManager {
public:
  ResourceManager() {}
  Shader LoadShaderFromFile(const char *vShaderFile, const char *fShaderFile,
                            const char *gShaderFile = nullptr);

  Texture2D LoadTextureFromFile(const char *file, bool alpha);
  Shader LoadShader(const char *vShaderFile, const char *fShaderFile,
                    const char *gShaderFile, std::string name);
  Shader GetShader(std::string name);
  Texture2D LoadTexture(const char *file, bool alpha, std::string name);
  Texture2D GetTexture(std::string name);
  void Clear();

private:
  std::map<std::string, Shader> shaders;
  std::map<std::string, Texture2D> textures;
};

#endif