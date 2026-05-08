#pragma once

#include <string>
#include <memory>

struct GameSaveData {
  int level_num;
  int lives;
  int coins;
  int score;
  int wave;
  int killed_enemies;
};

struct ScoreRecord {
  int level_num;
  int score;
  bool won;
};

class SaveManager {
public:
  // Delete copying
  SaveManager(const SaveManager&) = delete;
  SaveManager& operator=(const SaveManager&) = delete;

  /**
   * @brief Get the one instance of SaveManager
   */
  static SaveManager& get_instance();

  /**
   * @brief Save the current game state
   */
  void save_game(const GameSaveData& data);

  /**
   * @brief Load the saved game state
   * @return true if save exists, false otherwise
   */
  bool load_game(GameSaveData& data);

  /**
   * @brief Check if a save file exists
   */
  bool has_save();

  /**
   * @brief Delete the save file
   */
  void delete_save();

  /**
   * @brief Add a score record
   */
  void add_score_record(const ScoreRecord& record);

  /**
   * @brief Get the highest score for a level
   */
  int get_high_score(int level_num);

  /**
   * @brief Get all scores for a level
   */
  int get_total_scores(int level_num);

private:
  SaveManager() = default;

private:
  static constexpr const char* SAVE_FILE = "./data/save.dat";
  static constexpr const char* SCORES_FILE = "./data/scores.dat";
};

