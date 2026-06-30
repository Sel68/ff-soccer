#include "ResourceManager.h"

Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile,
                                   const char* gShaderFile, std::string name) {
  std::string vFile(vShaderFile);
  size_t pos = vFile.find_last_of("/\\");
  std::string vBaseName = (pos == std::string::npos) ? vFile : vFile.substr(pos + 1);
  std::cout << "[INFO] [ResourceManager]: Loading Shader '" << name << "' from " << vBaseName << std::endl;
  shaders[name] = LoadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
  return shaders[name];
}

Shader ResourceManager::GetShader(std::string name) { return shaders[name]; }

Texture2D ResourceManager::LoadTexture(const char* file, bool alpha, std::string name) {
  textures[name] = LoadTextureFromFile(file, alpha);
  return textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name) { return textures[name]; }

void ResourceManager::Clear() {
  for (auto& iter : shaders) {
    glDeleteProgram(iter.second.ID);
  }
  shaders.clear();
  for (auto& iter : textures) {
    glDeleteTextures(1, &iter.second.ID);
  }
  textures.clear();
}

Shader ResourceManager::LoadShaderFromFile(const char* vShaderFile, const char* fShaderFile,
                                           const char* gShaderFile) {
  std::string vertexCode;
  std::string fragmentCode;
  std::string geometryCode;

  try {
    std::ifstream vertexShaderFile(vShaderFile);
    std::ifstream fragmentShaderFile(fShaderFile);
    std::stringstream vShaderStream, fShaderStream;

    vShaderStream << vertexShaderFile.rdbuf();
    fShaderStream << fragmentShaderFile.rdbuf();

    vertexShaderFile.close();
    fragmentShaderFile.close();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    if (gShaderFile != nullptr) {
      std::ifstream geometryShaderFile(gShaderFile);
      std::stringstream gShaderStream;
      gShaderStream << geometryShaderFile.rdbuf();
      geometryShaderFile.close();
      geometryCode = gShaderStream.str();
    }
  } catch (std::exception e) {
    std::cout << "[ERROR] [ResourceManager]: Failed to read shader files (" << vShaderFile << ", " << fShaderFile << ")" << std::endl;
  }
  const char* vShaderCode = vertexCode.c_str();
  const char* fShaderCode = fragmentCode.c_str();
  const char* gShaderCode = geometryCode.c_str();

  Shader shader;
  shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
  return shader;
}

Texture2D ResourceManager::LoadTextureFromFile(const char* file, bool alpha) {
  Texture2D texture;
  if (alpha) {
    texture.Internal_Format = GL_RGBA;
    texture.Image_Format = GL_RGBA;
  }

  int width, height, nrChannels;
  unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
  std::string fileStr(file);
  size_t pos = fileStr.find_last_of("/\\");
  std::string baseName = (pos == std::string::npos) ? fileStr : fileStr.substr(pos + 1);

  if (data) {
    texture.Generate(width, height, data);
    stbi_image_free(data);
    std::cout << "[INFO] [ResourceManager]: Loaded texture '" << baseName << "' (" << width << "x" << height << ", " << nrChannels << " channels)" << std::endl;
  } else {
    std::cout << "[ERROR] [ResourceManager]: Failed to load texture '" << baseName << "'" << std::endl;
  }

  return texture;
}