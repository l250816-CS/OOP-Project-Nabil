#include "Pages/GamePage.hpp"
#include <memory>
#include <string>
#include "Components/MusicPlayer.hpp"
#include "Enums/Event.hpp"
#include "Game.hpp"
#include "GameSettings.hpp"
#include "Utils/FontFactory.hpp"
#include "Utils/Logger.hpp"
#include "Views/BuildableTileView.hpp"
#include "Views/EnemyPathTileView.hpp"
#include "Views/EnemyView.hpp"
#include "Views/NonBuildableTileView.hpp"

GamePage::GamePage(unsigned width, unsigned height) : Page(width, height) {
  init_map();
  init_sidebar();
  init_hud();
  init_callbacks();
}

void GamePage::on_pause() {}

void GamePage::on_unpause() {
  auto lvl = Game::get_instance().get_level();
  if (lvl) {
    LOG_INFO("GamePage: on_unpause called. Current level: " + std::to_string(lvl->get_level_num()));
  } else {
    LOG_ERROR("GamePage: on_unpause called but level is NULL!");
  }
  notify_observers(Event::BG_CLEAR);
  MusicPlayer::get_instance().set_current_music(MusicPlayer::LEVEL_MUSIC);
}

void GamePage::handle_events(EventData evt) {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
    notify_observers(Event::PAUSE_PAGE_SWITCH);
  }
  // DEBUG CHEATS
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
    auto lvl = Game::get_instance().get_level();
    if (lvl) lvl->cheat_win_level();
  }
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
    auto lvl = Game::get_instance().get_level();
    if (lvl) lvl->cheat_complete_wave();
  }
  else {
    const std::size_t row = map.size();
    const std::size_t col = row > 0 ? map.get(0).size() : 0;
    for (std::size_t i = 0; i < row; ++i) {
      for (std::size_t j = 0; j < col; ++j) {
        map.get(i).get(j)->handle_events(evt);
      }
    }

    sidebar->handle_events(evt);
  }
}

void GamePage::render(RenderData ren) {
  for (std::size_t i = 0; i < map.size(); ++i) {
    for (std::size_t j = 0; j < map.get(i).size(); ++j) {
      map.get(i).get(j)->render(ren);
    }
  }
  for (std::size_t i = 0; i < enemies.size(); ++i) enemies.get(i)->render(ren);
  for (std::size_t i = 0; i < attacks.size(); ++i) attacks.get(i)->render(ren);

  ren.window->draw(top_hud_strip);
  ren.window->draw(hud_wave);
  ren.window->draw(hud_gold);
  ren.window->draw(hud_lives);
  ren.window->draw(hud_score);
  ren.window->draw(hud_timer);

  sidebar->render(ren);
}

void GamePage::update(UpdateData dat) {
  auto lvl = Game::get_instance().get_level();
  if (!lvl) return; // Safety check: level should exist

  static int update_counter = 0;
  if (update_counter++ % 300 == 0) {  // Log every ~5 seconds at 60 FPS
    LOG_DEBUG("GamePage: Update tick - Level " + std::to_string(lvl->get_level_num()) + 
              ", Wave " + std::to_string(lvl->get_wave_mng()->get_wave()) +
              ", State: " + std::to_string(lvl->get_game_state()));
  }

  if (lvl->get_game_state() == Level::WON) {
    LOG_INFO("GamePage: Level WON state detected, transitioning to WIN_PAGE");
    notify_observers(Event::WIN_PAGE_SWITCH);
    return;
  } else if (lvl->get_game_state() == Level::LOST) {
    LOG_INFO("GamePage: Level LOST state detected, transitioning to LOSE_PAGE");
    notify_observers(Event::LOSE_PAGE_SWITCH);
    return;
  }
  lvl->run_iteration();

  for (std::size_t i = 0; i < map.size(); ++i) {
    for (std::size_t j = 0; j < map.get(i).size(); ++j) {
      map.get(i).get(j)->update(dat);
    }
  }

  for (std::size_t i = 0; i < enemies.size();) {
    enemies.get(i)->update(dat);
    if (enemies.get(i)->get_removed()) {
      enemies.remove(i);
    } else {
      ++i;
    }
  }

  for (std::size_t i = 0; i < attacks.size(); ++i) attacks.get(i)->update(dat);
  update_hud();
  sidebar->update(dat);
}

void GamePage::init_map() {
  const int row = GameSettings::get_instance().get_rows();
  const int col = GameSettings::get_instance().get_columns();

  auto lvl = Game::get_instance().get_level();
  auto mmap = lvl->get_map();
  assert(mmap);

  for (int i = 0; i < row; ++i) {
    DynamicArray<std::shared_ptr<TileView>> row_tiles;
    for (int j = 0; j < col; ++j) {
      auto tile = mmap->get_tile(i, j);
      assert(tile != nullptr);

      std::shared_ptr<TileView> view = nullptr;
      auto type = tile->get_type();
      switch (type) {
        case BaseTile::Buildable: {
          auto res = std::make_shared<BuildableTileView>(tile);
          res->set_handler([this, res]() { set_selected(res); });
          view = std::move(res);
          break;
        }
        case BaseTile::NonBuildable:
          view = std::make_shared<NonBuildableTileView>(tile);
          break;
        case BaseTile::EnemyPath:
          view = std::make_shared<EnemyPathTileView>(tile);
          break;
      }
      row_tiles.add(std::move(view));
    }
    map.add(std::move(row_tiles));
  }
}

void GamePage::init_sidebar() { sidebar = std::make_shared<Sidebar>(); }

void GamePage::init_hud() {
  const auto tile_len = GameSettings::get_instance().get_tile_size();
  const auto board_cols = GameSettings::get_instance().get_columns();
  const float board_width = static_cast<float>(board_cols * tile_len);

  top_hud_strip.setSize(sf::Vector2f(board_width, 44.f));
  top_hud_strip.setPosition(0.f, 0.f);
  top_hud_strip.setFillColor(sf::Color(206, 170, 132, 235));
  top_hud_strip.setOutlineThickness(1.f);
  top_hud_strip.setOutlineColor(sf::Color(149, 108, 72, 200));

  hud_font = FontFactory::get_instance().get_primary_font();

  hud_wave.setFont(hud_font);
  hud_wave.setCharacterSize(22);
  hud_wave.setFillColor(sf::Color(95, 62, 38));
  hud_wave.setPosition(16.f, 8.f);

  hud_gold.setFont(hud_font);
  hud_gold.setCharacterSize(22);
  hud_gold.setFillColor(sf::Color(128, 84, 44));
  hud_gold.setPosition(250.f, 8.f);

  hud_lives.setFont(hud_font);
  hud_lives.setCharacterSize(22);
  hud_lives.setFillColor(sf::Color(142, 72, 54));
  hud_lives.setPosition(470.f, 8.f);

  hud_score.setFont(hud_font);
  hud_score.setCharacterSize(22);
  hud_score.setFillColor(sf::Color(60, 120, 60));
  hud_score.setPosition(700.f, 8.f);

  hud_timer.setFont(hud_font);
  hud_timer.setCharacterSize(22);
  hud_timer.setFillColor(sf::Color(60, 60, 120));
  hud_timer.setPosition(900.f, 8.f);

  update_hud();
}

void GamePage::update_hud() {
  auto lvl = Game::get_instance().get_level();
  if (!lvl) return; // Safety check to prevent null pointer access

  auto wave_mng = lvl->get_wave_mng();
  if (!wave_mng) return; // Safety check

  hud_wave.setString("WAVE " + std::to_string(wave_mng->get_wave()) +
                     "/" + std::to_string(wave_mng->get_wave_count()));
  hud_gold.setString("GOLD " + std::to_string(lvl->get_coins()));
  hud_lives.setString("LIVES " + std::to_string(lvl->get_lives()));
  hud_score.setString("SCORE " + std::to_string(lvl->get_score()));
  long ms = lvl->get_current_wave_elapsed_ms();
  int sec = static_cast<int>(ms / 1000);
  hud_timer.setString("TIME " + std::to_string(sec) + "s");
}

void GamePage::set_selected(std::shared_ptr<TileView> tile_view) {
  if (selected_tile != nullptr) selected_tile->set_selected(false);
  selected_tile = tile_view;
  selected_tile->set_selected(true);

  auto tile = std::dynamic_pointer_cast<BuildableTileView>(tile_view);

  sidebar->set_tile_target(tile_view);

  if (tile->get_tower_view() != nullptr) {
    sidebar->show_upgrades();
  } else {
    sidebar->hide_upgrades();
  }
}

void GamePage::init_callbacks() {
  auto lvl = Game::get_instance().get_level();
  assert(lvl != nullptr);

  lvl->set_on_enemy_created([this](std::shared_ptr<BaseEnemy> enm) {
    enemies.add(std::make_shared<EnemyView>(enm));
  });
  lvl->set_on_attack_created([this](std::shared_ptr<BaseAttack> att) {
    attacks.add(std::make_shared<AttackView>(att));
  });
  lvl->set_on_enemy_death([this](std::shared_ptr<BaseEnemy> enm) {
    for (std::size_t i = 0; i < enemies.size(); ++i) {
      if (enemies.get(i)->get_enemy() == enm) {
        enemies.remove(i);
        break;
      }
    }
  });
  lvl->set_on_enemy_out_of_bound([this](std::shared_ptr<BaseEnemy> enm) {
    for (std::size_t i = 0; i < enemies.size(); ++i) {
      if (enemies.get(i)->get_enemy() == enm) {
        enemies.remove(i);
        break;
      }
    }
  });
  lvl->set_on_attack_hit([this](std::shared_ptr<BaseAttack> att) {
    // TODO: ADD SFX HERE
  });
  lvl->set_on_attack_out_of_bound([this](std::shared_ptr<BaseAttack> att) {
    for (std::size_t i = 0; i < attacks.size(); ++i) {
      if (attacks.get(i)->get_attack() == att) {
        attacks.remove(i);
        break;
      }
    }
  });
}
