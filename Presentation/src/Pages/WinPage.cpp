#include "Pages/WinPage.hpp"
#include <memory>

#include "Components/MusicPlayer.hpp"
#include "Components/TextButton.hpp"
#include "Enums/Event.hpp"
#include "Game.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "Widgets/MuteButton.hpp"
#include "Utils/Logger.hpp"

WinPage::WinPage(unsigned width, unsigned height)
    : Page(width, height), title("VICTORY", width, height) {
  unsigned start_x = width / 2, start_y = height / 2;
  int score_text_offset_y = -50;
  int buttons_offset_y = 100;
  int buttons_spacing_x = 160;
  int button_offset_multiplier = -1;

  // Cache level info before creating buttons that might invalidate the level
  auto lvl = Game::get_instance().get_level();
  int current_level_num = lvl ? lvl->get_level_num() : 0;
  int current_score = lvl ? lvl->get_score() : 0;
  int level_count = lvl ? lvl->get_level_count() : 0;

  LOG_INFO("WinPage: Loaded level_num=" + std::to_string(current_level_num) + 
           ", score=" + std::to_string(current_score) + 
           ", level_count=" + std::to_string(level_count));

  score_text = std::make_shared<Text>(
      "Score: " + std::to_string(current_score),
      80, sf::Color::White, start_x, start_y + score_text_offset_y);

  restart_button = std::make_shared<IconButton>(
      "./assets/buttons/Restart.png", "./assets/buttons/Restart_Hover.png",
      start_x + buttons_spacing_x * button_offset_multiplier++,
      start_y + buttons_offset_y);
  restart_button->set_handler([this, current_level_num]() {
    LOG_INFO("WinPage: Restart button clicked - reloading level " + std::to_string(current_level_num));
    LOG_BUTTON_CLICK("RESTART");
    Game::get_instance().init_level(current_level_num);
    notify_observers(Event::GAME_PAGE_SWITCH);
  });

  continue_button = std::make_shared<IconButton>(
      "./assets/buttons/Play.png", "./assets/buttons/Play_Hover.png",
      start_x + buttons_spacing_x * button_offset_multiplier++,
      start_y + buttons_offset_y);
  continue_button->set_handler([this, current_level_num, level_count]() {
    LOG_INFO("WinPage: Continue button clicked - current_level=" + std::to_string(current_level_num) + 
             ", level_count=" + std::to_string(level_count));
    
    // Check if there's a next level (level numbers are 1-indexed)
    if (current_level_num < level_count) {
      int next_level = current_level_num + 1;
      LOG_INFO("WinPage: Loading next level: " + std::to_string(next_level));
      LOG_BUTTON_CLICK("CONTINUE_NEXT_LEVEL");
      Game::get_instance().init_level(next_level);
      notify_observers(Event::GAME_PAGE_SWITCH);
    } else {
      // All levels completed - go back to levels page
      LOG_INFO("WinPage: All levels completed, returning to levels page");
      LOG_BUTTON_CLICK("CONTINUE_ALL_DONE");
      notify_observers(Event::ABOUT_PAGE_SWITCH);
    }
  });

  levels_button = std::make_shared<IconButton>(
      "./assets/buttons/Levels.png", "./assets/buttons/Levels_Hover.png",
      start_x + buttons_spacing_x * button_offset_multiplier++,
      start_y + buttons_offset_y);
  levels_button->set_handler([this]() {
    LOG_INFO("WinPage: Levels button clicked");
    LOG_BUTTON_CLICK("LEVELS");
    notify_observers(Event::LEVEL_PAGE_SWITCH);
  });
}

void WinPage::on_pause() {}
void WinPage::on_unpause() {
  notify_observers(Event::BG_SHADOW_SWITCH);
  // TODO: Change to win music
  MusicPlayer::get_instance().set_current_music(MusicPlayer::LEVEL_MUSIC);
}

void WinPage::handle_events(EventData evt) {
  if (levels_button) levels_button->handle_events(evt);
  if (continue_button) continue_button->handle_events(evt);
  if (restart_button) restart_button->handle_events(evt);
}

void WinPage::update(UpdateData dat) {}

void WinPage::render(RenderData ren) {
  title.render(ren);
  if (restart_button) restart_button->render(ren);
  if (continue_button) continue_button->render(ren);
  if (levels_button) levels_button->render(ren);
  if (score_text) score_text->render(ren);
}
