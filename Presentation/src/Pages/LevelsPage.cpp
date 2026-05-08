#include "Pages/LevelsPage.hpp"
#include <memory>
#include <iostream>
#include "Components/IconButton.hpp"
#include "Components/LevelButton.hpp"
#include "Components/MusicPlayer.hpp"
#include "Game.hpp"
#include "SaveManager.hpp"
#include "Widgets/MuteButton.hpp"

LevelsPage::LevelsPage(unsigned w, unsigned h) : Page(w, h) {
  level_count = Game::get_instance().get_level()->get_level_count();

  const int ROW_COUNT = 5;
  const int startx = 440, starty = 400;
  const int offsetx = 250, offsety = 200;
  for (int i = 0; i < static_cast<int>(level_count); i++) {
    int column = i % ROW_COUNT;
    int row = i / ROW_COUNT;
    auto btn =
        std::make_unique<LevelButton>(startx + column * offsetx,
                                      starty + row * offsety,
                                      i + 1);
    btn->set_handler([this, i]() {
      Game::get_instance().init_level(i + 1);
      notify_observers(Event::GAME_PAGE_SWITCH);
    });
    levels_btns.add(std::move(btn));
  }

  go_back =
      std::make_shared<IconButton>("./assets/buttons/GoBack.png",
                                   "./assets/buttons/GoBack_Hover.png", 60, 60);
  go_back->set_handler([this]() { notify_observers(Event::MENU_MENU_SWITCH); });

  mute_button = std::make_shared<MuteButton>(60, h - 60);
}

void LevelsPage::on_pause() {}
void LevelsPage::on_unpause() {
  notify_observers(Event::BG_DEFAULT_SWITCH);
  MusicPlayer::get_instance().set_current_music(MusicPlayer::MAIN_MUSIC);
  mute_button->check_status();
}

void LevelsPage::handle_events(EventData evt) {
  for (std::size_t i = 0; i < levels_btns.size(); ++i) levels_btns.get(i)->handle_events(evt);
 
  go_back->handle_events(evt);
  mute_button->handle_events(evt);
}

void LevelsPage::update(UpdateData dat) { mute_button->check_status(); }

void LevelsPage::render(RenderData ren) {
  for (std::size_t i = 0; i < levels_btns.size(); ++i) levels_btns.get(i)->render(ren);
 
  go_back->render(ren);
  mute_button->render(ren);
}
