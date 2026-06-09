#include <vector>

#include "GameObject.h"
#include "ResourceManager.h"

class GameLevel {
 public:
  std::vector<GameObject> Bricks;
  GameLevel() {}

  void Load(ResourceManager& resource_manager, const char* file, unsigned int levelWidth,
            unsigned int levelHeight);

  void Draw(SpriteRenderer& renderer);

  bool IsCompleted();

 private:
  void init(ResourceManager& resource_manager, std::vector<std::vector<unsigned int>> tileData,
            unsigned int levelWidth, unsigned int levelHeight);
};