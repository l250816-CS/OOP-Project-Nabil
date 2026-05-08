#include "Views/TowerView.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include "Game.hpp"
#include "GameSettings.hpp"
#include "Tower/BaseTower.hpp"

std::unordered_map<BaseTower::TowerType, TowerView::TowerInfo>
    TowerView::towers_info = {};
std::unordered_map<int, std::string> TowerView::levels_info = {
    {0, "LVL1"},
    {1, "LVL2"},
    {2, "LVL3"},
};
TowerView::TowerView(std::shared_ptr<BaseTower> tower) : tower(tower) {
  init_tower_sprite();
  init_weapon_sprite();
}

void TowerView::init_tower_sprite() {
  assert(tower != nullptr);

  // Initialize the sheet manager
  tower_sprite_mng.load_sheet(sprite, sprite_sheet,
                              towers_info[tower->get_type()].tower_sprite);
  tower_sprite_mng.set_width(64);
  tower_sprite_mng.set_height(128);
  for (auto& [i, collc] : levels_info) {
    std::cout << i << " " << collc << std::endl;
    tower_sprite_mng.register_collection(collc, i, 1);
  }
  tower_sprite_mng.set_collection("LVL1");
  tower_sprite_mng.init_sprite_texture(sprite);

  // Set Position with Offset
  auto pos = tower->get_position();
  auto tile_len = GameSettings::get_instance().get_tile_size();
  pos.x += tile_len / 2.f * (1 - TOWER_TILE_FACTOR);
  pos.y -= tile_len / 2.f * (TOWER_TILE_FACTOR);
  sprite.setPosition(pos.x, pos.y);

  // Set Scale to the sprite
  sprite.setScale(TOWER_TILE_FACTOR * 120.f / 64,
                  TOWER_TILE_FACTOR * 120.f / 64);
}

void TowerView::init_weapon_sprite() {
  auto& [tower_texture_sheet, weapon_texture_sheet, attacks_offsets,
         attacks_size, sprites_number, shooting_sprite] =
      towers_info[tower->get_type()];
  auto& [num1, num2, num3] = sprites_number;

  // Load weapon texture and initialize the sprite
  weapon_sprite_mng.load_sheet(weapon, weapon_texture, weapon_texture_sheet);
  weapon_sprite_mng.set_width(attacks_size.first);  // Width from attacks_size
  weapon_sprite_mng.set_height(
      attacks_size.second);  // Height from attacks_size
  weapon_sprite_mng.register_collection("LVL1", 0, num1);
  weapon_sprite_mng.register_collection("LVL2", 1, num2);
  weapon_sprite_mng.register_collection("LVL3", 2, num3);
  weapon_sprite_mng.set_collection("LVL1");
  weapon_sprite_mng.scale_animation_delay(2);
  weapon_sprite_mng.init_sprite_texture(weapon);

  // Calculate weapon position
  auto pos = tower->get_position();  // Tower position
  auto tile_len = GameSettings::get_instance().get_tile_size();

  // Fetch weapon dimensions from attacks_size
  float weapon_offset_x = (tile_len) / 2.f;
  float weapon_offset_y =
      -(tile_len / 2.f) + attacks_offsets[tower->get_level() - 1];

  // Set weapon position
  pos.x += weapon_offset_x;
  pos.y += weapon_offset_y;
  weapon.setPosition(pos.x, pos.y);

  // Set scale using attacks_size
  weapon.setScale(WEAPON_SPRITE_LEN / attacks_size.first,
                  WEAPON_SPRITE_LEN / attacks_size.second);

  // Set the origin of the weapon to its center (important for correct rotation)
  weapon.setOrigin(weapon.getLocalBounds().width / 2.f,
                   weapon.getLocalBounds().height / 2.f);
}
void TowerView::handle_events(EventData data) {}
void TowerView::render(RenderData ren) {
  ren.window->draw(sprite);
  ren.window->draw(weapon);
}

void TowerView::update(UpdateData dat) {
  // Check update of level
  auto current_lvl_collection = levels_info[tower->get_level() - 1];
  if (current_lvl_collection != tower_sprite_mng.get_current_collection()) {
    tower_sprite_mng.set_collection(levels_info[tower->get_level() - 1]);
    tower_sprite_mng.init_sprite_texture(sprite);
  }

  // Check Update of attack
  if (current_lvl_collection != weapon_sprite_mng.get_current_collection()) {
    weapon_sprite_mng.set_collection(levels_info[tower->get_level() - 1]);
    weapon_sprite_mng.init_sprite_texture(weapon);
  }

  auto& [tower_texture_sheet, weapon_texture_sheet, attacks_offsets,
         attacks_size, sprites_number, shooting_sprite] =
      towers_info[tower->get_type()];
  auto enemies_in_range = tower->enemies_in_range();
  if (!enemies_in_range.size() || !tower->can_shoot()) {
    if (weapon_sprite_mng.get_current_index() != 0)
      weapon_sprite_mng.next_sprite(weapon);
    return;
  }
  weapon_sprite_mng.next_sprite(weapon);

  // Get the position of the first enemy in range
  auto enemy_position = enemies_in_range[0]->get_position();
  auto weapon_pos = weapon.getPosition();

  // Calculate the direction vector (enemy position - weapon position)
  float dx = enemy_position.x - weapon_pos.x;
  float dy = enemy_position.y - weapon_pos.y;

  // Use the existing get_angle() method to calculate the angle of the
  // direction
  Vector<float> direction(dx, dy);
  float angle = direction.get_angle();

  // Rotate the weapon to face the enemy (ensure it rotates around its center)
  weapon.setRotation(angle + 90);  // Adjust by 90 if necessary

  // Shoot the attack if on the current sprite
  if (weapon_sprite_mng.get_current_index() ==
      shooting_sprite[tower->get_level() - 1]) {
    auto lvl = Game::get_instance().get_level();
    lvl->attack(tower, weapon_pos.x, weapon_pos.y, 8, 8, enemy_position);
  }
}

std::shared_ptr<BaseTower> TowerView::get_tower() { return tower; }

void TowerView::load_tower_info() {
  std::ifstream file(FILE_PATH);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open towers info file: " + FILE_PATH);
  }

  towers_info.clear();
  std::string line;

  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }

    std::istringstream line_stream(line);
    std::string tower_name, tower_sprite, weapon_sprite, attack_offsets_str,
        attack_size_str, frame_counts_str, frame_speeds_str;

    line_stream >> tower_name >> tower_sprite >> weapon_sprite >>
        attack_offsets_str >> attack_size_str >> frame_counts_str >>
        frame_speeds_str;

    BaseTower::TowerType type;

    if (tower_name == "ArcheryTower")
      type = BaseTower::ArcheryTower;
    else if (tower_name == "CatapultTower")
      type = BaseTower::CatapultTower;
    else if (tower_name == "SlingshotTower")
      type = BaseTower::SlingshotTower;
    else if (tower_name == "ElectroTower")
      type = BaseTower::ElectroTower;
    else if (tower_name == "OrbTower")
      type = BaseTower::OrbTower;
    else if (tower_name == "CrossbowTower")
      type = BaseTower::CrossbowTower;
    else
      throw std::runtime_error("Unknown tower type: " + tower_name);

    auto parse_int_triplet = [](const std::string& csv) {
      std::array<int, 3> values = {0, 0, 0};
      std::istringstream stream(csv);
      std::string token;
      int idx = 0;
      while (std::getline(stream, token, ',') && idx < 3) {
        values[idx++] = std::stoi(token);
      }
      if (idx != 3) throw std::runtime_error("Expected 3 comma-separated integers");
      return values;
    };

    auto attack_offsets = parse_int_triplet(attack_offsets_str);

    std::pair<int, int> attack_size;
    std::istringstream attack_size_stream(attack_size_str);
    std::string size_value;
    std::getline(attack_size_stream, size_value, ',');
    attack_size.first = std::stoi(size_value);
    std::getline(attack_size_stream, size_value, ',');
    attack_size.second = std::stoi(size_value);

    auto num_of_sprites = parse_int_triplet(frame_counts_str);
    auto shooting_sprite_indices = parse_int_triplet(frame_speeds_str);

    TowerInfo info = {tower_sprite,
                      weapon_sprite,
                      attack_offsets,
                      attack_size,
                      num_of_sprites,
                      shooting_sprite_indices};

    towers_info[type] = info;
  }

  file.close();
  std::cout << "Load Tower Info: " << towers_info.size() << " towers"
            << std::endl;
}
