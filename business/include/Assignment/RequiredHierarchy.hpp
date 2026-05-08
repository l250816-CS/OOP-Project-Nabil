#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include "Utils/DynamicArray.hpp"

struct TDPosition {
  float x = 0.f;
  float y = 0.f;

  bool operator==(const TDPosition& other) const {
    return x == other.x && y == other.y;
  }
};

class Entity {
public:
  virtual ~Entity() = default;
  virtual void update(float delta_time) = 0;
  virtual void render(sf::RenderWindow& window) = 0;
};

class Enemy : public Entity {
public:
  virtual ~Enemy() override = default;

  virtual void move(float delta_time) = 0;
  virtual void takeDamage(float amount) = 0;

  bool isAlive() const { return hp > 0.f; }
  TDPosition getPosition() const { return position; }
  float getHP() const { return hp; }

protected:
  Enemy(float hp, float speed, int reward, TDPosition spawn, TDPosition target)
      : hp(hp), speed(speed), reward(reward), position(spawn), target(target) {}

protected:
  float hp;
  float speed;
  int reward;
  TDPosition position;
  TDPosition target;
};

class Tower : public Entity {
public:
  virtual ~Tower() override = default;
  virtual void attack(DynamicArray<Enemy*>& enemies, float delta_time) = 0;

  TDPosition getPosition() const { return position; }

protected:
  Tower(float range, float damage, float fire_rate, TDPosition position)
      : range(range), damage(damage), fire_rate(fire_rate), position(position) {}

protected:
  float range;
  float damage;
  float fire_rate;
  float cooldown = 0.f;
  TDPosition position;
};

class BasicEnemy : public Enemy {
public:
  BasicEnemy(TDPosition spawn, TDPosition target)
      : Enemy(100.f, 60.f, 10, spawn, target) {}

  void update(float delta_time) override { move(delta_time); }

  void move(float delta_time) override {
    const float dx = target.x - position.x;
    const float dy = target.y - position.y;
    const float dist = std::sqrt(dx * dx + dy * dy);
    if (dist <= 1.f) return;

    position.x += (dx / dist) * speed * delta_time;
    position.y += (dy / dist) * speed * delta_time;
  }

  void takeDamage(float amount) override { hp -= amount; }

  void render(sf::RenderWindow& window) override {
    sf::CircleShape shape(10.f);
    shape.setFillColor(sf::Color(180, 60, 60));
    shape.setPosition(position.x - 10.f, position.y - 10.f);
    window.draw(shape);
  }
};

class FastEnemy : public Enemy {
public:
  FastEnemy(TDPosition spawn, TDPosition target)
      : Enemy(70.f, 95.f, 12, spawn, target) {}

  void update(float delta_time) override { move(delta_time); }

  void move(float delta_time) override {
    const float dx = target.x - position.x;
    const float dy = target.y - position.y;
    const float dist = std::sqrt(dx * dx + dy * dy);
    if (dist <= 1.f) return;

    position.x += (dx / dist) * speed * delta_time;
    position.y += (dy / dist) * speed * delta_time;
  }

  void takeDamage(float amount) override { hp -= amount; }

  void render(sf::RenderWindow& window) override {
    sf::CircleShape shape(8.f);
    shape.setFillColor(sf::Color(210, 140, 70));
    shape.setPosition(position.x - 8.f, position.y - 8.f);
    window.draw(shape);
  }
};

class TankEnemy : public Enemy {
public:
  TankEnemy(TDPosition spawn, TDPosition target)
      : Enemy(260.f, 35.f, 20, spawn, target) {}

  void update(float delta_time) override { move(delta_time); }

  void move(float delta_time) override {
    const float dx = target.x - position.x;
    const float dy = target.y - position.y;
    const float dist = std::sqrt(dx * dx + dy * dy);
    if (dist <= 1.f) return;

    position.x += (dx / dist) * speed * delta_time;
    position.y += (dy / dist) * speed * delta_time;
  }

  void takeDamage(float amount) override { hp -= amount; }

  void render(sf::RenderWindow& window) override {
    sf::CircleShape shape(13.f);
    shape.setFillColor(sf::Color(130, 92, 65));
    shape.setPosition(position.x - 13.f, position.y - 13.f);
    window.draw(shape);
  }
};

class FlyingEnemy : public Enemy {
public:
  FlyingEnemy(TDPosition spawn, TDPosition target)
      : Enemy(85.f, 80.f, 16, spawn, target) {}

  void update(float delta_time) override { move(delta_time); }

  void move(float delta_time) override {
    const float dx = target.x - position.x;
    const float dy = target.y - position.y;
    const float dist = std::sqrt(dx * dx + dy * dy);
    if (dist <= 1.f) return;

    position.x += (dx / dist) * speed * delta_time;
    position.y += (dy / dist) * speed * delta_time;
  }

  void takeDamage(float amount) override { hp -= amount; }

  void render(sf::RenderWindow& window) override {
    sf::CircleShape shape(9.f, 3);
    shape.setFillColor(sf::Color(150, 150, 180));
    shape.setPosition(position.x - 9.f, position.y - 9.f);
    window.draw(shape);
  }
};

class BurrowEnemy : public Enemy {
public:
  BurrowEnemy(TDPosition spawn, TDPosition target)
      : Enemy(120.f, 55.f, 18, spawn, target) {}

  void update(float delta_time) override {
    burrow_timer += delta_time;
    move(delta_time);
  }

  void move(float delta_time) override {
    const float dx = target.x - position.x;
    const float dy = target.y - position.y;
    const float dist = std::sqrt(dx * dx + dy * dy);
    if (dist <= 1.f) return;

    const float speed_multiplier = (burrow_timer > 1.8f && burrow_timer < 2.4f) ? 1.8f : 1.f;
    if (burrow_timer >= 2.4f) burrow_timer = 0.f;

    position.x += (dx / dist) * speed * speed_multiplier * delta_time;
    position.y += (dy / dist) * speed * speed_multiplier * delta_time;
  }

  void takeDamage(float amount) override { hp -= amount; }

  void render(sf::RenderWindow& window) override {
    sf::CircleShape shape(10.f);
    shape.setFillColor(sf::Color(100, 76, 52));
    shape.setPosition(position.x - 10.f, position.y - 10.f);
    window.draw(shape);
  }

private:
  float burrow_timer = 0.f;
};

class CannonTower : public Tower {
public:
  explicit CannonTower(TDPosition position)
      : Tower(120.f, 40.f, 0.8f, position) {}

  void update(float delta_time) override { cooldown += delta_time; }

  void attack(DynamicArray<Enemy*>& enemies, float delta_time) override {
    cooldown += delta_time;
    const float period = 1.f / fire_rate;
    if (cooldown < period) return;

    for (std::size_t i = 0; i < enemies.size(); ++i) {
      Enemy* enemy = enemies.get(i);
      if (!enemy || !enemy->isAlive()) continue;
      const TDPosition p = enemy->getPosition();
      const float dx = p.x - position.x;
      const float dy = p.y - position.y;
      if (std::sqrt(dx * dx + dy * dy) <= range) {
        enemy->takeDamage(damage);
        cooldown = 0.f;
        break;
      }
    }
  }

  void render(sf::RenderWindow& window) override {
    sf::RectangleShape shape(sf::Vector2f(24.f, 24.f));
    shape.setFillColor(sf::Color(86, 103, 130));
    shape.setPosition(position.x - 12.f, position.y - 12.f);
    window.draw(shape);
  }
};

class SniperTower : public Tower {
public:
  explicit SniperTower(TDPosition position)
      : Tower(260.f, 55.f, 0.35f, position) {}

  void update(float delta_time) override { cooldown += delta_time; }

  void attack(DynamicArray<Enemy*>& enemies, float delta_time) override {
    cooldown += delta_time;
    const float period = 1.f / fire_rate;
    if (cooldown < period) return;

    Enemy* best = nullptr;
    float best_dist = 1000000.f;
    for (std::size_t i = 0; i < enemies.size(); ++i) {
      Enemy* enemy = enemies.get(i);
      if (!enemy || !enemy->isAlive()) continue;
      const TDPosition p = enemy->getPosition();
      const float dx = p.x - position.x;
      const float dy = p.y - position.y;
      const float d = std::sqrt(dx * dx + dy * dy);
      if (d <= range && d < best_dist) {
        best = enemy;
        best_dist = d;
      }
    }

    if (best) {
      best->takeDamage(damage);
      cooldown = 0.f;
    }
  }

  void render(sf::RenderWindow& window) override {
    sf::RectangleShape shape(sf::Vector2f(22.f, 22.f));
    shape.setFillColor(sf::Color(120, 128, 96));
    shape.setPosition(position.x - 11.f, position.y - 11.f);
    window.draw(shape);
  }
};

class MachineGunTower : public Tower {
public:
  explicit MachineGunTower(TDPosition position)
      : Tower(120.f, 10.f, 5.f, position) {}

  void update(float delta_time) override { cooldown += delta_time; }

  void attack(DynamicArray<Enemy*>& enemies, float delta_time) override {
    cooldown += delta_time;
    const float period = 1.f / fire_rate;
    if (cooldown < period) return;

    for (std::size_t i = 0; i < enemies.size(); ++i) {
      Enemy* enemy = enemies.get(i);
      if (!enemy || !enemy->isAlive()) continue;
      const TDPosition p = enemy->getPosition();
      const float dx = p.x - position.x;
      const float dy = p.y - position.y;
      if (std::sqrt(dx * dx + dy * dy) <= range) {
        enemy->takeDamage(damage);
        cooldown = 0.f;
        break;
      }
    }
  }

  void render(sf::RenderWindow& window) override {
    sf::RectangleShape shape(sf::Vector2f(20.f, 20.f));
    shape.setFillColor(sf::Color(145, 120, 70));
    shape.setPosition(position.x - 10.f, position.y - 10.f);
    window.draw(shape);
  }
};

class SlowTower : public Tower {
public:
  explicit SlowTower(TDPosition position)
      : Tower(125.f, 8.f, 1.f, position) {}

  void update(float delta_time) override { cooldown += delta_time; }

  void attack(DynamicArray<Enemy*>& enemies, float delta_time) override {
    cooldown += delta_time;
    const float period = 1.f / fire_rate;
    if (cooldown < period) return;

    for (std::size_t i = 0; i < enemies.size(); ++i) {
      Enemy* enemy = enemies.get(i);
      if (!enemy || !enemy->isAlive()) continue;
      const TDPosition p = enemy->getPosition();
      const float dx = p.x - position.x;
      const float dy = p.y - position.y;
      if (std::sqrt(dx * dx + dy * dy) <= range) {
        enemy->takeDamage(damage);
        cooldown = 0.f;
        break;
      }
    }
  }

  void render(sf::RenderWindow& window) override {
    sf::RectangleShape shape(sf::Vector2f(20.f, 20.f));
    shape.setFillColor(sf::Color(90, 120, 90));
    shape.setPosition(position.x - 10.f, position.y - 10.f);
    window.draw(shape);
  }
};

class FlameTower : public Tower {
public:
  explicit FlameTower(TDPosition position)
      : Tower(95.f, 16.f, 2.f, position) {}

  void update(float delta_time) override { cooldown += delta_time; }

  void attack(DynamicArray<Enemy*>& enemies, float delta_time) override {
    cooldown += delta_time;
    const float period = 1.f / fire_rate;
    if (cooldown < period) return;

    int hits = 0;
    for (std::size_t i = 0; i < enemies.size(); ++i) {
      Enemy* enemy = enemies.get(i);
      if (!enemy || !enemy->isAlive()) continue;
      const TDPosition p = enemy->getPosition();
      const float dx = p.x - position.x;
      const float dy = p.y - position.y;
      if (std::sqrt(dx * dx + dy * dy) <= range) {
        enemy->takeDamage(damage);
        ++hits;
      }
    }

    if (hits > 0) cooldown = 0.f;
  }

  void render(sf::RenderWindow& window) override {
    sf::RectangleShape shape(sf::Vector2f(22.f, 22.f));
    shape.setFillColor(sf::Color(160, 95, 50));
    shape.setPosition(position.x - 11.f, position.y - 11.f);
    window.draw(shape);
  }
};
