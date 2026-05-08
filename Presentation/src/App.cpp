#include "App.hpp"
#include <cassert>
#include <memory>
#include "Components/MusicPlayer.hpp"
#include "Components/PageManager.hpp"
#include "GameSettings.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "Views/AttackView.hpp"
#include "Views/EnemyView.hpp"
#include "Views/TowerView.hpp"
#include "Utils/Logger.hpp"

App::App() {
  init_info();
  init_window();
  init_time();
  init_page_manager();
  MusicPlayer::get_instance().play();
}

void App::create_window(bool fullscreen) {
  fullscreen_mode = fullscreen;

  if (fullscreen_mode) {
    auto desktop = sf::VideoMode::getDesktopMode();
    window = std::make_shared<sf::RenderWindow>(
        desktop, "Tower Defense", sf::Style::Fullscreen);
  } else {
    window = std::make_shared<sf::RenderWindow>(
        sf::VideoMode(target_width, target_height), "Tower Defense",
        sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);
  }

  window->setFramerateLimit(60);
  apply_viewport();
}

void App::apply_viewport() {
  const float window_w = static_cast<float>(window->getSize().x);
  const float window_h = static_cast<float>(window->getSize().y);
  const float target_ratio = static_cast<float>(target_width) / target_height;
  const float window_ratio = window_w / window_h;

  sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);
  if (window_ratio > target_ratio) {
    const float width = target_ratio / window_ratio;
    viewport.left = (1.f - width) * 0.5f;
    viewport.width = width;
  } else if (window_ratio < target_ratio) {
    const float height = window_ratio / target_ratio;
    viewport.top = (1.f - height) * 0.5f;
    viewport.height = height;
  }

  world_view.setViewport(viewport);
  window->setView(world_view);
}

void App::init_window() {
  const auto tile = GameSettings::get_instance().get_tile_size();
  const auto cols = GameSettings::get_instance().get_columns();
  const auto sidebar_cols = GameSettings::get_instance().get_sidebar_row_count();
  target_height = GameSettings::get_instance().get_size().y;
  target_width = (cols + sidebar_cols) * tile;

  world_view = sf::View(
      sf::FloatRect(0.f, 0.f, static_cast<float>(target_width),
                    static_cast<float>(target_height)));

  create_window(true);
}

void App::init_page_manager() {
  assert(window);
  page_mng = std::make_shared<PageManager>(target_width, target_height);
  page_mng->go_to_page(PageType::MAIN_MENU);
}

void App::run() {
  while (window->isOpen()) {
    handle_events();
    update();
    render();
    deltaTime = clock.restart();
  }
}

void App::handle_events() {
  while (window->pollEvent(event)) {
    if (event.type == sf::Event::Closed ||
        (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && sf::Keyboard::isKeyPressed(sf::Keyboard::A) 
        )         
        ) {
      Logger::get_instance().log_info("EXIT SHORTCUT: Ctrl+Shift+Q pressed - exiting game");
      window->close();
      continue;
    }


    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11) {
      create_window(!fullscreen_mode);
      init_page_manager();
      continue;
    }

    if (event.type == sf::Event::Resized) {
      apply_viewport();
    }

    sf::Vector2i pixel = sf::Mouse::getPosition(*window);
    if (event.type == sf::Event::MouseMoved) {
      pixel = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
    } else if (event.type == sf::Event::MouseButtonPressed ||
               event.type == sf::Event::MouseButtonReleased) {
      pixel = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
    }

    const sf::Vector2f world = window->mapPixelToCoords(pixel, world_view);
    page_mng->handle_events(
        {event, sf::Vector2i(static_cast<int>(world.x), static_cast<int>(world.y))});
  }
}

void App::update() { page_mng->update({}); }

void App::render() {
  window->clear();
  window->setView(world_view);
  page_mng->render({deltaTime, window});
  window->display();
}

void App::init_time() { deltaTime = sf::Time::Zero; }
void App::init_info() {
  TowerView::load_tower_info();
  EnemyView::load_enemy_info();
  AttackView::load_attack_info();
}

void App::on_notify(const Event& event) {
    if (event == Event::QUIT) {
        Logger::get_instance().log_info("QUIT button pressed from MainMenu - exiting game");
        window->close();
    }
}
