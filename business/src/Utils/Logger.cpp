#include "Utils/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <filesystem>

Logger::Logger() {
  // Create logs directory if it doesn't exist
  try {
    std::filesystem::create_directories("./logs");
  } catch (const std::exception& e) {
    std::cerr << "Failed to create logs directory: " << e.what() << std::endl;
  }
  
  // Open log file with timestamp
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  
  std::stringstream ss;
  ss << "./logs/game_" << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") << ".log";
  
  log_file.open(ss.str(), std::ios::app);
  
  if (log_file.is_open()) {
    log_info("========== GAME SESSION STARTED ==========");
  } else {
    std::cerr << "Failed to open log file at: " << ss.str() << std::endl;
  }
}

Logger::~Logger() {
  if (log_file.is_open()) {
    log_info("========== GAME SESSION ENDED ==========");
    log_file.close();
  }
}

std::string Logger::get_timestamp() {
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  
  std::stringstream ss;
  ss << std::put_time(std::localtime(&time), "%H:%M:%S");
  return ss.str();
}

std::string Logger::get_level_string(LogLevel level) {
  switch (level) {
    case DEBUG: return "[DEBUG]";
    case INFO: return "[INFO]";
    case WARNING: return "[WARNING]";
    case ERROR: return "[ERROR]";
    case CRITICAL: return "[CRITICAL]";
    default: return "[UNKNOWN]";
  }
}

void Logger::log(LogLevel level, const std::string& message) {
  std::string log_line = get_timestamp() + " " + get_level_string(level) + " " + message;
  
  // Console output
  std::cout << log_line << std::endl;
  
  // File output
  if (log_file.is_open()) {
    log_file << log_line << std::endl;
    log_file.flush();
  }
}

void Logger::log_debug(const std::string& message) {
  log(DEBUG, message);
}

void Logger::log_info(const std::string& message) {
  log(INFO, message);
}

void Logger::log_warning(const std::string& message) {
  log(WARNING, message);
}

void Logger::log_error(const std::string& message) {
  log(ERROR, message);
}

void Logger::log_critical(const std::string& message) {
  log(CRITICAL, message);
}

void Logger::log_level_start(int level_num) {
  std::stringstream ss;
  ss << "LEVEL " << level_num << " STARTED";
  log_info(ss.str());
}

void Logger::log_level_end(bool won, int final_score) {
  std::stringstream ss;
  ss << "LEVEL " << (won ? "WON" : "LOST") << " - Final Score: " << final_score;
  log_info(ss.str());
}

void Logger::log_wave_cleared(int wave_num, long duration_ms, int wave_score) {
  std::stringstream ss;
  ss << "WAVE " << wave_num << " CLEARED - Duration: " << duration_ms << "ms, Score: " << wave_score;
  log_info(ss.str());
}

void Logger::log_score_calculation(int base, int time_penalty, int gold_bonus, int final) {
  std::stringstream ss;
  ss << "SCORE CALC: base=" << base << " - time_penalty=" << time_penalty 
     << " + gold_bonus=" << gold_bonus << " = " << final;
  log_debug(ss.str());
}

void Logger::log_page_transition(const std::string& from_page, const std::string& to_page) {
  std::stringstream ss;
  ss << "PAGE TRANSITION: " << from_page << " -> " << to_page;
  log_info(ss.str());
}

void Logger::log_button_click(const std::string& button_name) {
  std::stringstream ss;
  ss << "BUTTON CLICKED: " << button_name;
  log_debug(ss.str());
}

void Logger::log_cheat_used(const std::string& cheat_name) {
  std::stringstream ss;
  ss << "CHEAT USED: " << cheat_name;
  log_warning(ss.str());
}
