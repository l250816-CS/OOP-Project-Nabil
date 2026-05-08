#pragma once
#include <string>
#include "Map/BuildableTile.hpp"
#include "Utils/DynamicArray.hpp"
#include "Views/TileView.hpp"
#include "Views/TowerView.hpp"

class NonBuildableTileView : public TileView {
public:
  NonBuildableTileView(std::shared_ptr<BaseTile> tile);

private:
  static DynamicArray<std::string> sheets;
};
