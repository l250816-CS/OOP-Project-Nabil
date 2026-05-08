#include "Tower/BaseTower.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <queue>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include "Enemy/BaseEnemy.hpp"
#include "Game.hpp"
#include "GameSettings.hpp"
#include "Map/EnemyPathTile.hpp"
#include "Map/Map.hpp"
#include "Tower/Towers/ArcheryTower.hpp"
#include "Tower/Towers/CatapultTower.hpp"
#include "Tower/Towers/CrossbowTower.hpp"
#include "Tower/Towers/ElectroTower.hpp"
#include "Tower/Towers/OrbTower.hpp"
#include "Tower/Towers/SlingshotTower.hpp"
#include "iostream"

// TODO: check the correct initialization for the sprite in BaseTower
BaseTower::BaseTower(std::shared_ptr<BaseTile> tile, unsigned range,
                     TowerType type, int price)

    : tile(tile),
      range(range),
      type(type),
      lvl(1),          // The starting level for each tower
      shoot_rate(800)  // TODO: Elevate to subclasses
{
  // Initialize upgrades
  // TODO: Handle price decreasing : AFter thinking, handle it outside
  // Invariant: Current number of coins is already >= price
}

DynamicArray<std::shared_ptr<BaseEnemy>> BaseTower::enemies_in_range() {
  auto map = Game::get_instance().get_level()->get_map();
  if (!map) return {};

  auto start_tile = tile;
  if (!start_tile) return {};

  std::queue<std::pair<std::shared_ptr<BaseTile>, int>> to_visit;
  std::set<std::shared_ptr<BaseTile>> visited;

  to_visit.push({start_tile, 0});
  visited.insert(start_tile);

  DynamicArray<std::pair<int, std::shared_ptr<BaseEnemy>>> enemies_with_depth;

  while (!to_visit.empty()) {
    auto [current_tile, depth] = to_visit.front();
    to_visit.pop();

    if (depth > static_cast<int>(range)) continue;

    if (current_tile->get_type() == BaseTile::EnemyPath) {
      auto enemy_path_tile =
          std::dynamic_pointer_cast<EnemyPathTile>(current_tile);
      auto enemies = enemy_path_tile->get_enemies();
      for (std::size_t i = 0; i < enemies.size(); ++i) {
        enemies_with_depth.add({depth, enemies.get(i)});
      }
    }

    auto len = GameSettings::get_instance().get_tile_size();
    auto x = current_tile->get_position().y / len;
    auto y = current_tile->get_position().x / len;

    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        if (dx == 0 && dy == 0) continue;

        int neighbor_x = x + dx;
        int neighbor_y = y + dy;
        auto neighbor_tile = map->get_tile(neighbor_x, neighbor_y);
        if (neighbor_tile && visited.find(neighbor_tile) == visited.end()) {
          to_visit.push({neighbor_tile, depth + 1});
          visited.insert(neighbor_tile);
        }
      }
    }
  }

  std::sort(enemies_with_depth.begin(), enemies_with_depth.end(),
            [](const auto& a, const auto& b) {
              return a.first < b.first;
            });

  DynamicArray<std::shared_ptr<BaseEnemy>> sorted_enemies;
  for (std::size_t i = 0; i < enemies_with_depth.size(); ++i) {
    sorted_enemies.add(enemies_with_depth.get(i).second);
  }

  return sorted_enemies;
}

void BaseTower::upgrade() {
  if (get_level() < get_upgrades_count()) lvl++;
}

int BaseTower::get_upgrades_count() const {
  static std::unordered_map<BaseTower::TowerType, int> levels = {
      {BaseTower::ArcheryTower, ArcheryTower::LEVELS},
      {BaseTower::CatapultTower, CatapultTower::LEVELS},
      {BaseTower::ElectroTower, ElectroTower::LEVELS},
      {BaseTower::SlingshotTower, SlingshotTower::LEVELS},
      {BaseTower::OrbTower, OrbTower::LEVELS},
      {BaseTower::CrossbowTower, CrossbowTower::LEVELS},
  };

  if (levels.find(get_type()) == levels.end())
    throw std::runtime_error("Unidentified tower type");
  return levels[get_type()];
}

int BaseTower::get_buy_price(BaseTower::TowerType type) {
  static std::unordered_map<BaseTower::TowerType, int> prices = {
      {BaseTower::ArcheryTower, ArcheryTower::PRICE},
      {BaseTower::CatapultTower, CatapultTower::PRICE},
      {BaseTower::ElectroTower, ElectroTower::PRICE},
      {BaseTower::SlingshotTower, SlingshotTower::PRICE},
      {BaseTower::OrbTower, OrbTower::PRICE},
      {BaseTower::CrossbowTower, CrossbowTower::PRICE},
  };

  if (prices.find(type) == prices.end())
    throw std::runtime_error("Unidentified tower type");
  return prices[type];
}

int BaseTower::get_upgrade_price() {
  return get_buy_price(type) * (get_level() + 1);
}

int BaseTower::get_sell_price() {
  int base_price = get_buy_price(type);

  int upgrades_price = 0;
  for (int level = 1; level < get_level(); level++) {
    upgrades_price += get_buy_price(type) * (level + 1);
  }

  // Sell for half the amount of coins spent on building the tower and its
  // current upgrades
  return (base_price + upgrades_price) / 2;
}

BaseTower::TowerType BaseTower::get_type() const { return type; }

Vector<float> BaseTower::get_position() const { return tile->get_position(); }

int BaseTower::get_level() const { return lvl; }

unsigned BaseTower::get_range() const { return range; }

unsigned BaseTower::get_shoot_rate_ms() const { return shoot_rate; }

void BaseTower::reset_shoot_time() {
  last_shoot_time = std::chrono::steady_clock::now();
}

bool BaseTower::can_shoot() const {
  auto now = std::chrono::steady_clock::now();
  auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - last_shoot_time)
                        .count();
  return elapsed_ms >= shoot_rate;
}
