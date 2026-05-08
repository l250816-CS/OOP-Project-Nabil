#include "Game.hpp"
#include <iostream>
#include "LevelReader.hpp"
#include "SaveManager.hpp"

Game& Game::get_instance() {
  static Game game;
  return game;
}

std::shared_ptr<Level> Game::get_level() { return lvl; }

void Game::init_level(int level_num) {
  lvl = LevelReader::get_instance().build_level(level_num);
}

void Game::save_current_game() {
  if (!lvl) {
    std::cerr << "Cannot save: no active level" << std::endl;
    return;
  }

  GameSaveData data;
  data.level_num = lvl->get_level_num();
  data.lives = lvl->get_lives();
  data.coins = lvl->get_coins();
  data.score = lvl->get_score();
  data.wave = lvl->get_wave_mng()->get_wave();
  data.killed_enemies = 0;  // WaveManager doesn't provide killed enemies count

  SaveManager::get_instance().save_game(data);
}

bool Game::load_saved_game() {
  GameSaveData data;
  if (!SaveManager::get_instance().load_game(data)) {
    return false;
  }

  init_level(data.level_num);
  if (!lvl) {
    return false;
  }

  // TODO: Restore the game state from the saved data
  // This requires additional methods in Level class to restore state
  // For now, return true as the level is initialized
  return true;
}

bool Game::has_saved_game() const {
  return SaveManager::get_instance().has_save();
}

void Game::clear_level() { lvl = nullptr; }

