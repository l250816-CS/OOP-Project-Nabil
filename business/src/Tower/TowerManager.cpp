#include "Tower/TowerManager.hpp"
#include <iostream>

void TowerManager::add_tower(std::shared_ptr<BaseTower> tower) {
  towers.add(std::move(tower));
}

void TowerManager::remove_tower(std::shared_ptr<BaseTower> tower) {
  for (std::size_t i = 0; i < towers.size(); ++i) {
    if (towers.get(i) == tower) {
      towers.remove(i);
      std::cout << "Tower removed!" << std::endl;
      return;
    }
  }
  std::cout << "Tower not found for removal!" << std::endl;
}

void TowerManager::upgrade_tower(std::shared_ptr<BaseTower> tower) {
  for (std::size_t i = 0; i < towers.size(); ++i) {
    if (towers.get(i) == tower) {
      tower->upgrade();
      std::cout << "Tower upgraded!" << std::endl;
      return;
    }
  }
  std::cout << "Tower not found for upgrade!" << std::endl;
}

int TowerManager::sell_tower(std::shared_ptr<BaseTower> tower) {
  for (std::size_t i = 0; i < towers.size(); ++i) {
    if (towers.get(i) == tower) {
      int sell_price = tower->get_sell_price();
      towers.remove(i);
      std::cout << "Tower sold for: " << sell_price << std::endl;
      return sell_price;
    }
  }

  std::cout << "Tower not found for selling!" << std::endl;
  return 0;
}
