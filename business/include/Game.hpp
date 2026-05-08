#pragma once

#include <memory>
#include "Level.hpp"
#include "SaveManager.hpp"

class Game {
public:
  // Delete copying
  Game(const Game&) = delete;
  Game& operator=(const Game&) = delete;

  /**
   * @brief Get the one instance of the Game
   */
  static Game& get_instance();

  /**
   * @brief Get Level
   */
  std::shared_ptr<Level> get_level();

  /**
   * @brief Initialize a level by using
   * LevelReader::get_instance().build_level(level_num);
   */
  void init_level(int level_num);

  /**
   * @brief Save the current game state
   */
  void save_current_game();

  /**
   * @brief Load the saved game
   * @return true if load successful, false otherwise
   */
  bool load_saved_game();

  /**
   * @brief Check if a save exists
   */
  bool has_saved_game() const;

  /**
   * @brief Remove the level
   */
  void clear_level();

private:
  Game() = default;  // < private constructor

private:
  std::shared_ptr<Level> lvl;  // < if null, the game hasn't started
};
