#include "Components/Sidebar.hpp"
#include <memory>
#include <string>
#include "Components/SFXPlayer.hpp"
#include "Components/SidebarItem.hpp"
#include "Components/SidebarTowerButton.hpp"
#include "Game.hpp"
#include "GameSettings.hpp"
#include "Tower/BaseTower.hpp"
#include "Utils/FontFactory.hpp"
#include "Views/BuildableTileView.hpp"
#include "iostream"

DynamicArray<std::pair<BaseTower::TowerType, std::string>> Sidebar::towers_info = {
    {BaseTower::ArcheryTower, "./assets/textures/towers/ArcheryTower.png"},
    {BaseTower::CatapultTower, "./assets/textures/towers/CatapultTower.png"},
    {BaseTower::ElectroTower, "./assets/textures/towers/ElectroTower.png"},
    {BaseTower::SlingshotTower, "./assets/textures/towers/SlingshotTower.png"},
    {BaseTower::OrbTower, "./assets/textures/towers/OrbTower.png"},
    {BaseTower::CrossbowTower, "./assets/textures/towers/CrossbowTower.png"},
};
Sidebar::Sidebar() {
  init_sidebar_bg();
  init_content();
  init_tower_card();

  show_upgrade_buttons = false;
  init_tower_buttons();
}

void Sidebar::init_sidebar_bg() {
  if (!sidebar_texture.loadFromFile("./assets/textures/sidebar/bg.png"))
    exit(1);
  sidebar_bg.setTexture(sidebar_texture);
  sidebar_bg.setColor(sf::Color(186, 151, 114, 240));
  auto h = GameSettings::get_instance().get_size().y;
  auto sidebar_row = GameSettings::get_instance().get_sidebar_row_count();
  auto tile_len = GameSettings::get_instance().get_tile_size();
  auto col = GameSettings::get_instance().get_columns();
  sidebar_bg.setPosition(col * tile_len, 0);
  auto bounds = sidebar_bg.getLocalBounds();
  float actual_width = bounds.width;
  float actual_height = bounds.height;

  // Calculate scale factors
  float scale_x = tile_len * sidebar_row / actual_width;
  float scale_y = h / actual_height;

  // Apply scaling
  sidebar_bg.setScale(scale_x, scale_y);
}

void Sidebar::init_content() {
  init_item(
      lives, "./assets/textures/sidebar/lives.png",
      []() {
        auto lvl = Game::get_instance().get_level();
        return std::string("LIVES: ") + std::to_string(lvl->get_lives());
      },
      90);
  init_item(
      coins, "./assets/textures/sidebar/coins.png",
      []() {
        auto lvl = Game::get_instance().get_level();
        return std::string("GOLD: ") + std::to_string(lvl->get_coins());
      },
      180);
  init_item(
      wave, "./assets/textures/sidebar/coins.png",
      []() {
        auto lvl = Game::get_instance().get_level();
        auto wm = lvl->get_wave_mng();
        return std::string("WAVE: ") + std::to_string(wm->get_wave()) +
               "/" + std::to_string(wm->get_wave_count());
      },
      270);
}

void Sidebar::init_tower_card() {
  auto tile_len = GameSettings::get_instance().get_tile_size();
  auto col = GameSettings::get_instance().get_columns();
  auto sidebar_len =
      GameSettings::get_instance().get_sidebar_row_count() * tile_len;
  float panel_x = static_cast<float>(col * tile_len + 14);

  tower_card_bg.setSize(sf::Vector2f(static_cast<float>(sidebar_len - 28), 130.f));
  tower_card_bg.setPosition(panel_x, 330.f);
  tower_card_bg.setFillColor(sf::Color(218, 186, 148, 238));
  tower_card_bg.setOutlineThickness(2.f);
  tower_card_bg.setOutlineColor(sf::Color(154, 112, 74, 220));

  hud_font = FontFactory::get_instance().get_primary_font();

  tower_card_title.setFont(hud_font);
  tower_card_title.setCharacterSize(20);
  tower_card_title.setFillColor(sf::Color(84, 56, 34));
  tower_card_title.setPosition(panel_x + 10.f, 340.f);

  tower_card_stats.setFont(hud_font);
  tower_card_stats.setCharacterSize(16);
  tower_card_stats.setFillColor(sf::Color(112, 76, 44));
  tower_card_stats.setPosition(panel_x + 10.f, 370.f);
}

std::string Sidebar::get_tower_name(BaseTower::TowerType type) const {
  switch (type) {
    case BaseTower::ArcheryTower: return "Archery Tower";
    case BaseTower::CatapultTower: return "Catapult Tower";
    case BaseTower::ElectroTower: return "Electro Tower";
    case BaseTower::SlingshotTower: return "Slingshot Tower";
    case BaseTower::OrbTower: return "Orb Tower";
    case BaseTower::CrossbowTower: return "Crossbow Tower";
    default: return "Tower";
  }
}

void Sidebar::update_tower_card() {
  if (target == nullptr || target->get_tile()->get_type() != BaseTile::Buildable) {
    tower_card_title.setString("Select a tile");
    tower_card_stats.setString("Pick a buildable tile\nto place or inspect towers.");
    return;
  }

  auto build_tile = std::dynamic_pointer_cast<BuildableTileView>(target);
  if (build_tile == nullptr || build_tile->get_tower_view() == nullptr) {
    tower_card_title.setString("Build Slot");
    tower_card_stats.setString("Choose a tower below\nto deploy defenses.");
    return;
  }

  auto tower = build_tile->get_tower_view()->get_tower();
  tower_card_title.setString(get_tower_name(tower->get_type()));
  tower_card_stats.setString(
      std::string("LVL: ") + std::to_string(tower->get_level()) +
      "\nRANGE: " + std::to_string(tower->get_range()) +
      "\nRATE: " + std::to_string(tower->get_shoot_rate_ms()) + " ms");
}

void Sidebar::handle_events(EventData data) {
  if (target != nullptr) {
    if (!show_upgrade_buttons) {
      for (std::size_t i = 0; i < twr_btns.size(); ++i) twr_btns.get(i)->handle_events(data);
    } else {
      upgrade_button->handle_events(data);
      sell_button->handle_events(data);
    }
  }
}

void Sidebar::render(RenderData ren) {
  ren.window->draw(sidebar_bg);
  lives->render(ren);
  coins->render(ren);
  wave->render(ren);
  ren.window->draw(tower_card_bg);
  ren.window->draw(tower_card_title);
  ren.window->draw(tower_card_stats);

  if (target != nullptr) {
    if (!show_upgrade_buttons) {
      for (std::size_t i = 0; i < twr_btns.size(); ++i) twr_btns.get(i)->render(ren);
    } else {
      upgrade_button->render(ren);
      sell_button->render(ren);
    }
  }
}
void Sidebar::update(UpdateData dat) {
  lives->update(dat);
  coins->update(dat);
  wave->update(dat);
  update_tower_card();
}
void Sidebar::init_item(std::shared_ptr<SidebarItem>& item,
                        std::string icon_path,
                        std::function<std::string(void)> func, int y) {
  auto tile_len = GameSettings::get_instance().get_tile_size();
  auto col = GameSettings::get_instance().get_columns();
  auto sidebar_len =
      GameSettings::get_instance().get_sidebar_row_count() * tile_len;
  item = std::make_shared<SidebarItem>(func, icon_path);
  auto bounds = item->get_global_bounds();
  item->set_position(col * tile_len + (sidebar_len - bounds.width) / 2.f, y);
}

void Sidebar::set_tile_target(std::shared_ptr<TileView> tile) { target = tile; }

void Sidebar::show_upgrades() {
  auto tile = std::static_pointer_cast<BuildableTileView>(target);
  auto tower = tile->get_tower_view()->get_tower();

  if (tower->get_level() < tower->get_upgrades_count()) {
    upgrade_button->set_text("upgrade\n      for\n    " +
                             std::to_string(tower->get_upgrade_price()));
  } else {
    upgrade_button->set_text("max\nlevel");
  }

  sell_button->set_text("sell\n  for\n   " +
                        std::to_string(tower->get_sell_price()));

  show_upgrade_buttons = true;
}

void Sidebar::hide_upgrades() { show_upgrade_buttons = false; }

void Sidebar::init_tower_buttons() {
  auto col = GameSettings::get_instance().get_columns();
  auto tile_len = GameSettings::get_instance().get_tile_size();
  auto sidebar_rows = GameSettings::get_instance().get_sidebar_row_count();

  int sidebar_width = sidebar_rows * tile_len;
  int grid_width = 2;
  int cell_width = 120;
  int cell_height = 90;
  float vertical_spacing = 1.15f;

  int x_start = col * tile_len +
                (sidebar_width - grid_width * cell_width - TOWERS_OFFSET) / 2;
  int y_start = 480;

  upgrade_button = std::make_shared<TextButton>(
      "upgrade", x_start + cell_width, y_start + 40, 35,
      StandardButton::ButtonSize::XLARGE, StandardButton::ButtonType::SQUARE);
  upgrade_button->set_handler([this]() {
    auto tile = std::static_pointer_cast<BuildableTileView>(target);
    if (tile->upgrade_tower()) {
      target->set_selected(false);
      target = nullptr;
      SFXPlayer::get_instance().play(SFXPlayer::TOWER_BUILD);
    } else {
      target->set_selected(false);
      target = nullptr;
    }
  });

  upgrade_button->transform_text(0, -30 * 2);

  sell_button = std::make_shared<TextButton>(
      "sell", x_start + cell_width, y_start + 280, 35,
      StandardButton::ButtonSize::XLARGE, StandardButton::ButtonType::SQUARE);
  sell_button->set_handler([this]() {
    auto tile = std::static_pointer_cast<BuildableTileView>(target);
    tile->sell_tower();
    target->set_selected(false);
    target = nullptr;
    SFXPlayer::get_instance().play(SFXPlayer::TOWER_BUILD);
  });

  sell_button->transform_text(0, -30 * 2);

  for (std::size_t index = 0; index < towers_info.size(); ++index) {
    auto type = towers_info.get(index).first;
    auto path = towers_info.get(index).second;
    int row = static_cast<int>(index) / grid_width;
    int column = static_cast<int>(index) % grid_width;

    int x = x_start + column * cell_width + (column != 0 ? TOWERS_OFFSET : 0);
    int y = y_start + static_cast<int>(row * cell_height * vertical_spacing);

    auto twn_btn = std::make_shared<SidebarTowerButton>(
        x, y, path, BaseTower::get_buy_price(type));
    twn_btn->set_handler([this, type]() {
      if (target->get_tile()->get_type() != BaseTile::Buildable) return;
      auto converted = std::dynamic_pointer_cast<BuildableTileView>(target);
      if (converted->build_tower(type)) {
        target->set_selected(false);
        target = nullptr;
        SFXPlayer::get_instance().play(SFXPlayer::TOWER_BUILD);
      } else {
        target->set_selected(false);
        target = nullptr;
      }
    });

    twr_btns.add(twn_btn);
  }
}
