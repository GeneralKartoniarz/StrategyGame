#pragma once
#include "States.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include "UI/Label.hpp"
#include "UI/Button.hpp"

class MenuState : public States
{
public:
    MenuState(sf::RenderWindow* windowPtr);
    ~MenuState() override;

    void Update(float dt) override;
    void Render(sf::RenderWindow* windowPtr) override;
    void HandleEvent(const sf::Event& event) override;

    bool requestGameState = false;

private:
    static void OnPlayClick();
    static MenuState* instance;

    std::unique_ptr<sf::Texture> bgTexture;
    std::unique_ptr<sf::Sprite> bgSprite;

    sf::Font font;
    std::unique_ptr<Label> titleLabel;
    std::unique_ptr<Button> playButton;
};