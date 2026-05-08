#pragma once

#include "Interfaces/Clickable.hpp"
#include "Interfaces/Widget.hpp"
#include "PriceModal.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "Text.hpp"

class SidebarTowerButton : public Widget, public Clickable {
public:
  SidebarTowerButton(unsigned x, unsigned y, std::string tower_path, int price);

  /**
   * Overrided Methods
   */
  void handle_events(EventData data);
  void render(RenderData);
  void update(UpdateData);

  /**
   * @brief Return the global bounds
   */
  const sf::FloatRect getGlobalBounds();

private:
  sf::Sprite bg;
  sf::Texture bg_texture;
  sf::Sprite twr;
  sf::Texture twr_texture;
  std::shared_ptr<PriceModal> price_modal;
};
