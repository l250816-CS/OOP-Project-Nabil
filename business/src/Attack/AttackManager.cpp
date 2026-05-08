#include "Attack/AttackManager.hpp"
#include <iostream>
#include "Attack/BaseAttack.hpp"

void AttackManager::register_attack(std::shared_ptr<BaseAttack> att) {
  attacks.add(att);
}

void AttackManager::remove_attack(std::shared_ptr<BaseAttack> att) {
  for (std::size_t i = 0; i < attacks.size(); ++i) {
    if (attacks.get(i) == att) {
      attacks.remove(i);
      return;
    }
  }
}

void AttackManager::move_attacks() {
  for (std::size_t i = 0; i < attacks.size(); ++i) attacks.get(i)->move_next();
}

void AttackManager::filter_attacks(
    std::function<void(std::shared_ptr<BaseAttack>)> on_attack_hit,
    std::function<void(std::shared_ptr<BaseAttack>)> on_attack_out_of_bound) {
  for (std::size_t i = 0; i < attacks.size();) {
    switch (attacks.get(i)->get_state()) {
      case BaseAttack::MOVING:
        ++i;
        break;
      case BaseAttack::HIT:
        on_attack_hit(attacks.get(i));
        ++i;
        break;
      case BaseAttack::OUT:
        on_attack_out_of_bound(attacks.get(i));
        attacks.remove(i);
        break;
    }
  }
}
