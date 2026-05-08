#include "Map/EnemyPathTile.hpp"
#include "Map/BaseTile.hpp"

EnemyPathTile::EnemyPathTile(float x, float y) : BaseTile(x, y, EnemyPath) {}

DynamicArray<std::shared_ptr<BaseEnemy>> EnemyPathTile::get_enemies() const {
  return enemies;
}

void EnemyPathTile::register_enemy(std::shared_ptr<BaseEnemy> enm) {
  enemies.add(enm);
}

void EnemyPathTile::remove_enemy(std::shared_ptr<BaseEnemy> enm) {
  for (std::size_t i = 0; i < enemies.size(); ++i) {
    if (enemies.get(i) == enm) {
      enemies.remove(i);
      return;
    }
  }
}
