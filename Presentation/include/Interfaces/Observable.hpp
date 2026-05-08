#pragma once

#include <iostream>
#include <memory>
#include "Interfaces/Observer.hpp"
#include "Utils/DynamicArray.hpp"
class Observable {
public:
  /*
   * @brief Notify all subscribers
   */
  void notify_observers(Event evt) const {
    for (std::size_t i = 0; i < observers.size(); ++i) {
      observers.get(i)->onEvent(evt);
    }
  }

public:
  /*
   * @brief Register an observer
   */
  void register_observer(std::shared_ptr<Observer> obs) { observers.add(obs); }

private:
  DynamicArray<std::shared_ptr<Observer>> observers;
};
