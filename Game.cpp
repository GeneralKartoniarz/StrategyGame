#include "Game.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include "States.hpp"
#include "TestState.hpp"
#include <vector>
using namespace std;
Game::Game()
{
    this->InitWindow();
    this->InitStates();
}

Game::~Game()
{
    delete this->windowPtr;
}

void Game::InitWindow()
{
    this->windowPtr = new sf::RenderWindow(sf::VideoMode({1920, 1080}), "Strategy Game");
}

void Game::InitStates()
{
    this->states.push_back(std::make_unique<TestState>(this->windowPtr));
}

void Game::Run()
{
    while (this->windowPtr->isOpen())
    {
        this->dt = dtClock.restart().asSeconds();
        this->Update(this->dt);
        this->Render();
    }
}
void Game::UpdateEvent()
{
    while (const auto event = windowPtr->pollEvent())
    {
        if (!this->states.empty())
        {
            this->states.back()->HandleEvent(*event);
        }

        if (event->is<sf::Event::Closed>())
        {
            windowPtr->close();
        }
    }
}

void Game::Update(float dt)
{
    this->UpdateEvent();

    if (!states.empty())
    {
        states.back()->Update(dt);

        if (states.back()->GetQuit())
        {
            std::unique_ptr<States> next = move(states.back()->nextState);

            states.pop_back();

            if (next)
            {
                states.push_back(move(next));
            }
        }
    }
    else
    {
        this->windowPtr->close();
    }
}
void Game::Render()
{
    if (!states.empty())
    {
        this->windowPtr->clear(sf::Color::Black);
        states.back()->Render(this->windowPtr);
        this->windowPtr->display();
    }
}