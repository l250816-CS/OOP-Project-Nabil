#pragma once

#include "Enemy/BaseEnemy.hpp"
#include "Map/BaseTile.hpp"
#include "Utils/DynamicArray.hpp"

class EnemyPathTile : public BaseTile {
public:
  EnemyPathTile(float x, float y);

  /*
   * @brief Register an enemy to be in current tile
   */
  void register_enemy(std::shared_ptr<BaseEnemy>);

  /*
   * @brief Remove the attack from the current tile
   */
  void remove_enemy(std::shared_ptr<BaseEnemy>);

  /*
   * @brief Getter for enemies
   */
  DynamicArray<std::shared_ptr<BaseEnemy>> get_enemies() const;

private:
  DynamicArray<std::shared_ptr<BaseEnemy>> enemies;
};
