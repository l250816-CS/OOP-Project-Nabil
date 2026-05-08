#pragma once

#include <fstream>
#include <string>
#include <chrono>
#include <sstream>

class Logger {
public:
  enum LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
  };

  static Logger& get_instance() {
    static Logger instance;
    return instance;
  }

  void log(LogLevel level, const std::string& message);
  void log_debug(const std::string& message);
  void log_info(const std::string& message);
  void log_warning(const std::string& message);
  void log_error(const std::string& message);
  void log_critical(const std::string& message);

  // Event logging
  void log_level_start(int level_num);
  void log_level_end(bool won, int final_score);
  void log_wave_cleared(int wave_num, long duration_ms, int wave_score);
  void log_score_calculation(int base, int time_penalty, int gold_bonus, int final);
  void log_page_transition(const std::string& from_page, const std::string& to_page);
  void log_button_click(const std::string& button_name);
  void log_cheat_used(const std::string& cheat_name);

private:
  Logger();
  ~Logger();

  std::ofstream log_file;
  std::string get_timestamp();
  std::string get_level_string(LogLevel level);
};

#define LOG_DEBUG(msg) Logger::get_instance().log_debug(msg)
#define LOG_INFO(msg) Logger::get_instance().log_info(msg)
#define LOG_WARNING(msg) Logger::get_instance().log_warning(msg)
#define LOG_ERROR(msg) Logger::get_instance().log_error(msg)
#define LOG_CRITICAL(msg) Logger::get_instance().log_critical(msg)
#define LOG_BUTTON_CLICK(name) Logger::get_instance().log_button_click(name)
