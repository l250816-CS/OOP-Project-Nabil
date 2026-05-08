#pragma once
#include <memory>
#include "Components/BackgroundManager.hpp"
#include "Components/PageManager.hpp"
#include "SFML/Graphics.hpp"

class App {
public:
  App();
  /*
   * @brief Run the game
   */
  void run();

private:
  /*
   * @brief Initliaze the window
   */
  void init_window();

  /**
   * @brief Init Time
   */
  void init_time();

  /*
   * @brief Initialize the page manager
   */
  void init_page_manager();

  /*
   * @brief Handle all events of the game
   */
  void handle_events();

  /*
   * @brief Update the state of the game
   */
  void update();

  /*
   * @brief Render the game
   */
  void render();

  /**
   * @brief Initialize all needed assets, info, reading from file, etc.
   */
  void init_info();

  /**
   * @brief Create render window
   * @param fullscreen True for fullscreen, false for windowed
   */
  void create_window(bool fullscreen);

  /**
   * @brief Apply the viewport settings to match the current window size
   */
  void apply_viewport();

  void on_notify(const Event&);

private:
  std::shared_ptr<sf::RenderWindow> window;
  sf::Event event;
  std::shared_ptr<PageManager> page_mng;
  sf::Clock clock;
  sf::Time deltaTime;

  bool fullscreen_mode = true;
  unsigned target_width = 0;
  unsigned target_height = 0;
  sf::View world_view;

  
};
