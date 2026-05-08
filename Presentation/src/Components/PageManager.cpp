#include "Components/PageManager.hpp"
#include <cassert>
#include <memory>
#include "Components/BackgroundManager.hpp"
#include "Components/PageFactory.hpp"
#include "Enums/Event.hpp"
#include "Enums/PageType.hpp"
#include "Game.hpp"
#include "Interfaces/EventData.hpp"
#include "Utils/Logger.hpp"
#include "SFML/Graphics/RenderTarget.hpp"

PageManager::PageManager(unsigned width, unsigned height)
    : target_width(width),
      target_height(height),
      back_mng(std::make_shared<BackgroundManager>(BackgroundManager::DEFAULT,
                                                   width, height)) {}

void PageManager::go_to_page(PageType type) {
  if (pages.size()) {
    pages.top()->on_pause();
  }
  auto page = PageFactory::get_instance().create_page(type, target_width,
                                                      target_height);
  auto self = shared_from_this();
  page->register_observer(self);
  page->register_observer(back_mng);
  page->on_unpause();
  pages.push(page);
}

void PageManager::replace_page(PageType type) {
  assert(pages.size());
  // Clear old level BEFORE creating new page
  // NOTE: Don't clear for GAME_PAGE since button handlers already create the level
  if ((type == PageType::LEVELS_PAGE)) {
    Game::get_instance().clear_level();
  }
  Logger::get_instance().log_debug("PageManager: replace_page called, popping old page");
  go_back();
  Logger::get_instance().log_debug("PageManager: creating new page of type " + std::to_string(static_cast<int>(type)));
  go_to_page(type);
  Logger::get_instance().log_debug("PageManager: new page created successfully");
}

void PageManager::go_back() {
  assert(pages.size());
  pages.pop();
  if (pages.size()) {
    pages.top()->on_unpause();
  };
}


void PageManager::render(RenderData ren) {
  back_mng->render(ren);
  if (pages.size() == 0) return; // Safety check
  pages.top()->render(ren);
}

void PageManager::handle_events(EventData evt) {
  if (pages.size() == 0) return; // Safety check
  pages.top()->handle_events(evt);
}

void PageManager::update(UpdateData dat) {
  if (pages.size() == 0) return; // Safety check
  pages.top()->update(dat);
}

void PageManager::onEvent(Event evt) {
  switch (evt) {
    case Event::MENU_MENU_SWITCH:
      go_to_page(PageType::MAIN_MENU);
      break;
    case Event::LEVEL_PAGE_SWITCH:
      go_to_page(PageType::LEVELS_PAGE);
      break;
    case Event::GAME_PAGE_SWITCH:
      go_to_page(PageType::GAME_PAGE);
      break;
    case Event::ABOUT_PAGE_SWITCH:
      go_to_page(PageType::ABOUT_PAGE);
      break;
    case Event::PAUSE_PAGE_SWITCH:
      go_to_page(PageType::PAUSE_PAGE);
      break;
    case Event::WIN_PAGE_SWITCH:
      go_to_page(PageType::WIN_PAGE);
      break;
    case Event::LOSE_PAGE_SWITCH:
      go_to_page(PageType::LOSE_PAGE);
      break;
    case Event::MENU_MENU_REPLACE:
      replace_page(PageType::MAIN_MENU);
      break;
    case Event::LEVEL_PAGE_REPLACE:
      replace_page(PageType::LEVELS_PAGE);
      break;
    case Event::GAME_PAGE_REPLACE:
      replace_page(PageType::GAME_PAGE);
      break;
    case Event::ABOUT_PAGE_REPLACE:
      replace_page(PageType::ABOUT_PAGE);
      break;
    case Event::PAUSE_PAGE_REPLACE:
      replace_page(PageType::PAUSE_PAGE);
      break;
    case Event::WIN_PAGE_REPLACE:
      replace_page(PageType::WIN_PAGE);
      break;
    case Event::LOSE_PAGE_REPLACE:
      replace_page(PageType::LOSE_PAGE);
      std::cout << "finished lose page replacement";
      break;
    case Event::GO_BACK_SWITCH:
      go_back();
      break;
    case Event::QUIT:
      
      break;

      
      
    default:
      break;
  }
}
