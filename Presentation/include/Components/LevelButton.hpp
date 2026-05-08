#pragma once

#include "Components/IButton.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Window/Window.hpp"

#define LEVEL_NUM_COLOR 196, 176, 144
#define LEVEL_NUM_FONT_SIZE 70
class LevelButton : public IButton {
public:
  LevelButton(unsigned x, unsigned y, int num);

  void render(RenderData) override;

private:
  int num;
  sf::Text text;
  sf::Font font;
};
