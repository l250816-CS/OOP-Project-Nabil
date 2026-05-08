#include "Enemy/EnemyManager.hpp"
#include <memory>
#include <random>
#include <stdexcept>
#include "Enemy/EnemyFactory.hpp"

void EnemyManager::assign_tickets(const BaseEnemy::EnemyType &type,
                                  const int &count) {
  tickets.remove_if([&](const auto &pair) { return pair.first == type; });
  tickets.emplace_back(type, count);
}

int EnemyManager::get_starting_wave(const BaseEnemy::EnemyType &type) const {
  auto it = starting_waves.find(type);
  return (it != starting_waves.end()) ? it->second : -1;
}

void EnemyManager::set_starting_wave(const BaseEnemy::EnemyType &type,
                                     const int &wave) {
  starting_waves[type] = wave;
}

void EnemyManager::process_enemies_states(
    std::function<void(std::shared_ptr<BaseEnemy>)> on_enemy_death,
    std::function<void(std::shared_ptr<BaseEnemy>)> on_enemy_out_of_bound) {
  for (std::size_t i = 0; i < enemies.size();) {
    auto enemy = enemies.get(i);
    switch (enemy->get_state()) {
      case BaseEnemy::ENTERING:
      case BaseEnemy::ON_BOARD:
        ++i;
        break;
      case BaseEnemy::OUT_BOUND:
        on_enemy_out_of_bound(enemy);
        enemies.remove(i);
        break;
      case BaseEnemy::DEAD:
        on_enemy_death(enemy);
        enemies.remove(i);
        break;
    }
  }
}

void EnemyManager::move_enemies() const {
  for (std::size_t i = 0; i < enemies.size(); ++i) {
    auto &enemy = enemies.get(i);
    if (enemy) enemy->move_next();
  }
}

std::shared_ptr<BaseEnemy> EnemyManager::generate_enemy(
    const int &starting_wave) {
  int total_tickets = 0;

  for (const auto &[type, count] : tickets) {
    int enemy_wave = get_starting_wave(type);
    if (enemy_wave <= starting_wave) {
      total_tickets += count;
    }
  }

  if (!total_tickets) {
    throw std::runtime_error("No tickets available to generate an enemy.");
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, total_tickets);
  int random_ticket = dis(gen);

  int cumulative = 0;
  for (const auto &[type, count] : tickets) {
    int enemy_wave = get_starting_wave(type);
    if (enemy_wave <= starting_wave) {
      cumulative += count;
      if (random_ticket <= cumulative) {
        if (starting_wave == -1) {
          throw std::runtime_error(
              "Starting wave not assigned for the enemy type.");
        }
        return EnemyFactory::get_intance().generate_enemy(type);
      }
    }
  }

  throw std::logic_error(
      "Failed to generate an enemy. This should not happen.");
}

void EnemyManager::regiseter_enemy(std::shared_ptr<BaseEnemy> enm) {
  enemies.add(std::move(enm));
}

int EnemyManager::get_enemy_count() const {
  return static_cast<int>(enemies.size());
}

void EnemyManager::clear_all_enemies() {
  // Remove all enemies from the back to avoid index issues
  while (enemies.size() > 0) {
    enemies.remove(enemies.size() - 1);
  }
}
