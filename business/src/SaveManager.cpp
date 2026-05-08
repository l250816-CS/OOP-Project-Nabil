#include "SaveManager.hpp"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <filesystem>

SaveManager& SaveManager::get_instance() {
  static SaveManager manager;
  return manager;
}

void SaveManager::save_game(const GameSaveData& data) {
  std::filesystem::create_directories("./data");

  std::ofstream file(SAVE_FILE, std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Failed to open save file for writing" << std::endl;
    return;
  }

  file.write(reinterpret_cast<const char*>(&data.level_num), sizeof(int));
  file.write(reinterpret_cast<const char*>(&data.lives), sizeof(int));
  file.write(reinterpret_cast<const char*>(&data.coins), sizeof(int));
  file.write(reinterpret_cast<const char*>(&data.score), sizeof(int));
  file.write(reinterpret_cast<const char*>(&data.wave), sizeof(int));
  file.write(reinterpret_cast<const char*>(&data.killed_enemies), sizeof(int));

  file.close();
}

bool SaveManager::load_game(GameSaveData& data) {
  std::ifstream file(SAVE_FILE, std::ios::binary);
  if (!file.is_open()) {
    return false;
  }

  file.read(reinterpret_cast<char*>(&data.level_num), sizeof(int));
  file.read(reinterpret_cast<char*>(&data.lives), sizeof(int));
  file.read(reinterpret_cast<char*>(&data.coins), sizeof(int));
  file.read(reinterpret_cast<char*>(&data.score), sizeof(int));
  file.read(reinterpret_cast<char*>(&data.wave), sizeof(int));
  file.read(reinterpret_cast<char*>(&data.killed_enemies), sizeof(int));

  file.close();
  return true;
}

bool SaveManager::has_save() {
  std::ifstream file(SAVE_FILE);
  return file.good();
}

void SaveManager::delete_save() {
  if (std::remove(SAVE_FILE) != 0) {
    std::cerr << "Failed to delete save file" << std::endl;
  }
}

void SaveManager::add_score_record(const ScoreRecord& record) {
  std::filesystem::create_directories("./data");

  std::ofstream file(SCORES_FILE, std::ios::app | std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Failed to open scores file for writing" << std::endl;
    return;
  }

  file.write(reinterpret_cast<const char*>(&record.level_num), sizeof(int));
  file.write(reinterpret_cast<const char*>(&record.score), sizeof(int));
  file.write(reinterpret_cast<const char*>(&record.won), sizeof(bool));

  file.close();
}

int SaveManager::get_high_score(int level_num) {
  std::ifstream file(SCORES_FILE, std::ios::binary);
  if (!file.is_open()) {
    return 0;
  }

  int high_score = 0;
  ScoreRecord record;

  while (file.read(reinterpret_cast<char*>(&record.level_num), sizeof(int))) {
    file.read(reinterpret_cast<char*>(&record.score), sizeof(int));
    file.read(reinterpret_cast<char*>(&record.won), sizeof(bool));

    if (record.level_num == level_num && record.score > high_score) {
      high_score = record.score;
    }
  }

  file.close();
  return high_score;
}

int SaveManager::get_total_scores(int level_num) {
  std::ifstream file(SCORES_FILE, std::ios::binary);
  if (!file.is_open()) {
    return 0;
  }

  int total = 0;
  ScoreRecord record;

  while (file.read(reinterpret_cast<char*>(&record.level_num), sizeof(int))) {
    file.read(reinterpret_cast<char*>(&record.score), sizeof(int));
    file.read(reinterpret_cast<char*>(&record.won), sizeof(bool));

    if (record.level_num == level_num) {
      total++;
    }
  }

  file.close();
  return total;
}

